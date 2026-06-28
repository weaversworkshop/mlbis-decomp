#!/usr/bin/env python3
"""Vertical Mermaid call graph for a delinked target: touched funcs (have an
objdiff match %) + one frontier layer of their untouched callees = what to do
next. Color = match %; shape = game/SDK/leaf/frontier.
Usage: call_graph.py <target> --report <json> [-o F] [--root FUNC] [--depth N]
       [--all] [--undone] [--leaves]."""
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


def node_text(nid, name, is_sdk, is_leaf, is_frontier, is_caller=False):
    label = f'"{name}"'
    if is_caller:                         # untouched caller: inbound next target
        return f"{nid}[\\{label}\\]"      # reverse parallelogram
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
    ap.add_argument("--callers", action="store_true",
                    help="also include untouched functions that CALL touched ones "
                         "(inbound 'caller frontier'); pairs well with --root for picking next targets")
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
    caller_frontier = set()
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

    # --callers: inbound mirror of the callee frontier. Untouched functions that call into
    # a touched node -- "what becomes tractable now that this is understood". Terminal like
    # the callee frontier (not expanded). A node that is ALSO a callee frontier keeps its
    # callee identity (downstream is the primary meaning); only pure callers get caller style.
    if args.callers and not args.all:
        caller_frontier = {c for c in funcs if c not in touched
                           and any(t in touched for t in funcs[c]["targets"])}
        caller_frontier -= frontier_nodes
        frontier_nodes |= caller_frontier
        nodes |= caller_frontier

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

    # caller frontier is terminal (not in expanders), so emit only its single inbound edge
    # into the touched node that pulled it in -- not its other (off-graph) calls.
    for c in caller_frontier:
        for t in funcs[c]["targets"]:
            if t in touched and t != c:
                edges.add((c, t))
                out_adj[c].add(t)
                in_adj[t].add(c)

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
        if args.callers:                       # also pull in the root's direct callers
            keep |= {c for c in in_adj[args.root] if c in nodes}
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
    by_class["caller"] = []
    L = ["flowchart TB"]
    for cname, color in CLASSES:
        L.append(f"  classDef {cname} fill:{color},stroke:#222,color:#000")
    L.append("  classDef frontier fill:#fafafa,stroke:#888,stroke-width:1px,"
             "stroke-dasharray:5 3,color:#555")
    L.append("  classDef caller fill:#b3e5fc,stroke:#0277bd,stroke-width:1px,"
             "stroke-dasharray:5 3,color:#013")
    for n in sorted(keep):
        is_caller = n in caller_frontier
        is_front = n in frontier_nodes and not is_caller
        is_sdk = funcs[n]["address"].lower() in sdk_addrs
        is_leaf = n in leaves
        L.append("  " + node_text(nid[n], n, is_sdk, is_leaf, is_front, is_caller))
        if is_caller:
            by_class["caller"].append(nid[n])
        elif is_front:
            by_class["frontier"].append(nid[n])
        else:
            by_class[pct_class(pcts.get(n))].append(nid[n])
    for a, b in sorted(keep_edges):
        L.append(f"  {nid[a]} --> {nid[b]}")
    for cname in [c for c, _ in CLASSES] + ["frontier", "caller"]:
        if by_class[cname]:
            L.append(f"  class {','.join(by_class[cname])} {cname}")

    Path(out_path).write_text("\n".join(L) + "\n", encoding="utf-8")
    n_leaf = len(keep & leaves)
    n_caller = len(keep & caller_frontier)
    n_front = len(keep & frontier_nodes) - n_caller
    print(f"wrote {out_path}: {len(keep)} nodes "
          f"({n_leaf} leaves, {n_front} callee-frontier, {n_caller} caller-frontier), "
          f"{len(keep_edges)} edges",
          file=sys.stderr)


if __name__ == "__main__":
    main()
