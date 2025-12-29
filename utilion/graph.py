import valloc
import vboost

if __name__ != "__main__":
    raise ImportError("Must be __main__")

BENCHMARKS = (
    valloc.create_visualizer(),
    vboost.create_visualizer(),
)

for it in BENCHMARKS:
    it.save_plots()