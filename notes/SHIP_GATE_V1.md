# Ship Gate V1

`tools/build.sh ship-gate` is the combined release-style validation command.

## What It Enforces

1. Required contract docs exist:
   - notes/SCRIPT_COMPATIBILITY_V1.md
   - notes/INTRINSIC_SURFACE_V1.md
   - notes/PERFORMANCE_GATE_V1.md
2. `ci-gate` passes:
   - C# build + debug tests
   - transpile
   - C++ build/test with computed-goto ON
   - C++ build/test with computed-goto OFF
3. `perf-gate` passes:
   - benchmark stage
   - soak stage
   - report artifacts generated

## Modes

- Full: `tools/build.sh ship-gate`
- Quick: `tools/build.sh ship-gate --quick`

## Output

- Ship gate report: `build/reports/ship_gate_<timestamp>.md`
- Perf gate report/logs: `build/reports/perf_gate_<timestamp>.md` and companion benchmark log
