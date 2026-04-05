# **Engine-Ready Checklist (First Shipped Integration)**

## **Must-Have (Ship Blockers)**

1. Single stable host API boundary

- Define one embedding facade in C++ around interpreter lifecycle: create, load script, run step budget, read errors, stop/reset.
- Mirror the capabilities already exposed in Interpreter.cs, especially reset/compile/run in Interpreter.cs and Interpreter.cs.
- Acceptance: engine-side code never reaches directly into VM internals except through this facade.

1. Deterministic frame-budget execution

- Drive scripts with bounded stepping/time budgets each frame.
- Use existing yielding model (wait/yield/time intrinsics in CoreIntrinsics.cs, CoreIntrinsics.cs, CoreIntrinsics.cs, and VM yield flag in VM.cs).
- Acceptance: long scripts cannot stall the game loop.

1. Error routing and crash containment

- Route compile/runtime errors to engine logging and gameplay-safe fallback.
- Use interpreter error callback path in Interpreter.cs.
- Acceptance: script faults fail the script instance, not the process.

1. Complete host data exchange for gameplay variables

- Implement/settle write-path for globals before ship, since setter is currently TODO in Interpreter.cs.
- Read path exists in Interpreter.cs; write path must match expected gameplay workflows.
- Acceptance: reliable get/set of exposed game variables from host.

1. Intrinsic surface freeze for v1

- Lock down which host functions are exposed via intrinsic registration model in Intrinsic.cs.
- Keep minimal and auditable for first release (print/logging, time, scheduling, core data ops).
- Acceptance: no ad hoc intrinsic additions during content lock.

1. C#→C++ pipeline as a hard CI gate

- Enforce build chain on every integration branch:
- build.sh cs
- build.sh transpile
- build.sh cpp on
- build.sh cpp off
- Script supports these stages in build.sh, and README documents core flow in README.md.
- Acceptance: no merge unless both C# and transpiled C++ pass.

1. Integration test pack for engine-critical behavior

- Extend the existing integration suite format in testSuite.txt and run it in C++ build gates.
- Minimum coverage: control flow, functions, closures, map/list/string ops, host callbacks, wait/yield behavior, error cases.
- Acceptance: stable pass rate in CI and on target platform.

1. Performance budget and memory soak on target game content

- Benchmark realistic scripts in release mode with your engine tick pattern.
- Require no growth/leak trend over long soak runs.
- Acceptance: frame time and memory are within agreed budget.

1. C# as only editable source for runtime logic

- Keep generated files as artifacts only; edit source in cs and regenerate.
- This matches project guidance in CLAUDE.md.
- Acceptance: no direct generated C++ hand-edits in release branch.

1. Versioned script compatibility contract

- Freeze language/intrinsic behavior for launch branch; document what content authors can rely on.
- Acceptance: no breaking script semantics after content freeze.

## **Nice-to-Have (Post-Ship or Pre-Ship if Time Allows)**

1. Multi-VM safety hardening

- You can defer this for your one-VM engine, but track the known issue in POTENTIAL_ISSUES.md for future scalability.

1. Unicode lexer hardening

- Known TODO in Lexer.cs; defer if your shipped script set is ASCII-focused.

1. Better script tooling for content creators

- Source mapping, richer diagnostics, script profiler hooks, hot-reload UX.

1. Sandbox policy controls

- Intrinsic allowlists per game mode, runtime quotas per script/class of script.

1. Replay determinism mode

- Optional deterministic random/time hooks for debugging and multiplayer replay workflows.

1. Packaging as reusable library target

- Current C# project is exe-oriented in MiniScript2.csproj; if needed, add a library packaging path for cleaner embedding ergonomics.

## **Recommended Ship Gate Order**

1. API boundary + error routing + variable read/write
2. Frame-budget scheduler integration (yield/wait)
3. CI gates for cs/transpile/cpp on+off
4. Engine-focused integration tests
5. Performance and soak validation
6. Content freeze with intrinsic/language contract
