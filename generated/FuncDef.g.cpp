// AUTO-GENERATED FILE.  DO NOT MODIFY.
// Transpiled from: FuncDef.cs

#include "FuncDef.g.h"
#include "StringUtils.g.h"
#include "IntrinsicAPI.g.h"
#include "gc.h"

namespace MiniScript {

const Int32 FuncDefStorage::JitStubStateNone = 0;
const Int32 FuncDefStorage::JitStubStateCandidate = 1;
const Int32 FuncDefStorage::JitStubStateCompiled = 2;
const Int32 FuncDefStorage::JitStubStateFailed = 3;
const Int32 FuncDefStorage::JitStubBackendNone = 0;
const Int32 FuncDefStorage::JitStubBackendReturnNull = 1;
const Int32 FuncDefStorage::JitStubBackendReturnInt = 2;
const Int32 FuncDefStorage::JitStubBackendReturnConst = 3;
FuncDefStorage::FuncDefStorage() {
}
void FuncDefStorage::ReserveRegister(Int32 registerNumber) {
	UInt16 impliedCount = (UInt16)(registerNumber + 1);
	if (MaxRegs < impliedCount) MaxRegs = impliedCount;
}
String FuncDefStorage::ToString() {
	String result = Name + "(";
	GC_PUSH_SCOPE();
	Value defaultVal; GC_PROTECT(&defaultVal);
	for (Int32 i = 0; i < ParamNames.Count(); i++) {
		if (i > 0) result += ", ";
		result += as_cstring(ParamNames[i]);
		defaultVal = ParamDefaults[i];
		if (!is_null(defaultVal)) {
			result += "=";
			result += as_cstring(value_repr(defaultVal));
		}
	}
	result += ")";
	GC_POP_SCOPE();
	return result;
}

} // end of namespace MiniScript
