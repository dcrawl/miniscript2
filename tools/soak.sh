#!/bin/bash
# MiniScript2 soak validation runner

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

ITERATIONS=500
LANG="both"  # cs|cpp|both
CS_SCRIPT="examples/recur_fib.ms"
CPP_SCRIPT="examples/recur_fib.msa"
CPP_GOTO_MODE="off"  # off|on

usage() {
    echo "Usage: $0 [-n ITERATIONS] [-lang cs|cpp|both] [-cs-script PATH] [-cpp-script PATH] [-cpp-goto off|on]"
    echo ""
    echo "Examples:"
    echo "  $0 -n 2000"
    echo "  $0 -n 2000 -lang cs"
    echo "  $0 -n 2000 -lang cpp -cpp-goto on"
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -n|--iterations)
            ITERATIONS="$2"
            shift 2
            ;;
        -lang|--lang)
            LANG="$2"
            shift 2
            ;;
        -cs-script)
            CS_SCRIPT="$2"
            shift 2
            ;;
        -cpp-script)
            CPP_SCRIPT="$2"
            shift 2
            ;;
        -cpp-goto)
            CPP_GOTO_MODE="$2"
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

if ! [[ "$ITERATIONS" =~ ^[0-9]+$ ]] || [[ "$ITERATIONS" -lt 1 ]]; then
    echo "Error: iterations must be a positive integer."
    exit 1
fi

if [[ "$LANG" != "cs" && "$LANG" != "cpp" && "$LANG" != "both" ]]; then
    echo "Error: -lang must be one of cs, cpp, both."
    exit 1
fi

if [[ "$CPP_GOTO_MODE" != "on" && "$CPP_GOTO_MODE" != "off" ]]; then
    echo "Error: -cpp-goto must be one of on, off."
    exit 1
fi

if [[ "$LANG" == "cs" || "$LANG" == "both" ]]; then
    if [[ ! -f "$CS_SCRIPT" ]]; then
        echo "Error: C# soak script not found: $CS_SCRIPT"
        exit 1
    fi

    echo "[soak 1/2] Building C# runtime"
    ./tools/build.sh cs
    echo "[soak 1/2] Running C# soak ($ITERATIONS iterations): $CS_SCRIPT"
    ./build/cs/miniscript2 -soak="$ITERATIONS" "$CS_SCRIPT"
fi

if [[ "$LANG" == "cpp" || "$LANG" == "both" ]]; then
    if [[ ! -f "$CPP_SCRIPT" ]]; then
        echo "Error: C++ soak script not found: $CPP_SCRIPT"
        exit 1
    fi

    echo "[soak 2/2] Transpiling C# -> C++"
    ./tools/build.sh transpile
    echo "[soak 2/2] Building C++ runtime (computed-goto $CPP_GOTO_MODE)"
    ./tools/build.sh cpp "$CPP_GOTO_MODE"
    echo "[soak 2/2] Running C++ soak ($ITERATIONS iterations): $CPP_SCRIPT"
    ./build/cpp/miniscript2 -soak="$ITERATIONS" "$CPP_SCRIPT"
fi

echo "Soak validation complete."
