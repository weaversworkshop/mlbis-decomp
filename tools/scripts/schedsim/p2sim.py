import copy
import json
import sys

import pcsim

if len(sys.argv) < 3:
    raise SystemExit("usage: p2sim.py <dump.jsonl> <pass-1 block idx> [vreg=physreg ...]")
rows = [json.loads(l) for l in open(sys.argv[1])]
rec = rows[int(sys.argv[2])]
ren = {int(k): int(v) for k, v in (kv.split("=") for kv in sys.argv[3:])}
r2 = copy.deepcopy(rec)
r2["nodes"] = [copy.deepcopy(rec["nodes"][i]) for i in rec["out"]]
for nd in r2["nodes"]:
    for op in nd["instr"]["ops"]:
        if op[0] == 0 and op[2] in ren:
            op[2] = ren[op[2]]
res = pcsim.schedule(r2, pcsim.model_graph(r2))
order = res[0] if isinstance(res, tuple) else res
print("pass-1 output order:", rec["out"])
print("pass-2 order       :", order)
for k in order:
    print("  ", pcsim.fmt(r2["nodes"][k]["instr"]))
