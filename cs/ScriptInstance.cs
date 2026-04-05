using System;
using System.Collections.Generic;

namespace MiniScript {

/// <summary>
/// ScriptInstance provides a stable host-facing API boundary around Interpreter.
/// Use this from an embedding host instead of reaching into VM internals.
/// </summary>
public class ScriptInstance {
	private readonly Interpreter _interpreter;
	private readonly List<String> _errors;

	public ScriptInstance(TextOutputMethod standardOutput=null, TextOutputMethod errorOutput=null) {
		_errors = new List<String>();
		_interpreter = new Interpreter((String)null, standardOutput, null);
		_interpreter.errorOutput = (String message, bool addLineBreak) => {
			_errors.Add(message);
			if (errorOutput != null) {
				errorOutput(message, addLineBreak);
			}
		};
	}

	public void LoadSource(String source) {
		ClearErrors();
		_interpreter.Reset(source);
	}

	public void LoadFunctions(List<FuncDef> functions) {
		ClearErrors();
		_interpreter.Reset(functions);
	}

	public bool Compile() {
		_interpreter.Compile();
		return _interpreter.vm != null;
	}

	public bool RunForSeconds(double timeBudgetSeconds=0.001, bool returnEarlyOnYield=true) {
		if (_interpreter.vm == null) {
			_interpreter.Compile();
			if (_interpreter.vm == null) return false;
		}
		_interpreter.RunUntilDone(timeBudgetSeconds, returnEarlyOnYield);
		return _interpreter.Running();
	}

	public bool RunCycles(UInt32 maxCycles=1000) {
		if (_interpreter.vm == null) {
			_interpreter.Compile();
			if (_interpreter.vm == null) return false;
		}
		_interpreter.vm.Run(maxCycles);
		return _interpreter.Running();
	}

	public bool IsRunning() {
		return _interpreter.Running();
	}

	public bool IsYielding() {
		return _interpreter.vm != null && _interpreter.vm.yielding;
	}

	public void Stop() {
		_interpreter.Stop();
	}

	public void Restart() {
		_interpreter.Restart();
	}

	public Value GetGlobalValue(String varName) {
		return _interpreter.GetGlobalValue(varName);
	}

	public void SetGlobalValue(String varName, Value value) {
		_interpreter.SetGlobalValue(varName, value);
	}

	public Int32 ErrorCount() {
		return _errors.Count;
	}

	public List<String> GetErrors() {
		return _errors;
	}

	public void ClearErrors() {
		_errors.Clear();
	}
}

}