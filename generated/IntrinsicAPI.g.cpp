// AUTO-GENERATED FILE.  DO NOT MODIFY.
// Transpiled from: IntrinsicAPI.cs

#include "IntrinsicAPI.g.h"

namespace MiniScript {

IntrinsicResult::IntrinsicResult(Value result,Boolean done ) {
	this->result = result;
	this->done = done;
}
const IntrinsicResult IntrinsicResult::Null = IntrinsicResult(val_null);
const IntrinsicResult IntrinsicResult::EmptyString = IntrinsicResult(val_empty_string);
const IntrinsicResult IntrinsicResult::Zero = IntrinsicResult(val_zero);
const IntrinsicResult IntrinsicResult::One = IntrinsicResult(val_one);

Context::Context(VM vm,List<Value> stack,Int32 baseIndex,Int32 argCount) {
	this->vm = vm;
	this->stack = stack;
	this->baseIndex = baseIndex;
	this->argCount = argCount;
}
Value Context::GetVar(String variableName) {
	return vm.LookupParamByName(variableName);
}

} // end of namespace MiniScript
