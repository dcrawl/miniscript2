// AUTO-GENERATED FILE.  DO NOT MODIFY.
// Transpiled from: App.cs

#include "App.g.h"
#include "CodeEmitter.g.h"
#include "ErrorPool.g.h"
#include "UnitTests.g.h"
#include "VM.g.h"
#include "gc.h"
#include "gc_debug_output.h"
#include "value_string.h"
#include "dispatch_macros.h"
#include "VMVis.g.h"
#include "Assembler.g.h"
#include "Disassembler.g.h"
#include "Parser.g.h"
#include "CodeGenerator.g.h"
#include "StringUtils.g.h"
#include "IOHelper.g.h"
#include "Interpreter.g.h"
#include "Intrinsic.g.h" // ToDo: remove this once we've refactored set_FunctionIndexOffset away
#include <thread>
#include <chrono>
using namespace MiniScript;

int main(int argc, const char* argv[]) {
List<String> args;
for (int i=0; i<argc; i++) args.Add(String(argv[i]));
MiniScript::App::MainProgram(args);
}

namespace MiniScript {

const Int32 App::JitTierOff = 0;
const Int32 App::JitTierSuper = 1;
const Int32 App::JitTierStub = 2;
bool App::debugMode = false;
bool App::visMode = false;
Int32 App::jitTier = JitTierOff;
bool App::jitProfile = false;
Int32 App::jitHotThreshold = 100000;
Int32 App::ParseJitTier(String text) {
	if (String::IsNullOrEmpty(text)) return -1;
	String normalized = text.ToLower();
	if (normalized == "off" || normalized == "0") {
		return JitTierOff;
	}
	if (normalized == "super" || normalized == "on" || normalized == "1") {
		return JitTierSuper;
	}
	if (normalized == "stub" || normalized == "2") {
		return JitTierStub;
	}
	return -1;
}
String App::JitTierName(Int32 tier) {
	if (tier == JitTierSuper) return "super";
	if (tier == JitTierStub) return "stub";
	return "off";
}
String App::FormatCounter(UInt64 value) {
	// Transpiler-safe display path: UInt64 formatting in C++ currently degrades.
	if (value > 2147483647UL) return ">2.1B";
	return StringUtils::Format("{0}", (Int32)value);
}
String App::StubStateName(Int32 state) {
	if (state == 1) return "candidate";
	if (state == 2) return "compiled";
	if (state == 3) return "failed";
	return "none";
}
void App::ApplyRuntimeOptions(Interpreter interp) {
	if (IsNull(interp)) return;
	interp.set_JitTier(jitTier);
	interp.set_EnableJitProfiling(jitProfile);
	interp.set_JitHotThreshold(jitHotThreshold);
}
void App::MainProgram(List<String> args) {
	gc_init();
	value_init_constants();

	// Parse command-line switches
	Int32 fileArgIndex = -1;
	String inlineCode = nullptr;
	bool soakMode = Boolean(false);
	Int32 soakIterations = 500;
	for (Int32 i = 1; i < args.Count(); i++) {
		if (args[i] == "-debug" || args[i] == "-d") {
			debugMode = Boolean(true);
		} else if (args[i] == "-vis") {
			visMode = Boolean(true);
		} else if (args[i] == "-jit" && i + 1 < args.Count()) {
			i = i + 1;
			jitTier = ParseJitTier(args[i]);
			if (jitTier < 0) {
				IOHelper::Print(StringUtils::Format("Invalid -jit value: {0} (use off|super|stub)", args[i]));
				return;
			}
		} else if (args[i].StartsWith("-jit=")) {
			String tierText = args[i].Substring(5);
			jitTier = ParseJitTier(tierText);
			if (jitTier < 0) {
				IOHelper::Print(StringUtils::Format("Invalid -jit value: {0} (use off|super|stub)", tierText));
				return;
			}
		} else if (args[i] == "-jit-profile") {
			jitProfile = Boolean(true);
		} else if (args[i] == "-jit-hot" && i + 1 < args.Count()) {
			i = i + 1;
			jitHotThreshold = StringUtils::ParseInt32(args[i]);
			if (jitHotThreshold < 1) jitHotThreshold = 1;
		} else if (args[i].StartsWith("-jit-hot=")) {
			String thresholdText = args[i].Substring(9);
			jitHotThreshold = StringUtils::ParseInt32(thresholdText);
			if (jitHotThreshold < 1) jitHotThreshold = 1;
		} else if (args[i] == "-soak") {
			soakMode = Boolean(true);
		} else if (args[i].StartsWith("-soak=")) {
			soakMode = Boolean(true);
			String countText = args[i].Substring(6);
			if (!String::IsNullOrEmpty(countText)) {
				soakIterations = StringUtils::ParseInt32(countText);
			}
		} else if (args[i] == "-c" && i + 1 < args.Count()) {
			// Inline code follows -c
			i = i + 1;
			inlineCode = args[i];
		} else if (!args[i].StartsWith("-")) {
			// First non-switch argument is the file
			if (fileArgIndex == -1) fileArgIndex = i;
		}
	}
	
	IOHelper::Print("MiniScript 2.0");
	#if VM_USE_COMPUTED_GOTO
	#define VARIANT "(goto)"
	#else
	#define VARIANT "(switch)"
	#endif
	IOHelper::Print(
		"Build: C++ " VARIANT " version"
	);
	
	if (debugMode) {
		IOHelper::Print("Running unit tests...");
		if (!UnitTests::RunAll()) return;
		IOHelper::Print("Unit tests complete.");

		IOHelper::Print("Running integration tests...");
		if (!RunIntegrationTests("tests/testSuite.txt")) {
			IOHelper::Print("Some integration tests failed.");
//				return;
		}
		IOHelper::Print("Integration tests complete.");
	}

	if (soakMode) {
		if (fileArgIndex == -1) {
			IOHelper::Print("Soak mode requires a script file argument.");
			return;
		}
		String filePath = args[fileArgIndex];
		RunSoak(filePath, soakIterations);
		IOHelper::Print("All done!");
		return;
	}
	
	// Handle inline code (-c), file argument, or REPL
	if (!IsNull(inlineCode)) {
		if (debugMode) IOHelper::Print(StringUtils::Format("Compiling: {0}", inlineCode));
		Interpreter interp = CreateInterpreter();
		interp.Reset(inlineCode);
		RunInterpreter(interp);
	} else if (fileArgIndex != -1) {
		String filePath = args[fileArgIndex];
		Interpreter interp = CreateInterpreter();
		if (filePath.EndsWith(".ms")) {
			// Source file: read, join, and compile via Interpreter
			if (debugMode) IOHelper::Print(StringUtils::Format("Reading source file: {0}", filePath));
			List<String> lines = IOHelper::ReadFile(filePath);
			if (lines.Count() == 0) {
				IOHelper::Print("No lines read from file.");
			} else {
				String source = "";
				for (Int32 i = 0; i < lines.Count(); i++) {
					if (i > 0) source += "\n";
					source += lines[i];
				}
				if (debugMode) IOHelper::Print(StringUtils::Format("Parsing {0} lines...", lines.Count()));
				interp.Reset(source);
				RunInterpreter(interp);
			}
		} else {
			// Assembly file (.msa or any other extension)
			List<FuncDef> functions = AssembleFile(filePath);
			if (!IsNull(functions)) {
				interp.Reset(functions);
				RunInterpreter(interp);
			}
		}
	} else if (!debugMode) {
		// No file or inline code: enter REPL mode
		RunREPL();
	}

	IOHelper::Print("All done!");
}
Interpreter App::CreateInterpreter() {
	Interpreter interp =  Interpreter::New();
	ApplyRuntimeOptions(interp);
	interp.set_standardOutput([](String s, Boolean) { IOHelper::Print(s); });
	interp.set_errorOutput([](String s, Boolean) { IOHelper::Print(s); });
	return interp;
}
List<FuncDef> App::AssembleFile(String filePath) {
	if (debugMode) IOHelper::Print(StringUtils::Format("Reading assembly file: {0}", filePath));

	List<String> lines = IOHelper::ReadFile(filePath);
	if (lines.Count() == 0) {
		IOHelper::Print("No lines read from file.");
		return nullptr;
	}

	if (debugMode) IOHelper::Print(StringUtils::Format("Assembling {0} lines...", lines.Count()));
	Assembler assembler =  Assembler::New();

	// Assemble the code
	assembler.Assemble(lines);

	// Check for assembly errors
	if (assembler.HasError()) {
		IOHelper::Print("Assembly failed with errors.");
		return nullptr;
	}

	if (debugMode) IOHelper::Print("Assembly complete.");

	return assembler.Functions();
}
bool App::RunIntegrationTests(String filePath) {
	List<String> lines = IOHelper::ReadFile(filePath);
	if (lines.Count() == 0) {
		IOHelper::Print(StringUtils::Format("Could not read test file: {0}", filePath));
		return Boolean(false);
	}

	Int32 testCount = 0;
	Int32 passCount = 0;
	Int32 failCount = 0;

	// Parse and run tests
	List<String> inputLines =  List<String>::New();
	List<String> expectedLines =  List<String>::New();
	bool inExpected = Boolean(false);
	Int32 testStartLine = 0;

	for (Int32 i = 0; i < lines.Count(); i++) {
		String line = lines[i];

		// Lines starting with ==== are comments/separators
		if (line.StartsWith("====")) {
			// If we have a pending test, run it
			if (inputLines.Count() > 0) {
				testCount++;
				bool passed = RunSingleTest(inputLines, expectedLines, testStartLine);
				if (passed) {
					passCount++;
				} else {
					failCount++;
				}
			}
			// Reset for next test
			inputLines =  List<String>::New();
			expectedLines =  List<String>::New();
			inExpected = Boolean(false);
			testStartLine = i + 2;  // Next line after this comment
			continue;
		}

		// Lines starting with ---- separate input from expected output
		if (line.StartsWith("----")) {
			inExpected = Boolean(true);
			continue;
		}

		// Accumulate lines
		if (inExpected) {
			expectedLines.Add(line);
		} else {
			inputLines.Add(line);
		}
	}

	// Handle final test if file doesn't end with ====
	if (inputLines.Count() > 0) {
		testCount++;
		bool passed = RunSingleTest(inputLines, expectedLines, testStartLine);
		if (passed) {
			passCount++;
		} else {
			failCount++;
		}
	}

	// Report results
	IOHelper::Print(StringUtils::Format("Integration tests: {0} passed, {1} failed, {2} total",
		passCount, failCount, testCount));

	return failCount == 0;
}
bool App::RunSingleTest(List<String> inputLines,List<String> expectedLines,Int32 lineNum) {
	// Join input lines into source code
	String source = "";
	for (Int32 i = 0; i < inputLines.Count(); i++) {
		if (i > 0) source += "\n";
		source += inputLines[i];
	}

	// Skip empty tests
	if (String::IsNullOrEmpty(source.Trim())) return Boolean(true);

	// Set up print output capture
	List<String> printOutput =  List<String>::New();
	static List<String> gPrintOutput;
	gPrintOutput = printOutput;  // Use global reference

	// Compile and run via Interpreter
	Interpreter interp =  Interpreter::New();
	ApplyRuntimeOptions(interp);
	interp.set_standardOutput([](String s, Boolean) { gPrintOutput.Add(s); });
	interp.set_errorOutput([](String s, Boolean) { gPrintOutput.Add(s); });
	interp.Reset(source);
	interp.RunUntilDone();

	// Get expected output (join lines, trim trailing empty lines)
	String expected = "";
	for (Int32 i = 0; i < expectedLines.Count(); i++) {
		if (i > 0) expected += "\n";
		expected += expectedLines[i];
	}
	expected = expected.Trim();

	// Get actual output from print statements
	String actual = "";
	for (Int32 i = 0; i < printOutput.Count(); i++) {
		if (i > 0) actual += "\n";
		actual += printOutput[i];
	}
	actual = actual.Trim();

	if (actual != expected) {
		IOHelper::Print(StringUtils::Format("FAIL (line {0}): {1}", lineNum, source));
		IOHelper::Print(StringUtils::Format("Expected:\n{0}", expected));
		IOHelper::Print(StringUtils::Format("Actual:  \n{0}", actual));
		return Boolean(false);
	}

	return Boolean(true);
}
void App::RunInterpreter(Interpreter interp) {
	interp.Compile();
	VM vm = interp.vm();
	if (IsNull(vm)) return;		// compilation error (already reported)

	// Debug: disassemble and print
	if (debugMode) {
		List<FuncDef> functions = vm.GetFunctions();
		IOHelper::Print(StringUtils::Format("JIT tier: {0}; profiling: {1}; hot threshold: {2}",
			JitTierName(jitTier), jitProfile, jitHotThreshold));
		if (jitTier != JitTierOff) {
			IOHelper::Print(StringUtils::Format("Superinstruction rewrites this reset: {0}", vm.GetSuperinstructionRewriteCount()));
		}
		IOHelper::Print("Disassembly:\n");
		List<String> disassembly = Disassembler::Disassemble(functions, Boolean(true));
		for (Int32 i = 0; i < disassembly.Count(); i++) {
			IOHelper::Print(disassembly[i]);
		}

		IOHelper::Print(StringUtils::Format("Found {0} functions:", functions.Count()));
		for (Int32 i = 0; i < functions.Count(); i++) {
			FuncDef func = functions[i];
			IOHelper::Print(StringUtils::Format("  {0}: {1} instructions, {2} constants, MaxRegs={3}",
				func.Name(), func.Code().Count(), func.Constants().Count(), func.MaxRegs()));
		}

		IOHelper::Print("");
		IOHelper::Print("Executing @main with VM...");
	}

	GC_PUSH_SCOPE();
	Value result = val_null; GC_PROTECT(&result);

	if (visMode) {
		VMVis vis = VMVis(vm);
		vis.ClearScreen();
		while (vm.IsRunning()) {
			vis.UpdateDisplay();
			String cmd = IOHelper::Input("Command: ");
			if (String::IsNullOrEmpty(cmd)) cmd = "step";
			if (cmd[0] == 'q')  {
				GC_POP_SCOPE();
				return;
			}
			if (cmd[0] == 's') {
				result = vm.Run(1);
				continue;
			} else if (cmd == "gcmark") {
				vis.ClearScreen();
				gc_mark_and_report();
			} else if (cmd == "interndump") {
				vis.ClearScreen();
				dump_intern_table();
			} else {
				IOHelper::Print("Available commands:");
				IOHelper::Print("q[uit] -- Quit to shell");
				IOHelper::Print("s[tep] -- single-step VM");
				IOHelper::Print("gcmark -- run GC mark and show reachable objects (C++ only)");
				IOHelper::Print("interndump -- dump interned strings table (C++ only)");
			}
			IOHelper::Input("\n(Press Return.)");
			vis.ClearScreen();
		}
	} else {
		vm.set_DebugMode(debugMode);
		while (vm.IsRunning()) {
			result = vm.Run();
			if (vm.IsRunning()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
	}

	if (!vm.Errors().HasError()) {
		IOHelper::Print("\nVM execution complete. Result in r0:");
		IOHelper::Print(StringUtils::Format("\u001b[1;93m{0}\u001b[0m", result)); // (bold bright yellow)
		if (jitProfile) {
			List<FuncDef> functions = vm.GetFunctions();
			List<UInt64> counts = vm.GetFunctionExecutionCounts();
			List<Int32> selected =  List<Int32>::New();
			IOHelper::Print(StringUtils::Format("JIT profile: total instructions = {0}",
				FormatCounter(vm.GetTotalExecutedInstructions())));
			if (jitTier != JitTierOff) {
				IOHelper::Print(StringUtils::Format("JIT profile: superinstruction rewrites = {0}", vm.GetSuperinstructionRewriteCount()));
			}
			for (Int32 rank = 0; rank < 5; rank++) {
				Int32 bestIdx = -1;
				UInt64 bestCount = 0;
				for (Int32 i = 0; i < counts.Count(); i++) {
					UInt64 c = counts[i];
					if (c == 0) continue;
					bool alreadyPrinted = Boolean(false);
					for (Int32 prior = 0; prior < selected.Count(); prior++) {
						if (selected[prior] == i) {
							alreadyPrinted = Boolean(true);
							break;
						}
					}
					if (alreadyPrinted) continue;
					if (bestIdx < 0 || c > bestCount) {
						bestIdx = i;
						bestCount = c;
					}
				}
				if (bestIdx < 0) break;
				selected.Add(bestIdx);
				IOHelper::Print(StringUtils::Format("  hot[{0}] {1}: {2} steps{3}",
					rank + 1,
					functions[bestIdx].Name(),
					FormatCounter(bestCount),
					vm.IsFunctionHot(bestIdx) ? " (hot)" : ""));
			}

			List<Int32> candidates = vm.GetHotFunctionCandidates();
			if (candidates.Count() > 0) {
				IOHelper::Print(StringUtils::Format("JIT profile: top candidate slots = {0}", candidates.Count()));
				for (Int32 i = 0; i < candidates.Count(); i++) {
					Int32 idx = candidates[i];
					IOHelper::Print(StringUtils::Format("  cand[{0}] {1}: {2} steps, state={3}",
						i + 1,
						functions[idx].Name(),
						FormatCounter(functions[idx].JitObservedInstructions()),
						StubStateName(functions[idx].JitStubState())));
				}
			}

			if (jitTier == JitTierStub) {
				IOHelper::Print(StringUtils::Format(
					"JIT stub lifecycle: candidate={0}, compiled={1}, failed={2}, attempts={3}, compiled-routes={4}, compiled-fast={5}",
					vm.GetJitStubStateCount(1),
					vm.GetJitStubStateCount(2),
					vm.GetJitStubStateCount(3),
					vm.GetJitStubCompileAttemptCount(),
					vm.GetJitStubCompiledRouteHitCount(),
					vm.GetJitStubCompiledFastExecCount()));
				for (Int32 i = 0; i < functions.Count(); i++) {
					if (functions[i].JitStubState() == 3 && !String::IsNullOrEmpty(functions[i].JitStubLastError())) {
						IOHelper::Print(StringUtils::Format("  stub-failed {0}: {1}",
							functions[i].Name(),
							functions[i].JitStubLastError()));
					}
				}
			}
		}
	}
	GC_POP_SCOPE();
}
void App::RunREPL() {
	Interpreter interp =  Interpreter::New();
	ApplyRuntimeOptions(interp);
	interp.set_standardOutput([](String s, Boolean) { IOHelper::Print(s); });
	interp.set_errorOutput([](String s, Boolean) { IOHelper::Print(s); });
	interp.set_implicitOutput([](String s, Boolean) { IOHelper::Print(s); });
	while (Boolean(true)) {
		String prompt = interp.NeedMoreInput() ? ">>> " : "> ";
		String line = IOHelper::Input(prompt);
		if (IsNull(line)) break;
		interp.REPL(line, 60);
	}
}
bool App::PrepareInterpreterForFile(Interpreter interp,String filePath) {
	if (filePath.EndsWith(".ms")) {
		List<String> lines = IOHelper::ReadFile(filePath);
		if (lines.Count() == 0) {
			IOHelper::Print("No lines read from file.");
			return Boolean(false);
		}
		String source = "";
		for (Int32 i = 0; i < lines.Count(); i++) {
			if (i > 0) source += "\n";
			source += lines[i];
		}
		interp.Reset(source);
		interp.Compile();
		return !IsNull(interp.vm());
	}

	List<FuncDef> functions = AssembleFile(filePath);
	if (IsNull(functions)) return Boolean(false);
	interp.Reset(functions);
	return !IsNull(interp.vm());
}
void App::RunSoak(String filePath,Int32 iterations) {
	if (iterations < 1) iterations = 1;

	IOHelper::Print(StringUtils::Format("Running soak test: {0} iterations ({1})",
		iterations, filePath));

	Interpreter interp = CreateInterpreter();
	GC_PUSH_SCOPE();
	Value result = val_null; GC_PROTECT(&result);
	if (!PrepareInterpreterForFile(interp, filePath)) {
		IOHelper::Print("Soak setup failed.");
		GC_POP_SCOPE();
		return;
	}

	Value baselineResult = val_null; GC_PROTECT(&baselineResult);
	for (Int32 i = 0; i < iterations; i++) {
		interp.Restart();
		interp.RunUntilDone(60, Boolean(false));

		if (interp.Running()) {
			IOHelper::Print(StringUtils::Format(
				"SOAK FAIL at iteration {0}: script did not complete.", i + 1));
			GC_POP_SCOPE();
			return;
		}

		if (IsNull(interp.vm()) || interp.vm().Errors().HasError()) {
			IOHelper::Print(StringUtils::Format(
				"SOAK FAIL at iteration {0}: runtime error.", i + 1));
			GC_POP_SCOPE();
			return;
		}

		result = interp.vm().GetStackValue(0);
		if (i == 0) {
			baselineResult = result;
		} else if (!value_equal(result, baselineResult)) {
			IOHelper::Print(StringUtils::Format(
				"SOAK FAIL at iteration {0}: result drift (expected {1}, got {2}).",
				i + 1, baselineResult, result));
			GC_POP_SCOPE();
			return;
		}

		if (((i + 1) % 100) == 0 || (i + 1) == iterations) {
			IOHelper::Print(StringUtils::Format("  progress: {0}/{1}", i + 1, iterations));
		}
	}

	IOHelper::Print(StringUtils::Format(
		"Soak passed. Stable result in r0: {0}", baselineResult));
	GC_POP_SCOPE();
}

} // end of namespace MiniScript
