import argparse, json, re, sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import pcsim  # noqa: E402


def trace(rec, g):
    """re-run the pick, recording for every pick the issuable candidates and the deciding level"""
    nodes = rec["nodes"]
    n = len(nodes)
    maxh = max(g.h)
    dl = [maxh - h for h in g.h]
    npred = list(g.npred); ready = [0] * n; done = [False] * n
    P = pcsim.Pressure(rec) if rec["vregs"] else None
    out = []
    cycle = busy = 0
    while len(out) < n:
        if busy == 0:
            cands = [c for c in range(n) if not done[c] and not npred[c] and ready[c] <= cycle]
            if cands:
                rel = {c: sum(1 for t, _ in g.succ[c] if npred[t] == 1) for c in cands}
                best, why = cands[0], "first"
                if P is not None and P.active():
                    for c in cands[1:]:
                        if P.cost(c) < P.cost(best):
                            best, why = c, "pressure cost"
                else:
                    for c in cands[1:]:
                        bd, cd = dl[best] <= cycle, dl[c] <= cycle
                        if bd != cd:
                            if cd:
                                best, why = c, "urgency"
                        elif rel[best] != rel[c]:
                            if rel[c] > rel[best]:
                                best, why = c, "release"
                        elif g.h[best] != g.h[c]:
                            if g.h[c] > g.h[best]:
                                best, why = c, "height"
                        elif rec["vregs"] and nodes[best]["desc"] > nodes[c]["desc"]:
                            best, why = c, "descriptor"
                out.append((cycle, best, why, [(c, dl[c], rel[c], g.h[c], P.cost(c) if P else None) for c in cands]))
                done[best] = True
                for t, lat in g.succ[best]:
                    npred[t] -= 1
                    ready[t] = max(ready[t], cycle + lat)
                busy = pcsim.cycles(nodes[best])
                if P is not None:
                    P.issue(best)
        if busy:
            busy -= 1
        cycle += 1
    return out


def main():
    ap = argparse.ArgumentParser(description="inspect scheduler blocks from a pcdump .jsonl")
    ap.add_argument("dump")
    ap.add_argument("blocks", nargs="*", type=int, help="record indices (default: list all)")
    ap.add_argument("--grep", help="only blocks containing an instruction matching this regex")
    ap.add_argument("--picks", action="store_true", help="show every pick with its candidates")
    ap.add_argument("--edges", action="store_true", help="show successor edges")
    a = ap.parse_args()
    rows = [json.loads(l) for l in open(a.dump)]
    for k, r in enumerate(rows):
        txt = [pcsim.fmt(nd["instr"]) for nd in r["nodes"]]
        if a.blocks and k not in a.blocks:
            continue
        if a.grep and not any(re.search(a.grep, t) for t in txt):
            continue
        g = pcsim.real_graph(r)
        print("== block %d  pass %d  %d instrs  moved=%s  live=%d avail=%d" % (k, 1 if r["vregs"] else 2, len(txt), r["out"] != list(range(len(txt))), r["live"], r["avail"]))
        if not a.blocks:
            continue
        al = r["aliases"]
        pos = {n: i for i, n in enumerate(r["out"])}
        for i, nd in enumerate(r["nodes"]):
            ar = al.get(str(nd["instr"]["alias"]))
            tag = "" if ar is None else ("W" if nd["instr"]["alias"] == r["worst"] else "obj%x+%d/%d" % (ar["obj"] & 0xffff, ar["off"], ar["size"]))
            e = "  -> " + " ".join("%d:%d" % (t, l) for t, l in sorted(g.succ[i])) if a.edges else ""
            print("%3d -> %3d  %-40s L%d h%-3d %-12s%s" % (i, pos.get(i, -1), txt[i], nd["L"], nd["h"], tag, e))
        if a.picks:
            for cyc, best, why, cands in trace(r, g):
                print(" cycle %3d pick %3d %-36s by %s" % (cyc, best, txt[best], why))
                if len(cands) > 1:
                    for c, d, rl, h, cost in cands:
                        print("        %s %3d %-34s deadline %d release %d height %d%s" % ("*" if c == best else " ", c, txt[c], d, rl, h, "" if cost is None else " cost %d" % cost))


if __name__ == "__main__":
    main()
