// AUTO-GENERATED FILE.  DO NOT MODIFY.
// Transpiled from: FuncDef.cs

#pragma once
#include "core_includes.h"
#include "forward_decs.g.h"
#include "value.h"

namespace MiniScript {
struct Context;  // forward declaration; defined in VM.g.h
struct IntrinsicResult;  // forward declaration
typedef IntrinsicResult (*NativeCallbackDelegate)(Context, IntrinsicResult);
inline bool IsNull(NativeCallbackDelegate f) { return f == nullptr; }

// DECLARATIONS

class FuncDefStorage : public std::enable_shared_from_this<FuncDefStorage> {
	friend struct FuncDef;
	public: String Name = "";
	public: List<UInt32> Code = List<UInt32>::New();
	public: List<Value> Constants = List<Value>::New();
	public: UInt16 MaxRegs = 0; // how many registers to reserve for this function
	public: List<Value> ParamNames = List<Value>::New(); // parameter names (as Value strings)
	public: List<Value> ParamDefaults = List<Value>::New(); // default values for parameters
	public: Int16 SelfReg = -1; // register for 'self' (-1 if not used)
	public: Int16 SuperReg = -1; // register for 'super' (-1 if not used)
	public: bool JitIsHotCandidate = false;
	public: UInt64 JitObservedInstructions = 0;
	public: static const Int32 JitStubStateNone;
	public: static const Int32 JitStubStateCandidate;
	public: static const Int32 JitStubStateCompiled;
	public: static const Int32 JitStubStateFailed;
	public: static const Int32 JitStubBackendNone;
	public: static const Int32 JitStubBackendReturnNull;
	public: static const Int32 JitStubBackendReturnInt;
	public: static const Int32 JitStubBackendReturnConst;
	public: Int32 JitStubState = 0;
	public: Int32 JitStubBackendKind = 0;
	public: Int32 JitStubBackendIntValue = 0;
	public: bool JitResetPrecompiled = false;
	public: Int32 JitStubCompileAttempts = 0;
	public: String JitStubLastError = "";
	public: NativeCallbackDelegate NativeCallback = null;

	// Tier-2 JIT groundwork: runtime hot-function candidate metadata.

	// Tier-2 stub lifecycle groundwork.

	// Native callback for intrinsic functions. When non-null, this FuncDef
	// represents a built-in function: CALL invokes the callback directly
	// instead of executing bytecode.  Parameters are in stack[baseIndex+1..].

	public: FuncDefStorage();

	public: void ReserveRegister(Int32 registerNumber);

	// Returns a string like "functionName(a, b=1, c=0)"
	public: String ToString();

	// Conversion to bool: returns true if function has a name
	public: operator bool() const;
	
}; // end of class FuncDefStorage

// Native callback for intrinsic functions.

// Function definition: code, constants, and how many registers it needs
struct FuncDef {
	friend class FuncDefStorage;
	protected: std::shared_ptr<FuncDefStorage> storage;
  public:
	FuncDef(std::shared_ptr<FuncDefStorage> stor) : storage(stor) {}
	FuncDef() : storage(nullptr) {}
	FuncDef(std::nullptr_t) : storage(nullptr) {}
	friend bool IsNull(const FuncDef& inst) { return inst.storage == nullptr; }
	private: FuncDefStorage* get() const;

	public: String Name();
	public: void set_Name(String _v);
	public: List<UInt32> Code();
	public: void set_Code(List<UInt32> _v);
	public: List<Value> Constants();
	public: void set_Constants(List<Value> _v);
	public: UInt16 MaxRegs(); // how many registers to reserve for this function
	public: void set_MaxRegs(UInt16 _v); // how many registers to reserve for this function
	public: List<Value> ParamNames(); // parameter names (as Value strings)
	public: void set_ParamNames(List<Value> _v); // parameter names (as Value strings)
	public: List<Value> ParamDefaults(); // default values for parameters
	public: void set_ParamDefaults(List<Value> _v); // default values for parameters
	public: Int16 SelfReg(); // register for 'self' (-1 if not used)
	public: void set_SelfReg(Int16 _v); // register for 'self' (-1 if not used)
	public: Int16 SuperReg(); // register for 'super' (-1 if not used)
	public: void set_SuperReg(Int16 _v); // register for 'super' (-1 if not used)
	public: bool JitIsHotCandidate();
	public: void set_JitIsHotCandidate(bool _v);
	public: UInt64 JitObservedInstructions();
	public: void set_JitObservedInstructions(UInt64 _v);
	public: Int32 JitStubStateNone();
	public: Int32 JitStubStateCandidate();
	public: Int32 JitStubStateCompiled();
	public: Int32 JitStubStateFailed();
	public: Int32 JitStubBackendNone();
	public: Int32 JitStubBackendReturnNull();
	public: Int32 JitStubBackendReturnInt();
	public: Int32 JitStubBackendReturnConst();
	public: Int32 JitStubState();
	public: void set_JitStubState(Int32 _v);
	public: Int32 JitStubBackendKind();
	public: void set_JitStubBackendKind(Int32 _v);
	public: Int32 JitStubBackendIntValue();
	public: void set_JitStubBackendIntValue(Int32 _v);
	public: bool JitResetPrecompiled();
	public: void set_JitResetPrecompiled(bool _v);
	public: Int32 JitStubCompileAttempts();
	public: void set_JitStubCompileAttempts(Int32 _v);
	public: String JitStubLastError();
	public: void set_JitStubLastError(String _v);
	public: NativeCallbackDelegate NativeCallback();
	public: void set_NativeCallback(NativeCallbackDelegate _v);

	// Tier-2 JIT groundwork: runtime hot-function candidate metadata.

	// Tier-2 stub lifecycle groundwork.

	// Native callback for intrinsic functions. When non-null, this FuncDef
	// represents a built-in function: CALL invokes the callback directly
	// instead of executing bytecode.  Parameters are in stack[baseIndex+1..].

	public: static FuncDef New() {
		return FuncDef(std::make_shared<FuncDefStorage>());
	}

	public: inline void ReserveRegister(Int32 registerNumber);

	// Returns a string like "functionName(a, b=1, c=0)"
	public: String ToString() { return get()->ToString(); }

	// Conversion to bool: returns true if function has a name
	public: operator bool() const { return (bool)(*get()); }
	public: FuncDefStorage* get_storage() const { return storage.get(); }
}; // end of struct FuncDef

// INLINE METHODS

inline FuncDefStorage* FuncDef::get() const { return static_cast<FuncDefStorage*>(storage.get()); }
inline String FuncDef::Name() { return get()->Name; }
inline void FuncDef::set_Name(String _v) { get()->Name = _v; }
inline List<UInt32> FuncDef::Code() { return get()->Code; }
inline void FuncDef::set_Code(List<UInt32> _v) { get()->Code = _v; }
inline List<Value> FuncDef::Constants() { return get()->Constants; }
inline void FuncDef::set_Constants(List<Value> _v) { get()->Constants = _v; }
inline UInt16 FuncDef::MaxRegs() { return get()->MaxRegs; } // how many registers to reserve for this function
inline void FuncDef::set_MaxRegs(UInt16 _v) { get()->MaxRegs = _v; } // how many registers to reserve for this function
inline List<Value> FuncDef::ParamNames() { return get()->ParamNames; } // parameter names (as Value strings)
inline void FuncDef::set_ParamNames(List<Value> _v) { get()->ParamNames = _v; } // parameter names (as Value strings)
inline List<Value> FuncDef::ParamDefaults() { return get()->ParamDefaults; } // default values for parameters
inline void FuncDef::set_ParamDefaults(List<Value> _v) { get()->ParamDefaults = _v; } // default values for parameters
inline Int16 FuncDef::SelfReg() { return get()->SelfReg; } // register for 'self' (-1 if not used)
inline void FuncDef::set_SelfReg(Int16 _v) { get()->SelfReg = _v; } // register for 'self' (-1 if not used)
inline Int16 FuncDef::SuperReg() { return get()->SuperReg; } // register for 'super' (-1 if not used)
inline void FuncDef::set_SuperReg(Int16 _v) { get()->SuperReg = _v; } // register for 'super' (-1 if not used)
inline bool FuncDef::JitIsHotCandidate() { return get()->JitIsHotCandidate; }
inline void FuncDef::set_JitIsHotCandidate(bool _v) { get()->JitIsHotCandidate = _v; }
inline UInt64 FuncDef::JitObservedInstructions() { return get()->JitObservedInstructions; }
inline void FuncDef::set_JitObservedInstructions(UInt64 _v) { get()->JitObservedInstructions = _v; }
inline Int32 FuncDef::JitStubStateNone() { return get()->JitStubStateNone; }
inline Int32 FuncDef::JitStubStateCandidate() { return get()->JitStubStateCandidate; }
inline Int32 FuncDef::JitStubStateCompiled() { return get()->JitStubStateCompiled; }
inline Int32 FuncDef::JitStubStateFailed() { return get()->JitStubStateFailed; }
inline Int32 FuncDef::JitStubBackendNone() { return get()->JitStubBackendNone; }
inline Int32 FuncDef::JitStubBackendReturnNull() { return get()->JitStubBackendReturnNull; }
inline Int32 FuncDef::JitStubBackendReturnInt() { return get()->JitStubBackendReturnInt; }
inline Int32 FuncDef::JitStubBackendReturnConst() { return get()->JitStubBackendReturnConst; }
inline Int32 FuncDef::JitStubState() { return get()->JitStubState; }
inline void FuncDef::set_JitStubState(Int32 _v) { get()->JitStubState = _v; }
inline Int32 FuncDef::JitStubBackendKind() { return get()->JitStubBackendKind; }
inline void FuncDef::set_JitStubBackendKind(Int32 _v) { get()->JitStubBackendKind = _v; }
inline Int32 FuncDef::JitStubBackendIntValue() { return get()->JitStubBackendIntValue; }
inline void FuncDef::set_JitStubBackendIntValue(Int32 _v) { get()->JitStubBackendIntValue = _v; }
inline bool FuncDef::JitResetPrecompiled() { return get()->JitResetPrecompiled; }
inline void FuncDef::set_JitResetPrecompiled(bool _v) { get()->JitResetPrecompiled = _v; }
inline Int32 FuncDef::JitStubCompileAttempts() { return get()->JitStubCompileAttempts; }
inline void FuncDef::set_JitStubCompileAttempts(Int32 _v) { get()->JitStubCompileAttempts = _v; }
inline String FuncDef::JitStubLastError() { return get()->JitStubLastError; }
inline void FuncDef::set_JitStubLastError(String _v) { get()->JitStubLastError = _v; }
inline NativeCallbackDelegate FuncDef::NativeCallback() { return get()->NativeCallback; }
inline void FuncDef::set_NativeCallback(NativeCallbackDelegate _v) { get()->NativeCallback = _v; }
inline void FuncDef::ReserveRegister(Int32 registerNumber) { return get()->ReserveRegister(registerNumber); }
inline FuncDefStorage::operator bool() const {
	return Name != "";
}

} // end of namespace MiniScript
