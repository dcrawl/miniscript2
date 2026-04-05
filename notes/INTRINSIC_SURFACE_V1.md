# Intrinsic Surface V1

This document defines the approved intrinsic surface for the first shipped engine integration.

Any change to this set should be treated as a compatibility-impacting change and reviewed explicitly.

## Approved Intrinsics

- `abs`
- `acos`
- `asin`
- `atan`
- `ceil`
- `char`
- `code`
- `cos`
- `floor`
- `freeze`
- `frozenCopy`
- `funcRef`
- `hasIndex`
- `indexOf`
- `indexes`
- `print`
- `input`
- `insert`
- `isFrozen`
- `join`
- `len`
- `list`
- `log`
- `lower`
- `map`
- `number`
- `pi`
- `pop`
- `pull`
- `push`
- `range`
- `remove`
- `replace`
- `rnd`
- `round`
- `shuffle`
- `sign`
- `sin`
- `slice`
- `sort`
- `split`
- `sqrt`
- `str`
- `string`
- `sum`
- `tan`
- `time`
- `wait`
- `yield`
- `upper`
- `val`
- `values`

## Enforcement

`UnitTests.TestIntrinsicAllowlistV1` enforces an exact-match policy:

- Fails if any approved intrinsic is missing.
- Fails if any unapproved intrinsic is present.
- Fails if total intrinsic count differs from this approved set.

When this file is updated, update `TestIntrinsicAllowlistV1` accordingly in `cs/UnitTests.cs`.
