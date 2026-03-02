using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using static System.Runtime.CompilerServices.MethodImplOptions;
// H: #include "VM.g.h"

namespace MiniScript {

// Context passed to native (intrinsic) callback functions.
public struct CallContext {
	public VM vm;
	public List<Value> stack;
	public Int32 baseIndex;		// index of return register; arguments follow this
	public Int32 argCount;		// how many arguments we have
	
	public CallContext(VM vm, List<Value> stack, Int32 baseIndex, Int32 argCount) {
		this.vm = vm;
		this.stack = stack;
		this.baseIndex = baseIndex;
		this.argCount = argCount;
	}
	
	// Get an argument from the stack, by number (the first argument is index 0,
	// the second is index 1, etc.).
	[MethodImpl(AggressiveInlining)]
	public Value GetArg(int zeroBasedIndex) {
		// The base index is the position of the return register; the arguments
		// start right after that.  Note that we don't do any range checking here;
		// be careful not to ask for arguments beyond the declared parameters.
		return stack[baseIndex + 1 + zeroBasedIndex];
	}
}

}
