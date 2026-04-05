// AUTO-GENERATED FILE.  DO NOT MODIFY.
// Transpiled from: ScriptInstance.cs

#include "ScriptInstance.g.h"

namespace MiniScript {

const Int32 ScriptInstanceStorage::StatusNotCompiled = 0;
const Int32 ScriptInstanceStorage::StatusRunning = 1;
const Int32 ScriptInstanceStorage::StatusYielded = 2;
const Int32 ScriptInstanceStorage::StatusCompleted = 3;
const Int32 ScriptInstanceStorage::StatusFaulted = 4;
ScriptInstanceStorage::ScriptInstanceStorage(TextOutputMethod standardOutput,TextOutputMethod errorOutput) {
	errors =  List<String>::New();
	interpreter =  Interpreter::New((String)nullptr, standardOutput, errorOutput);
}
void ScriptInstanceStorage::LoadSource(String source) {
	ClearErrors();
	interpreter.Reset(source);
}
void ScriptInstanceStorage::LoadFunctions(List<FuncDef> functions) {
	ClearErrors();
	interpreter.Reset(functions);
}
bool ScriptInstanceStorage::Compile() {
	interpreter.Compile();
	return !IsNull(interpreter.vm());
}
bool ScriptInstanceStorage::RunForSeconds(double timeBudgetSeconds,bool returnEarlyOnYield) {
	if (IsNull(interpreter.vm())) {
		interpreter.Compile();
		if (IsNull(interpreter.vm())) return Boolean(false);
	}
	interpreter.RunUntilDone(timeBudgetSeconds, returnEarlyOnYield);
	if (!IsNull(interpreter.vm()) && !String::IsNullOrEmpty(interpreter.vm().RuntimeError())) {
		errors.Add(interpreter.vm().RuntimeError());
	}
	return interpreter.Running();
}
Int32 ScriptInstanceStorage::RunFrame(UInt32 maxInstructions,bool stopOnYield) {
	if (IsNull(interpreter.vm())) {
		interpreter.Compile();
		if (IsNull(interpreter.vm())) return StatusNotCompiled;
	}

	Int32 errorCountBefore = errors.Count();
	interpreter.vm().set_yielding(Boolean(false));

	for (UInt32 i = 0; i < maxInstructions; i++) {
		if (!interpreter.Running()) break;
		interpreter.Step();
		if (!IsNull(interpreter.vm()) && !String::IsNullOrEmpty(interpreter.vm().RuntimeError())) {
			errors.Add(interpreter.vm().RuntimeError());
			return StatusFaulted;
		}
		if (errors.Count() > errorCountBefore) return StatusFaulted;
		if (stopOnYield && IsYielding()) return StatusYielded;
	}

	if (errors.Count() > errorCountBefore) return StatusFaulted;
	if (!interpreter.Running()) return StatusCompleted;
	if (IsYielding()) return StatusYielded;
	return StatusRunning;
}
bool ScriptInstanceStorage::RunCycles(UInt32 maxCycles) {
	RunFrame(maxCycles, Boolean(true));
	return interpreter.Running();
}
bool ScriptInstanceStorage::IsRunning() {
	return interpreter.Running();
}
bool ScriptInstanceStorage::IsYielding() {
	return !IsNull(interpreter.vm()) && interpreter.vm().yielding();
}
void ScriptInstanceStorage::Stop() {
	interpreter.Stop();
}
void ScriptInstanceStorage::Restart() {
	interpreter.Restart();
}
Value ScriptInstanceStorage::GetGlobalValue(String varName) {
	return interpreter.GetGlobalValue(varName);
}
void ScriptInstanceStorage::SetGlobalValue(String varName,Value value) {
	interpreter.SetGlobalValue(varName, value);
}
Int32 ScriptInstanceStorage::ErrorCount() {
	return errors.Count();
}
List<String> ScriptInstanceStorage::GetErrors() {
	return errors;
}
void ScriptInstanceStorage::ClearErrors() {
	errors.Clear();
}

} // end of namespace MiniScript
