# MiniScript 2.0

MiniScript 2.0 is complete rewrite of the [MiniScript](https://miniscript.org) language, focused mainly on performance.  It is the cornerstone of the [2026 MiniScript Road Map](https://dev.to/joestrout/miniscript-road-map-for-2026-17mh).  For MiniScript 1.x source code, see [here](https://github.com/JoeStrout/miniscript).

MiniScript 2.0 is developed in C# and transpiled to C++ for production performance. The VM uses 32-bit fixed-width instructions and supports computed-goto dispatch on supported compilers.

## Building

```bash
tools/build.sh cs         # Build C# only
tools/build.sh transpile  # Generate C++ code from C# code
tools/build.sh cpp        # Build C++ only
tools/build.sh all        # Build everything (C#, transpile, C++)
tools/build.sh test       # Run smoke tests
tools/build.sh ci-gate    # Full C# + transpile + C++(goto/switch) debug gate
tools/build.sh soak       # Soak validation wrapper (see options below)
tools/build.sh perf-gate  # Benchmark + soak report gate
tools/build.sh ship-gate  # Combined release-style validation gate
```

The `ci-gate` target is the recommended pre-merge validation path.

## Soak Validation

Use soak mode to run repeated in-process restart/run cycles against one script:

```bash
# Run script 500 times (default)
./build/cs/miniscript2 -soak examples/recur_fib.ms

# Run script N times
./build/cs/miniscript2 -soak=2000 examples/recur_fib.ms
./build/cpp/miniscript2 -soak=2000 examples/recur_fib.msa
```

Soak mode fails if a run does not complete, if runtime errors occur, or if the final `r0`
result drifts across iterations.

For a repeatable multi-runtime soak workflow, use:

```bash
tools/build.sh soak -n 2000            # run C# + C++ (switch) soak
tools/build.sh soak -n 2000 -lang cs   # C# only
tools/build.sh soak -n 2000 -lang cpp  # C++ only
```

For a combined performance and long-run stability gate with report output:

```bash
tools/build.sh perf-gate           # full benchmark + soak gate
tools/build.sh perf-gate --quick   # faster smoke variant
tools/build.sh perf-gate --max-gate-s 3600 --max-recurfib-s 45
tools/build.sh ship-gate --quick   # ci-gate + perf-gate + contract checks
```

## Notes

- [CS_CODING_STANDARDS.md](notes/CS_CODING_STANDARDS.md) — C# coding restrictions required by the C#-to-C++ transpiler.
- [DEV_LOG.md](notes/DEV_LOG.md) — Development log documenting project status and decisions.
- [FROZEN_VALUES.md](notes/FROZEN_VALUES.md) — Design for immutable maps and lists via freeze/isFrozen/frozenCopy.
- [FUNCTION_CALLS.md](notes/FUNCTION_CALLS.md) — Design and considerations for the VM's function call mechanism.
- [GC_USAGE.md](notes/GC_USAGE.md) — Guide to the garbage collection shadow-stack system.
- [LANGUAGE_CHANGES.md](notes/LANGUAGE_CHANGES.md) — Observable language changes in MiniScript 2.0 vs 1.x.
- [MEMORY_SYSTEMS.md](notes/MEMORY_SYSTEMS.md) — Overview of the three memory systems (GC, intern table, etc.).
- [OPCODE_ADDITION.md](notes/OPCODE_ADDITION.md) — Procedure for adding new opcodes to the VM.
- [SCRIPT_COMPATIBILITY_V1.md](notes/SCRIPT_COMPATIBILITY_V1.md) — Launch-branch compatibility guarantees and change policy for shipped scripts.
- [SHIP_GATE_V1.md](notes/SHIP_GATE_V1.md) — Combined release-style gate (`ship-gate`) covering contracts, CI, and perf/soak checks.
- [UNARY_MINUS_QUIRK.md](notes/UNARY_MINUS_QUIRK.md) — Language quirk involving unary minus and call statement syntax.
- [VARIABLES.md](notes/VARIABLES.md) — How variables map to registers and interact with scope maps.
- [VM_DESIGN.md](notes/VM_DESIGN.md) — Architecture of the register-based VM and instruction encoding.
- [miniscript.g4](notes/miniscript.g4) — ANTLR grammar defining MiniScript syntax.


## Sponsor Me!

MiniScript is free, and apart from a small amount of revenue from the [books](https://miniscript.org/books/) and [Unity asset](https://assetstore.unity.com/packages/tools/integration/miniscript-87926), generates no significant income.  Your support is greatly appreciated, and will be used to fund community growth & reward programs like [these](https://miniscript.org/earn.html).

So, [click here to sponsor](https://github.com/sponsors/JoeStrout) -- contributions of any size are greatly appreciated!

## Star History

[![Star History Chart](https://api.star-history.com/svg?repos=JoeStrout/miniscript2&type=date&legend=top-left)](https://www.star-history.com/#JoeStrout/miniscript2&type=date&legend=top-left)

Click the ⭐️ at the top to help push this graph up!  Every like helps more people discover MiniScript, and keeps me motivated to push it forward every day!

