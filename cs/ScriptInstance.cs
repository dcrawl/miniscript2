using System;
using System.Collections.Generic;
// H: #include "Interpreter.g.h"
// H: #include "IntrinsicAPI.g.h"
// H: #include "FuncDef.g.h"

namespace MiniScript {

/// <summary>
/// ScriptInstance provides a stable host-facing API boundary around Interpreter.
/// Use this from an embedding host instead of reaching into VM internals.
/// </summary>
public class ScriptInstance {
	public const Int32 StatusNotCompiled = 0;
	public const Int32 StatusRunning = 1;
	public const Int32 StatusYielded = 2;
	public const Int32 StatusCompleted = 3;
	public const Int32 StatusFaulted = 4;

	private Interpreter interpreter;
	private List<String> errors;

	public ScriptInstance(TextOutputMethod standardOutput=null, TextOutputMethod errorOutput=null) {
		errors = new List<String>();
		interpreter = new Interpreter((String)null, standardOutput, errorOutput);
	}

	public void LoadSource(String source) {
		ClearErrors();
		interpreter.Reset(source);
	}

	public void LoadFunctions(List<FuncDef> functions) {
		ClearErrors();
		interpreter.Reset(functions);
	}

	public bool Compile() {
		interpreter.Compile();
		return interpreter.vm != null;
	}

	public bool RunForSeconds(double timeBudgetSeconds=0.001, bool returnEarlyOnYield=true) {
		if (interpreter.vm == null) {
			interpreter.Compile();
			if (interpreter.vm == null) return false;
		}
		interpreter.RunUntilDone(timeBudgetSeconds, returnEarlyOnYield);
		if (interpreter.vm != null && !String.IsNullOrEmpty(interpreter.vm.RuntimeError)) {
			errors.Add(interpreter.vm.RuntimeError);
		}
		return interpreter.Running();
	}

	// Run a deterministic instruction budget and report detailed run status.
	public Int32 RunFrame(UInt32 maxInstructions=1000, bool stopOnYield=true) {
		if (interpreter.vm == null) {
			interpreter.Compile();
			if (interpreter.vm == null) return StatusNotCompiled;
		}

		Int32 errorCountBefore = errors.Count;
		interpreter.vm.yielding = false;

		for (UInt32 i = 0; i < maxInstructions; i++) {
			if (!interpreter.Running()) break;
			interpreter.Step();
			if (interpreter.vm != null && !String.IsNullOrEmpty(interpreter.vm.RuntimeError)) {
				errors.Add(interpreter.vm.RuntimeError);
				return StatusFaulted;
			}
			if (errors.Count > errorCountBefore) return StatusFaulted;
			if (stopOnYield && IsYielding()) return StatusYielded;
		}

		if (errors.Count > errorCountBefore) return StatusFaulted;
		if (!interpreter.Running()) return StatusCompleted;
		if (IsYielding()) return StatusYielded;
		return StatusRunning;
	}

	public bool RunCycles(UInt32 maxCycles=1000) {
		RunFrame(maxCycles, true);
		return interpreter.Running();
	}

	public bool IsRunning() {
		return interpreter.Running();
	}

	public bool IsYielding() {
		return interpreter.vm != null && interpreter.vm.yielding;
	}

	public void Stop() {
		interpreter.Stop();
	}

	public void Restart() {
		interpreter.Restart();
	}

	public Value GetGlobalValue(String varName) {
		return interpreter.GetGlobalValue(varName);
	}

	public void SetGlobalValue(String varName, Value value) {
		interpreter.SetGlobalValue(varName, value);
	}

	public Int32 ErrorCount() {
		return errors.Count;
	}

	public List<String> GetErrors() {
		return errors;
	}

	public void ClearErrors() {
		errors.Clear();
	}
}

}