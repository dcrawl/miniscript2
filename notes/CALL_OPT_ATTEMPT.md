# Call/Return Optimization

## Motivation

Benchmarking (March 2026) revealed that MiniScript 2's call/return overhead is significantly higher than both Python and Lua. On the recursive Fibonacci benchmark (fib(33), ~11.4M calls), the C++ computed-goto build takes 1.49s vs Python at 1.38s and Lua at 0.35s. Meanwhile on iterative benchmarks (no function calls), MiniScript 2 beats both by 2x or more.

This tells us the core dispatch loop is competitive, but the function call mechanism is the bottleneck.

### Benchmark Data (C++ computed-goto, .msa hand-written assembly)

| Benchmark            | MS2 goto | Python | Lua   |
|----------------------|----------|--------|-------|
| Iterative Factorial  | 0.30s    | 2.25s  | 0.61s |
| Iterative Fibonacci  | 0.31s    | 1.95s  | 0.70s |
| Recursive Fibonacci  | 1.49s    | 1.38s  | 0.35s |

## Current Call/Return Overhead

### CallInfo struct (6 fields)

```csharp
public struct CallInfo {
    public Int32 ReturnPC;
    public Int32 ReturnBase;
    public Int32 ReturnFuncIndex;
    public Int32 CopyResultToReg;   // only used by CALL_rA_rB_rC, not CALLF
    public Value LocalVarMap;       // only used when `locals` is referenced
    public Value OuterVarMap;       // only used for closures
}
```

### CALLF_iA_iBC (hot path for direct calls)

1. Decode instruction
2. Bounds-check funcIndex
3. `functions[funcIndex]` lookup to get FuncDef
4. Call stack overflow check
5. Construct `new CallInfo(...)` -- initializes all 6 fields
6. `ApplyPendingContext()` -- function call + branch (no-op for non-method calls)
7. Reassign 5 state variables (pc, curFunc, codeCount, curCode, curConstants)
8. `EnsureFrame()` -- function call + bounds check

### RETURN (hot path)

1. Read result from `stack[baseIndex]`
2. Branch: check if call stack empty
3. Read current frame, branch: check LocalVarMap
4. Pop call stack, read caller's CallInfo
5. Restore 5 state variables
6. `functions[returnFuncIndex]` lookup to restore curFunc
7. Branch: check CopyResultToReg

### Summary

Each call+return cycle involves ~6 branches, 2 function calls to helpers (ApplyPendingContext, EnsureFrame), construction of a 6-field struct, and two `functions[]` lookups. Most of this work is for features (closures, `locals`, self/super, CopyResultToReg) that are unused in the common case.

## Proposed Optimization: Stack-Interleaved Call Frames

### Concept

Instead of a separate `callStack` array, store call frame data directly on the main value stack, interleaved with each frame's registers. This is the approach Lua uses to achieve very low call overhead.

### Frame Layout on the Value Stack

```
[...caller registers...]
[frame header: PC (int Value), base (int Value), funcIndex (int Value)]
[r0: return value] [r1: param1] [r2: param2] ... [rN]
[frame header: PC, base, funcIndex]
[r0] [r1] ...
```

Each frame header is 3 Value slots (24 bytes) stored at known offsets below the frame's r0. The base pointer in each header points back to the caller's base, forming an implicit linked list.

### Why 3 fields instead of 6

- **CopyResultToReg**: Eliminated. RETURN leaves the result in `stack[calleeBase]` (callee's r0). The caller already knows where the callee's base is (it set it up), so a post-call MOV instruction copies the result. This moves work from the return path (every return) to the call site (only when needed).

- **LocalVarMap**: Moved to a side table, allocated lazily only when `locals` is actually referenced. The vast majority of calls never touch this.

- **OuterVarMap**: Moved to the same side table, allocated only for closure calls.

### What the hot path becomes

**CALL:**
1. Write 3 Values to stack (PC, base, funcIndex)
2. Update baseIndex
3. Set pc = 0, update curFunc/curCode/curConstants

**RETURN:**
1. Read result from stack[baseIndex] (r0)
2. Read 3 Values from frame header (PC, base, funcIndex)
3. Restore baseIndex, pc
4. `functions[funcIndex]` lookup to restore curFunc

### Additional simplifications

- **ApplyPendingContext**: Skip entirely for CALLF. Only needed for CALL_rA_rB_rC (method dispatch via dot-call).

- **EnsureFrame**: Make debug-only. Pre-allocate the stack to a generous fixed size. In release builds, stack overflow crashes like native code.

- **funcIndex as integer Value**: PC, base, and funcIndex all fit as integer-tagged Values (using the existing `INTEGER_TAG` NaN-box encoding). No new Value types needed.

### Cache locality benefit

With call frames on the same stack as registers, the frame header is adjacent in memory to the registers the function is actively using. This is better for cache performance than the current design where `callStack[]` and `stack[]` are separate allocations that may be in different cache lines.

### Trade-offs and considerations

- **Stack layout complexity**: Register offsets from base must account for the frame header. If the header is stored *below* r0 (at negative offsets from base), existing register addressing (base + regIndex) is unchanged.

- **Mixed stack content**: Some stack slots are frame headers, not MiniScript values. This is fine internally but means stack inspection (e.g., for debugging) needs to understand the layout.

- **Side table for rare features**: LocalVarMap and OuterVarMap need a separate structure (e.g., a dictionary keyed by base pointer or call depth). This adds a lookup on the rare path but removes two Value-sized fields from every call frame on the common path.

## Implementation Plan

1. Remove CopyResultToReg from CallInfo; have the compiler emit a post-call copy instruction instead.
2. Move LocalVarMap and OuterVarMap to a side table.
3. At this point CallInfo is 3 Int32 fields -- validate that performance improves.
4. Interleave the 3-field frame header onto the value stack as integer Values.
5. Remove the separate callStack array.
6. Make EnsureFrame debug-only; skip ApplyPendingContext for CALLF.
7. Benchmark and compare.


## Results

We tried the above, and it did not help significantly.  In fact in the cpp-goto src benchmark, the time was actually slightly worse.  So, we've reverted to the previous CallInfo stack.
