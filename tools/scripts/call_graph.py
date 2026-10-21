#!/usr/bin/env python3
import argparse
import re
import sys
from pathlib import Path

from build_todo import asm_functions, first_addr, parse_report
from common import ROOT, load_json, sdk_mappings

BL_RE = re.compile(r"^\s*bl(?:eq|ne|cs|hs|cc|lo|mi|pl|vs|vc|hi|ls|ge|lt|gt|le|al)?\s+(\S+)")
BLX_RE = re.compile(r"^\s*blx\s+(\S+)")
B_RE = re.compile(r"^\s*b\s+(\S+)")
WORD_RE = re.compile(r"^\s*(?:[.\w]+:\s*)?\.word\s+(\S+)\s*$")

CLASSES = [
    ("done", "#64dd17"),
    ("near", "#9ccc65"),
    ("mid",  "#fdd835"),
    ("low",  "#e53935"),
    ("none", "#bdbdbd"),
]


def pct_class(pct):
    if pct is None or pct <= 0:
        return "none"
    if pct >= 100:
        return "done"
    if pct <= 25:
        return "low"
    if pct < 60:
        return "mid"
    return "near"


def call_targets(body):
    out = set()
    for ln in body:
        cm = BL_RE.match(ln) or BLX_RE.match(ln) or B_RE.match(ln)
        if cm:
            out.add(cm.group(1))
        wm = WORD_RE.match(ln)
        if wm:
            out.add(wm.group(1))
    return out


def parse_asm(path):
    return {name: {"address": first_addr(body), "targets": call_targets(body)}
            for name, body in asm_functions(path)}


def best_pcts(report):
    return {name: max(pct for _unit, pct in slot.values())
            for name, slot in parse_report(report).items()}


def node_text(nid, name, is_sdk, is_leaf, is_frontier, is_caller=False):
    label = f'"{name}"'
    if is_caller:
        return f"{nid}[\\{label}\\]"
    if is_frontier:
        return f"{nid}[/{label}/]"
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
    ap.add_argument("--undone", action="store_true", help="hide functions already at 100%%")
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
    pcts = best_pcts(load_json(args.report))
    sdk_addrs = {e["addr"].lower() for e in sdk_mappings()}

    touched = {n for n in funcs if n in pcts}
    sdk_nodes = {n for n in funcs if funcs[n]["address"].lower() in sdk_addrs}
    caller_frontier = set()
    if args.all:
        nodes, frontier_nodes = set(funcs), set()
    elif args.touched_only:
        nodes, frontier_nodes = set(touched), set()
    else:
        frontier_nodes = {t for n in touched for t in funcs[n]["targets"]
                          if t in funcs and t not in touched}
        nodes = touched | frontier_nodes

    if args.callers and not args.all:
        # a node in both frontiers keeps callee style
        caller_frontier = {c for c in funcs if c not in touched
                           and any(t in touched for t in funcs[c]["targets"])}
        caller_frontier -= frontier_nodes
        frontier_nodes |= caller_frontier
        nodes |= caller_frontier

    expanders = nodes if args.all else (nodes - frontier_nodes)
    edges = set()
    out_adj = {n: set() for n in nodes}
    in_adj = {n: set() for n in nodes}

    def add_edges(src, allowed):
        for t in funcs[src]["targets"]:
            if t in allowed and t != src:
                edges.add((src, t))
                out_adj[src].add(t)
                in_adj[t].add(src)

    for name in expanders:
        add_edges(name, nodes)
    for c in caller_frontier:
        add_edges(c, touched)

    leaves = {n for n in nodes if n not in frontier_nodes and not out_adj[n]}

    keep = set(nodes)
    if args.root:
        if args.root not in nodes:
            sys.exit(f"--root {args.root} not found in {asm_path}")
        keep, frontier, d = set(), {args.root}, 0
        while frontier and (args.depth == 0 or d <= args.depth):
            keep |= frontier
            frontier = {t for n in frontier for t in out_adj[n]} - keep
            d += 1
        if args.callers:
            keep |= {c for c in in_adj[args.root] if c in nodes}
    if args.leaves:
        keep &= (leaves | {c for lf in leaves for c in in_adj[lf]})
    if args.undone:
        keep = {n for n in keep if pcts.get(n, 0) < 100}
    if args.game_only:
        keep -= sdk_nodes

    keep_edges = [(a, b) for (a, b) in edges if a in keep and b in keep]

    if args.connected_only:
        keep &= {n for e in keep_edges for n in e}

    # recompute: filtering can strip a node's last edge
    has_out = {a for a, _ in keep_edges}
    leaves = {n for n in keep if n not in frontier_nodes and n not in has_out}

    if len(keep) > args.max_nodes:
        print(f"WARNING: {len(keep)} nodes (> --max-nodes {args.max_nodes}); "
              f"consider --root/--undone/--leaves to scope it down.", file=sys.stderr)

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
        L.append("  " + node_text(nid[n], n, is_sdk, n in leaves, is_front, is_caller))
        if is_caller:
            by_class["caller"].append(nid[n])
        elif is_front:
            by_class["frontier"].append(nid[n])
        else:
            by_class[pct_class(pcts.get(n))].append(nid[n])
    for a, b in sorted(keep_edges):
        L.append(f"  {nid[a]} --> {nid[b]}")
    for cname, ids in by_class.items():
        if ids:
            L.append(f"  class {','.join(ids)} {cname}")

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
