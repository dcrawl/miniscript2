// AUTO-GENERATED FILE.  DO NOT MODIFY.
// Transpiled from: Intrinsic.cs

#include "Intrinsic.g.h"
#include "CoreIntrinsics.g.h"

namespace MiniScript {

	List<Intrinsic> IntrinsicStorage::_all;
	Boolean IntrinsicStorage::_initialized;
Int32 IntrinsicStorage::Count() {
	if (!_initialized) {
		CoreIntrinsics::Init();
		_initialized = Boolean(true);
	}
	return _all.Count();
}
Intrinsic IntrinsicStorage::Create(String name) {
	Intrinsic result =  Intrinsic::New();
	result.set_Name(name);
	result.set__paramNames( List<String>::New());
	result.set__paramDefaults( List<Value>::New());
	_all.Add(result);
	return result;
}
void IntrinsicStorage::AddParam(String name) {
	_paramNames.Add(name);
	_paramDefaults.Add(val_null);
}
void IntrinsicStorage::AddParam(String name,Value defaultValue) {
	_paramNames.Add(name);
	_paramDefaults.Add(defaultValue);
}
Intrinsic IntrinsicStorage::GetByName(String name) {
	if (!_initialized) {
		CoreIntrinsics::Init();
		_initialized = Boolean(true);
	}
	for (Int32 i = 0; i < _all.Count(); i++) {
		if (_all[i].Name() == name) return _all[i];
	}
	return nullptr;
}
List<String> IntrinsicStorage::AllNames() {
	if (!_initialized) {
		CoreIntrinsics::Init();
		_initialized = Boolean(true);
	}
	List<String> result =  List<String>::New();
	for (Int32 i = 0; i < _all.Count(); i++) {
		result.Add(_all[i].Name());
	}
	return result;
}
Value IntrinsicStorage::GetFunc() {
	return make_funcref(_funcIndex, val_null);
}
FuncDef IntrinsicStorage::BuildFuncDef() {
	FuncDef def =  FuncDef::New();
	def.set_Name(Name);
	for (Int32 i = 0; i < _paramNames.Count(); i++) {
		def.ParamNames().Add(make_string(_paramNames[i]));
		def.ParamDefaults().Add(_paramDefaults[i]);
	}
	def.set_MaxRegs((UInt16)(_paramNames.Count() + 1)); // r0 + params
	def.set_NativeCallback(Code);
	return def;
}
void IntrinsicStorage::RegisterAll(List<FuncDef> functions,Dictionary<String, Value> intrinsics) {
	if (!_initialized) {
		CoreIntrinsics::Init();
		_initialized = Boolean(true);
	}
	intrinsics.Clear();
	for (Int32 i = 0; i < _all.Count(); i++) {
		Intrinsic intr = _all[i];
		FuncDef def = intr.BuildFuncDef();
		Int32 funcIndex = functions.Count();
		intr.set__funcIndex(funcIndex);
		functions.Add(def);
		intrinsics[intr.Name()] = make_funcref(funcIndex, val_null);
	}
	// Invalidate type maps so they get rebuilt with current function indices
	CoreIntrinsics::InvalidateTypeMaps();
}

} // end of namespace MiniScript
