// AUTO-GENERATED FILE.  DO NOT MODIFY.
// Transpiled from: ScriptInstance.cs

#pragma once
#include "core_includes.h"
#include "forward_decs.g.h"
#include "Interpreter.g.h"
#include "IntrinsicAPI.g.h"
#include "FuncDef.g.h"

namespace MiniScript {

// DECLARATIONS

class ScriptInstanceStorage : public std::enable_shared_from_this<ScriptInstanceStorage> {
	friend struct ScriptInstance;
	public: static const Int32 StatusNotCompiled;
	public: static const Int32 StatusRunning;
	public: static const Int32 StatusYielded;
	public: static const Int32 StatusCompleted;
	public: static const Int32 StatusFaulted;
	private: Interpreter interpreter;
	private: List<String> errors;

	public: ScriptInstanceStorage(TextOutputMethod standardOutput=null, TextOutputMethod errorOutput=null);

	public: void LoadSource(String source);

	public: void LoadFunctions(List<FuncDef> functions);

	public: bool Compile();

	public: bool RunForSeconds(double timeBudgetSeconds=0.001, bool returnEarlyOnYield=true);

	// Run a deterministic instruction budget and report detailed run status.
	public: Int32 RunFrame(UInt32 maxInstructions=1000, bool stopOnYield=true);

	public: bool RunCycles(UInt32 maxCycles=1000);

	public: bool IsRunning();

	public: bool IsYielding();

	public: void Stop();

	public: void Restart();

	public: Value GetGlobalValue(String varName);

	public: void SetGlobalValue(String varName, Value value);

	public: Int32 ErrorCount();

	public: List<String> GetErrors();

	public: void ClearErrors();
}; // end of class ScriptInstanceStorage

/// <summary>
/// ScriptInstance provides a stable host-facing API boundary around Interpreter.
/// Use this from an embedding host instead of reaching into VM internals.
/// </summary>
struct ScriptInstance {
	friend class ScriptInstanceStorage;
	protected: std::shared_ptr<ScriptInstanceStorage> storage;
  public:
	ScriptInstance(std::shared_ptr<ScriptInstanceStorage> stor) : storage(stor) {}
	ScriptInstance() : storage(nullptr) {}
	ScriptInstance(std::nullptr_t) : storage(nullptr) {}
	friend bool IsNull(const ScriptInstance& inst) { return inst.storage == nullptr; }
	private: ScriptInstanceStorage* get() const;

	public: Int32 StatusNotCompiled();
	public: Int32 StatusRunning();
	public: Int32 StatusYielded();
	public: Int32 StatusCompleted();
	public: Int32 StatusFaulted();
	private: Interpreter interpreter();
	private: void set_interpreter(Interpreter _v);
	private: List<String> errors();
	private: void set_errors(List<String> _v);

	public: static ScriptInstance New(TextOutputMethod standardOutput=null, TextOutputMethod errorOutput=null) {
		return ScriptInstance(std::make_shared<ScriptInstanceStorage>(standardOutput, errorOutput));
	}

	public: inline void LoadSource(String source);

	public: inline void LoadFunctions(List<FuncDef> functions);

	public: inline bool Compile();

	public: inline bool RunForSeconds(double timeBudgetSeconds=0.001, bool returnEarlyOnYield=true);

	// Run a deterministic instruction budget and report detailed run status.
	public: inline Int32 RunFrame(UInt32 maxInstructions=1000, bool stopOnYield=true);

	public: inline bool RunCycles(UInt32 maxCycles=1000);

	public: inline bool IsRunning();

	public: inline bool IsYielding();

	public: inline void Stop();

	public: inline void Restart();

	public: inline Value GetGlobalValue(String varName);

	public: inline void SetGlobalValue(String varName, Value value);

	public: inline Int32 ErrorCount();

	public: inline List<String> GetErrors();

	public: inline void ClearErrors();
}; // end of struct ScriptInstance

// INLINE METHODS

inline ScriptInstanceStorage* ScriptInstance::get() const { return static_cast<ScriptInstanceStorage*>(storage.get()); }
inline Int32 ScriptInstance::StatusNotCompiled() { return get()->StatusNotCompiled; }
inline Int32 ScriptInstance::StatusRunning() { return get()->StatusRunning; }
inline Int32 ScriptInstance::StatusYielded() { return get()->StatusYielded; }
inline Int32 ScriptInstance::StatusCompleted() { return get()->StatusCompleted; }
inline Int32 ScriptInstance::StatusFaulted() { return get()->StatusFaulted; }
inline Interpreter ScriptInstance::interpreter() { return get()->interpreter; }
inline void ScriptInstance::set_interpreter(Interpreter _v) { get()->interpreter = _v; }
inline List<String> ScriptInstance::errors() { return get()->errors; }
inline void ScriptInstance::set_errors(List<String> _v) { get()->errors = _v; }
inline void ScriptInstance::LoadSource(String source) { return get()->LoadSource(source); }
inline void ScriptInstance::LoadFunctions(List<FuncDef> functions) { return get()->LoadFunctions(functions); }
inline bool ScriptInstance::Compile() { return get()->Compile(); }
inline bool ScriptInstance::RunForSeconds(double timeBudgetSeconds,bool returnEarlyOnYield) { return get()->RunForSeconds(timeBudgetSeconds, returnEarlyOnYield); }
inline Int32 ScriptInstance::RunFrame(UInt32 maxInstructions,bool stopOnYield) { return get()->RunFrame(maxInstructions, stopOnYield); }
inline bool ScriptInstance::RunCycles(UInt32 maxCycles) { return get()->RunCycles(maxCycles); }
inline bool ScriptInstance::IsRunning() { return get()->IsRunning(); }
inline bool ScriptInstance::IsYielding() { return get()->IsYielding(); }
inline void ScriptInstance::Stop() { return get()->Stop(); }
inline void ScriptInstance::Restart() { return get()->Restart(); }
inline Value ScriptInstance::GetGlobalValue(String varName) { return get()->GetGlobalValue(varName); }
inline void ScriptInstance::SetGlobalValue(String varName,Value value) { return get()->SetGlobalValue(varName, value); }
inline Int32 ScriptInstance::ErrorCount() { return get()->ErrorCount(); }
inline List<String> ScriptInstance::GetErrors() { return get()->GetErrors(); }
inline void ScriptInstance::ClearErrors() { return get()->ClearErrors(); }

} // end of namespace MiniScript
