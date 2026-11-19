import argparse, collections, glob, json, sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import cc, coldump, colsim  # noqa: E402

CATS = ("graph", "cost", "push", "colour", "post")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("tus", nargs="*")
    ap.add_argument("--log", default=str(cc.ROOT / "build/schedsim/colparity.log"))
    a = ap.parse_args()
    tus = a.tus or sorted(glob.glob(str(cc.ROOT / "arm9/src/**/*.c"), recursive=True) + glob.glob(str(cc.ROOT / "arm9/src/**/*.cpp"), recursive=True))
    Path(a.log).parent.mkdir(parents=True, exist_ok=True)
    log = open(a.log, "w")
    tot = collections.Counter()
    skipped, failed = [], []
    for tu in tus:
        name = Path(tu).name
        src = coldump.scratch(tu)
        norm = coldump.WORK / "obj" / (src.stem + ".o")
        try:
            cc.compile(tu, "stock", norm, src)
        except Exception as e:
            skipped.append(name)
            print("SKIP %-36s %s" % (name, str(e).splitlines()[0][:90]), flush=True)
            log.write("== SKIP %s\n%s\n" % (tu, e))
            continue
        out = coldump.WORK / "dumps" / (src.stem + ".jsonl")
        nr, nf, obj = coldump.dump(tu, out, src)
        st = collections.Counter(tus=1, funcs=nf)
        if obj is None or obj.read_bytes() != norm.read_bytes():
            st["object_bad"] += 1
            log.write("== %s: object under the debugger %s\n" % (tu, "missing" if obj is None else "differs from the normal compile"))
        for line in open(out):
            r = json.loads(line)
            st["rounds"] += 1
            st["stall_rounds"] += bool(r["picks"])
            st["picks"] += len(r["picks"])
            st["respill_rounds"] += r["round"] > 0
            st["cost_rounds"] += bool(r["costs"])
            st["post_rounds"] += "post" in r
            err = colsim.check(r)
            bad = [c for c in CATS if err[c]]
            for c in bad:
                st[c + "_bad"] += 1
            if bad:
                log.write("== %s %s (fn %d) round %d\n" % (tu, r["func"], r["fn"], r["round"]))
                for c in bad:
                    for e in err[c][:10]:
                        log.write("   %s: %s\n" % (c, e))
        nbad = sum(v for k, v in st.items() if k.endswith("_bad"))
        if nbad:
            failed.append(name)
        tot += st
        print("%-40s fns %3d rounds %3d stalls %3d  %s" % (name, nf, st["rounds"], st["stall_rounds"],
                                                         "ok" if not nbad else "MISMATCH " + " ".join("%s=%d" % (k, v) for k, v in st.items() if k.endswith("_bad"))), flush=True)
    log.close()
    print("TOTAL TUs %d checked, %d skipped (no scratch compile: %s)" % (tot["tus"], len(skipped), " ".join(skipped) or "-"))
    print("      functions with rounds %d, rounds %d (stalled %d, picks %d, re-runs %d), cost rounds %d, post-rewrite rounds %d" % (
        tot["funcs"], tot["rounds"], tot["stall_rounds"], tot["picks"], tot["respill_rounds"], tot["cost_rounds"], tot["post_rounds"]))
    print("      mismatching rounds: " + "  ".join("%s %d" % (c, tot[c + "_bad"]) for c in CATS) + "  | objects differing %d" % tot["object_bad"])
    if failed:
        print("FAIL: %s  (details in %s)" % (" ".join(failed), a.log))
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()
