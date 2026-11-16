import argparse, collections, glob, json, sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import cc, pcdump, pcsim  # noqa: E402


def graph_diff(rec, g, r):
    """[(node, kind)] where the model graph differs from the real one"""
    out = []
    for k in range(len(rec["nodes"])):
        a, b = dict(g.succ[k]), dict(r.succ[k])
        if a != b:
            fl = rec["nodes"][k]["instr"]["flags"]
            extra = sorted(set(a) - set(b)); miss = sorted(set(b) - set(a))
            lat = sorted(t for t in set(a) & set(b) if a[t] != b[t])
            out.append((k, "mem" if fl & (pcsim.F_LOAD | pcsim.F_STORE) else "other", extra, miss, lat))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("tus", nargs="*")
    ap.add_argument("--variant", default="stock")
    ap.add_argument("--log", default=str(cc.ROOT / "build/schedsim/parity.log"))
    ap.add_argument("--max-examples", type=int, default=40)
    a = ap.parse_args()
    tus = a.tus or sorted(glob.glob(str(cc.ROOT / "arm9/src/**/*.c"), recursive=True) + glob.glob(str(cc.ROOT / "arm9/src/**/*.cpp"), recursive=True))
    Path(a.log).parent.mkdir(parents=True, exist_ok=True)
    log = open(a.log, "w")
    tot = collections.Counter()
    ex = [0]
    for tu in tus:
        st = collections.Counter()

        def rec(r, tu=tu, st=st):
            p = "p1" if r["vregs"] else "p2"
            n = len(r["nodes"])
            st[p + "_blocks"] += 1
            if sorted(r.get("out", [])) != list(range(n)):
                st[p + "_badout"] += 1
                return
            press = bool(r["vregs"]) and r["live"] >= r["avail"]
            real = pcsim.real_graph(r)
            pick_ok = pcsim.schedule(r, real, pcsim.Pressure(r) if r["vregs"] else None) == r["out"]
            model = pcsim.model_graph(r)
            gd = graph_diff(r, model, real)
            e2e_ok = pcsim.schedule(r, model, pcsim.Pressure(r) if r["vregs"] else None) == r["out"]
            if press:
                st[p + "_pressure"] += 1
            st[p + "_pick_ok"] += pick_ok
            st[p + "_graph_ok"] += not gd
            st[p + "_e2e_ok"] += e2e_ok
            if press and not pick_ok:
                st[p + "_pick_fail_pressure"] += 1
            for k, kind, extra, miss, lat in gd:
                st[p + "_edgediff_" + kind] += 1
            if (not pick_ok or gd) and ex[0] < a.max_examples:
                ex[0] += 1
                log.write("== %s block#%d %s n=%d pick_ok=%s pressure=%s live=%d avail=%d\n" % (Path(tu).name, st[p + "_blocks"], p, n, pick_ok, press, r["live"], r["avail"]))
                if not pick_ok:
                    sim = pcsim.schedule(r, real, pcsim.Pressure(r) if r["vregs"] else None)
                    i = next(i for i in range(n) if sim[i] != r["out"][i])
                    log.write("   first divergence at pick %d: sim %d (%s) real %d (%s)\n" % (i, sim[i], pcsim.fmt(r["nodes"][sim[i]]["instr"]), r["out"][i], pcsim.fmt(r["nodes"][r["out"][i]]["instr"])))
                for k, kind, extra, miss, lat in gd[:6]:
                    log.write("   graph %s node %d %s fl=%x: extra %s missing %s lat %s\n" % (kind, k, pcsim.fmt(r["nodes"][k]["instr"]), r["nodes"][k]["instr"]["flags"], extra, miss, lat))
                if not pick_ok or gd:
                    log.write("   " + json.dumps({"nodes": [[pcsim.fmt(nd["instr"]), nd["L"], nd["h"], nd["instr"]["alias"]] for nd in r["nodes"]]})[:1500] + "\n")
        try:
            pcdump.dump_tu(tu, a.variant, rec)
        except Exception as e:
            print("SKIP", Path(tu).name, str(e)[:100], flush=True)
            continue
        tot += st
        print(Path(tu).name, dict(st), flush=True)
    print("TOTAL", dict(sorted(tot.items())))
    for p in ("p1", "p2"):
        b = tot[p + "_blocks"] - tot[p + "_badout"]
        if b:
            print("%s: %d blocks | pick on real graph %.2f%% | graph %.2f%% | end-to-end %.2f%%" % (p, b, 100 * tot[p + "_pick_ok"] / b, 100 * tot[p + "_graph_ok"] / b, 100 * tot[p + "_e2e_ok"] / b))
    log.close()


if __name__ == "__main__":
    main()
