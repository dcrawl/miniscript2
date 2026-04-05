# Script Compatibility Contract V1

This document defines the script-level compatibility promises for the first shipped engine integration branch of MiniScript 2.0.

Scope: this contract applies to game content scripts and host integrations that use the public interpreter/facade API. It is intended to protect shipped content from breaking behavior changes during the v1 lifecycle.

## Versioning

- Contract version: v1
- Language baseline: MiniScript 2.0 behavior in this repository as validated by the current integration and unit test suites.
- Intrinsic baseline: exact intrinsic set defined in notes/INTRINSIC_SURFACE_V1.md.

## Compatibility Guarantees (Frozen for V1)

1. Language semantics used by existing content and covered by tests remain stable.
2. Intrinsic names and core behavior remain stable according to notes/INTRINSIC_SURFACE_V1.md.
3. Map key iteration order remains insertion order.
4. Power operator (`^`) remains right-associative.
5. Frozen list/map behavior remains stable (`freeze`, `isFrozen`, `frozenCopy`) including mutation-error behavior.
6. Host observable runtime error behavior remains script-instance scoped (script may fail, process should not crash under normal API usage).
7. Yield/time scheduling semantics remain cooperative and frame-budget friendly under host-driven execution.
8. C# source remains the single editable runtime source of truth; transpiled C++ must preserve observable behavior for covered cases.

## Non-Guaranteed Areas in V1

1. Multi-VM safety hardening beyond the single-VM embedding assumption.
2. Full Unicode lexer hardening beyond currently tested behavior.
3. New intrinsic additions without explicit versioned review.
4. REPL/tooling UX details that do not affect shipped game content execution.

## Change Policy

- No breaking script behavior changes are allowed after content freeze on the launch branch.
- Any behavior change that can affect script outcomes requires one of:
  - a new contract version (for example v2), or
  - an explicitly documented migration path accepted before release.
- Intrinsic surface changes require updates to both:
  - notes/INTRINSIC_SURFACE_V1.md
  - cs/UnitTests.cs (`TestIntrinsicAllowlistV1`)

## Required Validation Before Merge (Launch Branch)

1. Full build/test gate passes:
   - tools/build.sh ci-gate
2. Soak validation passes for representative scripts:
   - tools/build.sh soak -n <agreed_count>
3. No direct manual edits to generated C++ files are introduced as source changes.

## Contract References

- notes/INTRINSIC_SURFACE_V1.md
- notes/LANGUAGE_CHANGES.md
- Checklist.md
- tools/build.sh
- tools/soak.sh
