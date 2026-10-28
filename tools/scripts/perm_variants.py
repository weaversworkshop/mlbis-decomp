import itertools
import random
from string import Template


def perms(items, sep="\n", limit=None):
    out = {}
    for p in itertools.permutations(items):
        out["".join(k[0] for k in p)] = sep.join(t for _, t in p)
        if limit and len(out) >= limit:
            break
    return out


def pick(items, sep="\n"):
    return {"".join(k[0] for k in c): sep.join(t for _, t in c) for c in items}


class Grid:
    def __init__(self, template):
        self.t = Template(template)
        self.axes = []

    def axis(self, name, options):
        self.axes.append((name, options if isinstance(options, dict) else dict(options)))
        return self

    def build(self, sample=None, seed=0):
        names = [n for n, _ in self.axes]
        combos = list(itertools.product(*[list(o.items()) for _, o in self.axes]))
        if sample and len(combos) > sample:
            combos = random.Random(seed).sample(combos, sample)
        V = {}
        for c in combos:
            key = "_".join(f"{n}{k}" for n, (k, _) in zip(names, c))
            V[key] = self.t.safe_substitute({n: v for n, (_, v) in zip(names, c)})
        return V
