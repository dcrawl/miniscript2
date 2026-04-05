#!/bin/bash
# MiniScript2 combined ship gate runner

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

QUICK_MODE=0
PERF_ARGS=()
REPORT_DIR="build/reports"
RUN_ID="$(date +%Y%m%d_%H%M%S)"
REPORT_PATH="${REPORT_DIR}/ship_gate_${RUN_ID}.md"

usage() {
    echo "Usage: $0 [--quick] [--perf-arg ARG ...]"
    echo ""
    echo "Options:"
    echo "  --quick               Use quick perf gate mode"
    echo "  --perf-arg ARG        Forward one argument to perf gate (repeatable)"
    echo ""
    echo "Examples:"
    echo "  $0"
    echo "  $0 --quick"
    echo "  $0 --perf-arg -n --perf-arg 2000"
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --quick)
            QUICK_MODE=1
            shift
            ;;
        --perf-arg)
            PERF_ARGS+=("$2")
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

mkdir -p "$REPORT_DIR"

start_epoch="$(date +%s)"
start_human="$(date)"

{
    echo "# Ship Gate Report"
    echo ""
    echo "- Start: $start_human"
    if [[ "$QUICK_MODE" -eq 1 ]]; then
        echo "- Mode: quick"
    else
        echo "- Mode: full"
    fi
    echo ""
    echo "## Contract Presence"
} > "$REPORT_PATH"

require_file() {
    local p="$1"
    if [[ -f "$p" ]]; then
        echo "- PASS: $p" | tee -a "$REPORT_PATH"
    else
        echo "- FAIL: Missing required file $p" | tee -a "$REPORT_PATH"
        exit 1
    fi
}

require_file "notes/SCRIPT_COMPATIBILITY_V1.md"
require_file "notes/INTRINSIC_SURFACE_V1.md"
require_file "notes/PERFORMANCE_GATE_V1.md"

echo "" | tee -a "$REPORT_PATH"
echo "## CI Gate" | tee -a "$REPORT_PATH"
./tools/build.sh ci-gate

echo "- PASS: ci-gate" >> "$REPORT_PATH"

echo "" >> "$REPORT_PATH"
echo "## Performance Gate" >> "$REPORT_PATH"
if [[ "$QUICK_MODE" -eq 1 ]]; then
    if [[ ${#PERF_ARGS[@]} -gt 0 ]]; then
        ./tools/build.sh perf-gate --quick "${PERF_ARGS[@]}"
    else
        ./tools/build.sh perf-gate --quick
    fi
else
    if [[ ${#PERF_ARGS[@]} -gt 0 ]]; then
        ./tools/build.sh perf-gate "${PERF_ARGS[@]}"
    else
        ./tools/build.sh perf-gate
    fi
fi

echo "- PASS: perf-gate" >> "$REPORT_PATH"

end_epoch="$(date +%s)"
end_human="$(date)"
elapsed="$((end_epoch - start_epoch))"

{
    echo ""
    echo "## Summary"
    echo ""
    echo "- End: $end_human"
    echo "- Duration (seconds): $elapsed"
    echo "- Overall: PASS"
} >> "$REPORT_PATH"

echo "Ship gate passed."
echo "Report: $REPORT_PATH"
