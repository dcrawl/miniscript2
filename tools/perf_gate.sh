#!/bin/bash
# MiniScript2 performance + soak gate runner

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

SOAK_ITERS=1000
BENCH_LANGS="cs,cpp-switch,cpp-goto"
QUICK_MODE=0
REPORT_DIR="build/reports"
RUN_ID="$(date +%Y%m%d_%H%M%S)"
REPORT_PATH="${REPORT_DIR}/perf_gate_${RUN_ID}.md"
BENCH_LOG="${REPORT_DIR}/perf_gate_${RUN_ID}_benchmark.log"

usage() {
    echo "Usage: $0 [--quick] [-n ITERATIONS] [-bench-langs LANGS] [-report PATH]"
    echo ""
    echo "Options:"
    echo "  --quick                 Run a faster gate (reduced soak/benchmark scope)"
    echo "  -n, --iterations N      Soak iterations (default: 1000)"
    echo "  -bench-langs LANGS      Benchmark runtime variants (cs,cpp-switch,cpp-goto)"
    echo "                          default: cs,cpp-switch,cpp-goto"
    echo "  -report PATH            Output report path"
    echo ""
    echo "Examples:"
    echo "  $0"
    echo "  $0 --quick"
    echo "  $0 -n 2000 -bench-langs cpp-goto"
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --quick)
            QUICK_MODE=1
            shift
            ;;
        -n|--iterations)
            SOAK_ITERS="$2"
            shift 2
            ;;
        -bench-langs)
            BENCH_LANGS="$2"
            shift 2
            ;;
        -report)
            REPORT_PATH="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

if ! [[ "$SOAK_ITERS" =~ ^[0-9]+$ ]] || [[ "$SOAK_ITERS" -lt 1 ]]; then
    echo "Error: soak iterations must be a positive integer."
    exit 1
fi

if [[ "$QUICK_MODE" -eq 1 ]]; then
    SOAK_ITERS=100
    BENCH_LANGS="cpp-goto"
fi

verify_result() {
    local actual="$1"
    local expected="$2"
    local name="$3"

    if [[ "$name" == "factorial_iterative" ]]; then
        if [[ "$actual" == "2.43290200817664E+18" || "$actual" == "2.4329e+18" || "$actual" == "2432902008176640000" ]]; then
            return 0
        fi
    elif [[ "$actual" == "$expected" ]]; then
        return 0
    fi
    return 1
}

contains_lang() {
    local lang="$1"
    [[ ",$BENCH_LANGS," == *",$lang,"* ]]
}

run_one_benchmark() {
    local exe="$1"
    local script_path="$2"
    local expected="$3"
    local name="$4"

    local started ended elapsed output actual
    started="$(date +%s)"
    output="$($exe "$script_path" 2>&1)"
    ended="$(date +%s)"
    elapsed="$((ended - started))"
    actual="$(printf '%s\n' "$output" | awk '/Result in r0:/{getline; print; exit}')"

    if verify_result "$actual" "$expected" "$name"; then
        echo "  - $name: PASS ($elapsed s, result=$actual)" | tee -a "$BENCH_LOG"
        return 0
    fi

    echo "  - $name: FAIL (expected=$expected, actual=${actual:-<empty>})" | tee -a "$BENCH_LOG"
    return 1
}

run_benchmark_variant() {
    local variant="$1"
    local exe=""

    echo "Running benchmark variant: $variant" | tee -a "$BENCH_LOG"

    case "$variant" in
        "cs")
            ./tools/build.sh cs >> "$BENCH_LOG" 2>&1
            exe="./build/cs/miniscript2"
            ;;
        "cpp-switch")
            ./tools/build.sh transpile >> "$BENCH_LOG" 2>&1
            ./tools/build.sh cpp off >> "$BENCH_LOG" 2>&1
            exe="./build/cpp/miniscript2"
            ;;
        "cpp-goto")
            ./tools/build.sh transpile >> "$BENCH_LOG" 2>&1
            ./tools/build.sh cpp on >> "$BENCH_LOG" 2>&1
            exe="./build/cpp/miniscript2"
            ;;
        *)
            echo "Unsupported benchmark variant: $variant" | tee -a "$BENCH_LOG"
            return 1
            ;;
    esac

    run_one_benchmark "$exe" "tools/benchmarks/factorial_iterative.ms" "2432902008176640000" "factorial_iterative" || return 1
    run_one_benchmark "$exe" "tools/benchmarks/iter_fib.ms" "832040" "iter_fib" || return 1
    run_one_benchmark "$exe" "tools/benchmarks/recur_fib.ms" "3524578" "recur_fib" || return 1
    return 0
}

mkdir -p "$REPORT_DIR"

start_epoch="$(date +%s)"
start_human="$(date)"

{
    echo "# Performance + Soak Gate Report"
    echo ""
    echo "- Start: $start_human"
    echo "- Soak iterations: $SOAK_ITERS"
    echo "- Benchmark languages: $BENCH_LANGS"
    if [[ "$QUICK_MODE" -eq 1 ]]; then
        echo "- Mode: quick"
    else
        echo "- Mode: full"
    fi
    echo ""
    echo "## Benchmark"
    echo ""
} > "$REPORT_PATH"

echo "[perf-gate 1/4] Running benchmarks: $BENCH_LANGS"
echo "Benchmark variants: $BENCH_LANGS" > "$BENCH_LOG"
bench_ok=1

if contains_lang "cs"; then
    run_benchmark_variant "cs" || bench_ok=0
fi
if contains_lang "cpp-switch"; then
    run_benchmark_variant "cpp-switch" || bench_ok=0
fi
if contains_lang "cpp-goto"; then
    run_benchmark_variant "cpp-goto" || bench_ok=0
fi

if [[ "$bench_ok" -eq 1 ]]; then
    echo "- Benchmark status: PASS" >> "$REPORT_PATH"
else
    echo "- Benchmark status: FAIL" >> "$REPORT_PATH"
    echo "" >> "$REPORT_PATH"
    echo "Gate failed during benchmark stage." >> "$REPORT_PATH"
    exit 1
fi

echo "" >> "$REPORT_PATH"
echo "Benchmark log: $BENCH_LOG" >> "$REPORT_PATH"
echo "" >> "$REPORT_PATH"
echo "## Soak" >> "$REPORT_PATH"
echo "" >> "$REPORT_PATH"

echo "[perf-gate 2/4] Running soak in C#"
./tools/soak.sh -n "$SOAK_ITERS" -lang cs

echo "[perf-gate 3/4] Running soak in C++ switch"
./tools/soak.sh -n "$SOAK_ITERS" -lang cpp -cpp-goto off

echo "[perf-gate 4/4] Running soak in C++ computed-goto"
./tools/soak.sh -n "$SOAK_ITERS" -lang cpp -cpp-goto on

echo "- Soak status: PASS (cs, cpp-switch, cpp-goto)" >> "$REPORT_PATH"

end_epoch="$(date +%s)"
end_human="$(date)"
elapsed="$((end_epoch - start_epoch))"

{
    echo ""
    echo "## Summary"
    echo ""
    echo "- End: $end_human"
    echo "- Duration (seconds): $elapsed"
    echo "- Overall gate: PASS"
} >> "$REPORT_PATH"

echo "Performance + soak gate passed."
echo "Report: $REPORT_PATH"
