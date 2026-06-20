#!/usr/bin/env python3
"""Generate a vertical Mermaid call graph for a delinked target.

Nodes  = "touched" functions (a match % in the objdiff report: a game .c we decompiled or
         a mapped SDK function) PLUS one layer of the untouched functions they call -- the
         "frontier" = what to go after next. The frontier is not expanded, so the graph
         grows naturally as you decompile more. Flags: --touched-only drops the frontier;
         --all shows every function fully expanded.
Edges  = bl / blx / b (tail-call) / .word references between two nodes.

Node COLOR encodes match % (from the objdiff report):
    0%       gray        |  <100%   yellow-green
    <=25%    red         |  100%    lime green
    <60%     yellow

Node SHAPE:
    game, has calls       rectangle      [ name ]
    game, LEAF            stadium        ( [ name ] )
    SDK,  has calls       hexagon        {{ name }}
    SDK,  LEAF            circle         (( name ))
    FRONTIER (untouched   parallelogram  [/ name /]   (gray, dashed -- next targets)
       callee, any kind)

SDK = address present in config/sdk_mappings.json. A LEAF is a *touched* node that calls
nothing else in the graph; frontier nodes are never leaves.

Usage (pipe the report in, like `make progress` does -- or `make callgraph`):
  tools/objdiff-cli.exe report generate -p . -o - | \
      python tools/scripts/call_graph.py main_7 --report - -o main_7_callgraph.mmd

Scope it further:
  ... call_graph.py main_7 --report - --root func_02009ab8 --depth 4
  ... call_graph.py main_7 --report - --undone           # hide finished (100%) funcs
  ... call_graph.py main_7 --report - --leaves            # only leaf nodes + direct callers
  ... call_graph.py main_7 --report - --all              # every function, not just touched
  ... call_graph.py main_7 --report - --game-only        # drop mapped SDK funcs (default in `make callgraph`)
"""
import argparse
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SDK_MAPPING_FILE = ROOT / "config" / "sdk_mappings.json"

START_RE = re.compile(r"^\s*(?:arm|thumb)_func_start\s+(\S+)")
END_RE = re.compile(r"^\s*(?:arm|thumb)_func_end\s+(\S+)")
ADDR_RE = re.compile(r"^\s*\S+:\s*;\s*(0x[0-9a-fA-F]+)")
# bl + optional condition (NOT blt/ble/bls which are b<cond> branches)
BL_RE = re.compile(r"^\s*bl(?:eq|ne|cs|hs|cc|lo|mi|pl|vs|vc|hi|ls|ge|lt|gt|le|al)?\s+(\S+)")
BLX_RE = re.compile(r"^\s*blx\s+(\S+)")
B_RE = re.compile(r"^\s*b\s+(\S+)")           # unconditional tail-call branch
WORD_RE = re.compile(r"^\s*(?:[.\w]+:\s*)?\.word\s+(\S+)\s*$")  # optional local-label prefix (literal pools)

# match% -> (mermaid classDef name, fill color)
CLASSES = [
    ("done", "#64dd17"),   # 100   lime green
    ("near", "#9ccc65"),   # <100  yellow-green
    ("mid",  "#fdd835"),   # <60   yellow
    ("low",  "#e53935"),   # <=25  red
    ("none", "#bdbdbd"),   # 0     gray
]


def pct_class(pct):
    if pct is None or pct <= 0:
        return "none"
    if pct >= 100:
        return "done"
    if pct < 25 or pct == 25:
        return "low"
    if pct < 60:
        return "mid"
    return "near"


def load_sdk_addrs():
    if not SDK_MAPPING_FILE.exists():
        return set()
    data = json.loads(SDK_MAPPING_FILE.read_text(encoding="utf-8"))
    return {e["addr"].lower() for e in data.get("mappings", [])}


def parse_asm(path):
    funcs = {}            # name -> {"address": str, "targets": set[str]}
    lines = Path(path).read_text(encoding="utf-8", errors="replace").splitlines()
    cur = None
    for ln in lines:
        m = START_RE.match(ln)
        if m:
            cur = m.group(1)
            funcs[cur] = {"address": "", "targets": set()}
            continue
        if END_RE.match(ln):
            cur = None
            continue
        if cur is None:
            continue
        info = funcs[cur]
        if not info["address"]:
            am = ADDR_RE.match(ln)
            if am:
                info["address"] = am.group(1)
        for rx in (BL_RE, BLX_RE, B_RE):
            cm = rx.match(ln)
            if cm:
                info["targets"].add(cm.group(1))
                break
        wm = WORD_RE.match(ln)
        if wm:
            info["targets"].add(wm.group(1))
    return funcs


def parse_report(path):
    report = json.load(sys.stdin) if path == "-" else json.loads(Path(path).read_text(encoding="utf-8"))
    out = {}
    for unit in report.get("units", []):
        for fn in unit.get("functions", []):
            pct = fn.get("fuzzy_match_percent")
            if pct is None:
                continue
            n = fn["name"]
            if n not in out or pct > out[n]:
                out[n] = pct
    return out


def node_text(nid, name, is_sdk, is_leaf, is_frontier):
    label = f'"{name}"'
    if is_frontier:                       # untouched callee: next target
        return f"{nid}[/{label}/]"        # parallelogram
    if is_sdk and is_leaf:
        return f"{nid}(({label}))"
    if is_sdk:
        return f"{nid}{{{{{label}}}}}"
    if is_leaf:
        return f"{nid}([{label}])"
    return f"{nid}[{label}]"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("target", help="e.g. main_7 (-> asm/_dsd_gap@<target>.s)")
    ap.add_argument("--asm", help="override .s path")
    ap.add_argument("--report", default="-", help="objdiff report json, or - for stdin")
    ap.add_argument("--out", "-o", help="output .mmd (default: <target>_callgraph.mmd)")
    ap.add_argument("--root", help="only graph the subtree reachable from this function")
    ap.add_argument("--depth", type=int, default=0, help="max depth for --root (0 = unlimited)")
    ap.add_argument("--undone", action="store_true", help="hide functions already at 100%")
    ap.add_argument("--leaves", action="store_true", help="only leaf nodes and their direct callers")
    ap.add_argument("--all", action="store_true",
                    help="include EVERY function (touched + all untouched), fully expanded")
    ap.add_argument("--touched-only", action="store_true",
                    help="touched functions only, no frontier layer")
    ap.add_argument("--connected-only", action="store_true",
                    help="drop nodes with no edge in the final graph (lone nodes)")
    ap.add_argument("--game-only", action="store_true",
                    help="exclude mapped SDK functions (config/sdk_mappings.json); show game functions only")
    ap.add_argument("--max-nodes", type=int, default=600, help="warn above this node count")
    args = ap.parse_args()

    asm_path = args.asm or str(ROOT / "asm" / f"_dsd_gap@{args.target}.s")
    out_path = args.out or str(ROOT / f"{args.target}_callgraph.mmd")

    funcs = parse_asm(asm_path)
    pcts = parse_report(args.report)
    sdk_addrs = load_sdk_addrs()

    # "touched" = has a match % (a game .c file or a mapped SDK function).
    touched = {n for n in funcs if n in pcts}
    sdk_nodes = {n for n in funcs if funcs[n]["address"].lower() in sdk_addrs}
    if args.all:
        nodes, frontier_nodes = set(funcs), set()
    elif args.touched_only:
        nodes, frontier_nodes = set(touched), set()
    else:
        # DEFAULT: touched functions + ONE layer of the untouched functions they call
        # (the "frontier" = what to go after next). Frontier nodes are NOT expanded, so
        # the graph shows where each trail leads but stops there.
        frontier_nodes = {t for n in touched for t in funcs[n]["targets"]
                          if t in funcs and t not in touched}
        nodes = touched | frontier_nodes

    # frontier nodes are terminal: only touched nodes (or every node, with --all) emit edges
    expanders = nodes if args.all else (nodes - frontier_nodes)
    edges = set()
    out_adj = {n: set() for n in nodes}
    in_adj = {n: set() for n in nodes}
    for name in expanders:
        for t in funcs[name]["targets"]:
            if t in nodes and t != name:
                edges.add((name, t))
                out_adj[name].add(t)
                in_adj[t].add(name)

    # a leaf is a non-frontier node that calls nothing else in the graph
    leaves = {n for n in nodes if n not in frontier_nodes and not out_adj[n]}

    # ---- scope selection ----
    keep = set(nodes)
    if args.root:
        if args.root not in nodes:
            sys.exit(f"--root {args.root} not found in {asm_path}")
        keep, frontier, d = set(), {args.root}, 0
        while frontier and (args.depth == 0 or d <= args.depth):
            keep |= frontier
            frontier = {t for n in frontier for t in out_adj[n]} - keep
            d += 1
    if args.leaves:
        keep &= (leaves | {c for lf in leaves for c in in_adj[lf]})
    if args.undone:
        keep = {n for n in keep if pcts.get(n, 0) < 100}
    if args.game_only:
        keep -= sdk_nodes

    keep_edges = [(a, b) for (a, b) in edges if a in keep and b in keep]

    if args.connected_only:
        linked = {n for e in keep_edges for n in e}
        keep &= linked
        keep_edges = [(a, b) for (a, b) in keep_edges if a in keep and b in keep]

    # recompute leaves against the FINAL graph: a node whose edges were all
    # filtered out (e.g. game func that only called now-dropped SDK nodes) is a leaf
    kept_out = {n: set() for n in keep}
    for a, b in keep_edges:
        kept_out[a].add(b)
    leaves = {n for n in keep if n not in frontier_nodes and not kept_out[n]}

    if len(keep) > args.max_nodes:
        print(f"WARNING: {len(keep)} nodes (> --max-nodes {args.max_nodes}); "
              f"consider --root/--undone/--leaves to scope it down.", file=sys.stderr)

    # ---- emit mermaid ----
    nid = {n: "n%d" % i for i, n in enumerate(sorted(keep))}
    by_class = {c: [] for c, _ in CLASSES}
    by_class["frontier"] = []
    L = ["flowchart TB"]
    for cname, color in CLASSES:
        L.append(f"  classDef {cname} fill:{color},stroke:#222,color:#000")
    L.append("  classDef frontier fill:#fafafa,stroke:#888,stroke-width:1px,"
             "stroke-dasharray:5 3,color:#555")
    for n in sorted(keep):
        is_front = n in frontier_nodes
        is_sdk = funcs[n]["address"].lower() in sdk_addrs
        is_leaf = n in leaves
        L.append("  " + node_text(nid[n], n, is_sdk, is_leaf, is_front))
        by_class["frontier" if is_front else pct_class(pcts.get(n))].append(nid[n])
    for a, b in sorted(keep_edges):
        L.append(f"  {nid[a]} --> {nid[b]}")
    for cname in [c for c, _ in CLASSES] + ["frontier"]:
        if by_class[cname]:
            L.append(f"  class {','.join(by_class[cname])} {cname}")

    Path(out_path).write_text("\n".join(L) + "\n", encoding="utf-8")
    n_leaf = len(keep & leaves)
    n_front = len(keep & frontier_nodes)
    print(f"wrote {out_path}: {len(keep)} nodes "
          f"({n_leaf} leaves, {n_front} frontier/next-targets), {len(keep_edges)} edges",
          file=sys.stderr)


if __name__ == "__main__":
    main()
