#!/usr/bin/env python3
"""Classify a delinked target function as handwritten ASM vs compiled C.

Usage: asm_smell.py <symbol> [<symbol>...] [--target PATH]

Hard tells (any one => ASM, mwcc cannot emit it):
  H1  restores lr from stack then `bx lr` (mwcc always pops to pc)
  H2  frame push after the prologue (push/pop around an individual call)
  H3  conditional ldm/stm (multi-reg transfer under a condition code)
  H4  conditional instruction before any flag-setter (uses caller's flags)
  H5  swp/mrs/msr/mcr/mrc
  H6  computed pc dispatch into non-branch rows (mwcc tables are all `b`)

Compiler tells (evidence of C) are counted and reported.
Exit: 0 = looks C, 1 = ASM (hard tell), 2 = error. Verdict is advisory.

SCOPE: hard tells prove "not compiled by OUR mwcc config". In game-code TUs
that means handwritten asm. In SDK/MSL/vendor-lib regions it can instead mean
vendor-built C with a different frame convention (e.g. FS_OpenFileFast ends
`pop {r4,lr}; add sp,#16; bx lr` -- older CW, sub-sp-before-push frames).
Route those through the SDK pipeline, not transcription.
"""
import argparse
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SRC_ROOT = ROOT / "arm9" / "src"
OBJDUMP = "arm-none-eabi-objdump"
SYM_RE = re.compile(r"^[0-9a-f]+ <([^>]+)>:")
INSN_RE = re.compile(r"^\s*[0-9a-f]+:\s*[0-9a-f]{4,8}\s+(\S.*)$")
COND = "eq|ne|cs|cc|mi|pl|vs|vc|hi|ls|ge|lt|gt|le"
# mnemonic root + optional cond + optional size/order suffix, both orders
LDMSTM_COND = re.compile(rf"^(ldm|stm)(({COND})(ia|ib|da|db)?|(ia|ib|da|db)({COND}))\b")
POP_LR = re.compile(r"^(pop|ldm\w*\s+sp!,)\s*.*\blr\b")
PUSH = re.compile(r"^(push|stmdb\s+sp!|stmfd\s+sp!)")
FLAG_SET = re.compile(
    rf"^(cmp|cmn|tst|teq)(\s|({COND})\s)"
    rf"|^(add|adc|sub|sbc|rsb|rsc|and|orr|eor|bic|mov|mvn|lsl|lsr|asr|ror|mul|mla"
    rf"|smull|smlal|umull|umlal)({COND})?s\s"
)
CONDITIONAL = re.compile(
    rf"^([a-z]{{2,6}}?)({COND})(s|ia|ib|da|db|b|h|sb|sh)?\s"
)
UNCOND_ROOTS = {"b", "bl", "bx", "blx"}  # bare branches: 'b' + cond handled below
SYSTEM = re.compile(r"^(swp|mrs|msr|mcr|mrc)\b")
C_TELLS = {
    "pop_pc": re.compile(r"^(pop|ldm\w*\s+sp!,)\s*.*\bpc\b"),
    "pad_push": re.compile(r"^(push|stmdb\s+sp!,)\s*\{r3,"),
    "switch_table": re.compile(r"^addls\s+pc,\s*pc,"),
}


def objdump(path):
    r = subprocess.run([OBJDUMP, "-d", str(path)], capture_output=True, text=True)
    if r.returncode != 0:
        print(f"error: objdump failed: {r.stderr.strip()}", file=sys.stderr)
        sys.exit(2)
    return r.stdout.splitlines()


def extract(lines, symbol):
    out, active, found = [], False, False
    for line in lines:
        m = SYM_RE.match(line)
        if m:
            name = m.group(1)
            if name == symbol and not found:
                active, found = True, True
                continue
            if active and not name.startswith(".L"):
                break  # first instance only (duplicate names exist, e.g. BeginSleep)
            continue
        if active:
            im = INSN_RE.match(line)
            if im and not im.group(1).startswith((".word", ".short", ".byte")):
                out.append(im.group(1).strip())
    return out, found


def is_conditional(insn):
    mnem = insn.split()[0]
    if mnem in UNCOND_ROOTS or mnem.startswith(".") :
        return False
    # bare conditional branch (beq, bxls, blne...)
    m = re.match(rf"^(b|bl|bx|blx)({COND})$", mnem)
    if m:
        return True
    m = CONDITIONAL.match(insn)
    if not m:
        return False
    root = m.group(1)
    # avoid false hits: e.g. 'smi'..., real roots that end like a cond
    return root in {
        "add", "adc", "sub", "sbc", "rsb", "rsc", "and", "orr", "eor", "bic",
        "mov", "mvn", "cmp", "cmn", "tst", "teq", "mul", "mla", "ldr", "str",
        "ldm", "stm", "lsl", "lsr", "asr", "ror", "smull", "smlal", "umull",
        "umlal", "clz", "ldrb", "strb", "ldrh", "strh", "ldrsb", "ldrsh",
        "push", "pop",
    }


# mwcc homes r0-r3 when a register param is address-taken; sp must then be
# unwound past the block, so the epilogue pops lr (not pc).
ARG_HOME = re.compile(r"^push\s+\{r0,\s*r1,\s*r2,\s*r3\}$")
ADD_SP = re.compile(rf"^add({COND})?\s+sp,\s*sp,\s*#")


def classify(insns):
    hard, ctell = [], []
    seen_flag_set = False
    prologue_done = False
    arg_home = bool(insns) and ARG_HOME.match(insns[0])
    for i, insn in enumerate(insns):
        mnem = insn.split()[0]
        if PUSH.match(insn):
            if prologue_done and i > 0:
                hard.append(f"H2 mid-function push @insn {i}: {insn}")
        elif not insn.startswith("sub\tsp") and not insn.startswith("sub sp"):
            if i > 0 or not PUSH.match(insn):
                prologue_done = True
        if LDMSTM_COND.match(insn) and "{" in insn and "," in insn.split("{")[-1]:
            hard.append(f"H3 conditional block transfer @insn {i}: {insn}")
        if SYSTEM.match(insn):
            hard.append(f"H5 system insn @insn {i}: {insn}")
        if not seen_flag_set and is_conditional(insn):
            hard.append(f"H4 conditional before any flag-setter @insn {i}: {insn}")
        if FLAG_SET.match(insn):
            seen_flag_set = True
        for name, rx in C_TELLS.items():
            if rx.match(insn):
                ctell.append(f"{name} @insn {i}")
        # H6: computed pc dispatch; mwcc follows with `b default` + all-b rows
        if re.match(rf"^add({COND})?\s+pc,\s*pc,", insn):
            rows = insns[i + 1 : i + 6]
            nb = sum(1 for r in rows if r.split()[0] == "b")
            if nb < len(rows) - 1:
                hard.append(f"H6 pc-dispatch into computation @insn {i}: {insn}")
        # H1: lr restored from stack, later bx lr
        if POP_LR.match(insn):
            rest = insns[i + 1 : i + 4]
            if any(r.startswith("bx") and r.endswith("lr") for r in rest):
                if not (arg_home and any(ADD_SP.match(r) for r in rest)):
                    hard.append(f"H1 lr-restore + bx lr @insn {i}: {insn}")
    return hard, ctell


ASM_DEF_RE = re.compile(r"^\s*(?:static\s+)?asm\s+[\w:*&\s]*?\b(\w+)\s*\(")


def matched_symbols(todo=ROOT / "main_7_todo.md"):
    done = set()
    if todo.is_file():
        for line in todo.read_text(encoding="utf-8").splitlines():
            m = re.match(r"\| \[x\] \| 0x[0-9a-f]+ \| `([^`]+)`", line)
            if m:
                done.add(m.group(1).strip())
    return done


def transcribed_symbols():
    """Functions already written as `asm void f(...)` in the tree.

    The warning's whole advice is "transcribe this as asm" -- once that's done it
    is noise, and it fires anyway whenever main_7_todo.md is stale (only `make
    progress` regenerates it, so `make diff` reads whatever the last run left)."""
    out = set()
    for src in list(SRC_ROOT.rglob("*.c")) + list(SRC_ROOT.rglob("*.cpp")):
        for line in src.read_text(encoding="utf-8", errors="replace").splitlines():
            m = ASM_DEF_RE.match(line)
            if m:
                out.add(m.group(1))
    return out


def todo_is_stale(todo=ROOT / "main_7_todo.md"):
    """main_7_todo.md older than the newest compiled object => its [x] marks
    predate the current build, so 'unmatched' can't be trusted."""
    if not todo.is_file():
        return True
    objs = list((ROOT / "build/arm9/src").rglob("*.o"))
    if not objs:
        return False
    return todo.stat().st_mtime < max(o.stat().st_mtime for o in objs)


def compiled_symbols():
    """Function symbols defined in compiled game TUs (= what objdiff pairs)."""
    syms = set()
    objs = sorted((ROOT / "build/arm9/src").rglob("*.o"))
    if not objs:
        return syms
    # ONE nm over every object instead of a spawn each. Its per-file header lines
    # ("foo.o:") split to 1 part, so the len==3 filter drops them already.
    r = subprocess.run(["arm-none-eabi-nm", "--defined-only", *map(str, objs)],
                       capture_output=True, text=True)
    for line in r.stdout.splitlines():
        parts = line.split()
        if len(parts) == 3 and parts[1] in ("T", "t"):
            syms.add(parts[2])
    return syms


def sweep(lines):
    """Warn about unmatched-but-compiled gap functions with hard asm tells.
    Scope = symbols in build/arm9/src .o files (active TUs), not the whole gap.
    Always exit 0."""
    done = matched_symbols() | transcribed_symbols()
    active = compiled_symbols()
    flagged = []
    syms = [m.group(1) for l in lines if (m := SYM_RE.match(l))
            and not m.group(1).startswith(".L")]
    seen = set()
    for s in syms:
        if s in seen or s in done or s not in active:
            continue
        seen.add(s)
        insns, _ = extract(lines, s)
        if insns:
            hard, _ = classify(insns)
            if hard:
                kinds = []  # distinct smells, first-hit order, descriptions kept
                for h in hard:
                    k = h.split(" @")[0]
                    if k not in kinds:
                        kinds.append(k)
                flagged.append((s, "; ".join(kinds)))
    if flagged:
        def quarantined(s):
            m = re.match(r"func_(0201d[0-9a-f]{3})$", s)
            return m and 0x0201d0bc <= int(m.group(1), 16) < 0x0201de08
        q = sum(1 for s, _ in flagged if quarantined(s))
        flagged = [(s, h) for s, h in flagged if not quarantined(s)]
        game = [(s, h) for s, h in flagged if s.startswith("func_") or s.islower()]
        vendor = len(flagged) - len(game)
        print(f"ASM-SMELL WARNING: {len(flagged)} unmatched functions have hard "
              f"asm tells -- do NOT decomp these as C:")
        for s, h in game:
            print(f"  {s:28s} {h}  (transcribe as asm)")
        if vendor:
            print(f"  (+{vendor} SDK/vendor-named -- route via sdk pipeline)")
        if q:
            print(f"  (+{q} in quarantined graphics lib -- do not touch)")
        if todo_is_stale():
            print("  NOTE: main_7_todo.md predates the current build (only `make "
                  "progress` regenerates it) -- some of these may already match.")
    sys.exit(0)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("symbols", nargs="*")
    ap.add_argument("--target", default=str(ROOT / "build/delinks/_dsd_gap@main_7.o"))
    ap.add_argument("--sweep", action="store_true",
                    help="scan all unmatched gap functions, warn on hard tells")
    args = ap.parse_args()
    lines = objdump(Path(args.target))
    if args.sweep:
        sweep(lines)
    if not args.symbols:
        ap.error("need symbols or --sweep")
    any_asm = False
    for sym in args.symbols:
        insns, found = extract(lines, sym)
        if not found or not insns:
            print(f"{sym}: NOT FOUND / empty")
            continue
        hard, ctell = classify(insns)
        if hard:
            any_asm = True
            verdict = "ASM"
        elif ctell:
            verdict = "C (compiler tells present)"
        else:
            verdict = "no hard tells (inspect soft tells manually)"
        print(f"{sym}: {verdict}  [{len(insns)} insns]")
        for h in hard:
            print(f"    {h}")
        for c in ctell[:3]:
            print(f"    (C) {c}")
    sys.exit(1 if any_asm else 0)


if __name__ == "__main__":
    main()
