# Performance Gate V1

This note defines the practical performance/soak gate process for launch-branch validation.

## Gate Command

- Full gate: `tools/build.sh perf-gate`
- Quick gate: `tools/build.sh perf-gate --quick`

The gate performs:

1. Benchmark stage for configured runtime variants (`cs`, `cpp-switch`, `cpp-goto`) using source benchmark scripts in `tools/benchmarks/*.ms`.
2. Soak stage in all runtime variants:
   - C#
   - C++ switch dispatch
   - C++ computed-goto dispatch
3. Report generation under `build/reports/`.

## Default Scope

- Benchmark languages: `cs,cpp-switch,cpp-goto`
- Soak iterations per runtime: `1000`

## Quick Scope

- Benchmark languages: `cpp-goto`
- Soak iterations per runtime: `100`

## Output Artifacts

- Markdown report: `build/reports/perf_gate_<timestamp>.md`
- Raw benchmark log: `build/reports/perf_gate_<timestamp>_benchmark.log`

## Pass/Fail Rules (V1)

- Benchmark stage must complete successfully (including expected-result checks for each benchmark script).
- All soak stages must complete successfully without:
  - non-completion,
  - runtime errors,
  - result drift.

## Optional Tightening (Post-Baseline)

When target hardware baselines are agreed, enforce explicit numeric thresholds in gate policy (for example p95 time limits and max acceptable soak duration).
