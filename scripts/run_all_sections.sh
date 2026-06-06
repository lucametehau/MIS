#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

configs=(
    configs/section0_csr_justify.json
    configs/section1_scaling_n.json
    configs/section2_thread_scaling.json
    configs/section3_topology.json
    configs/section4_single_run.json
)

ts="$(date +%Y-%m-%d_%H%M)"
out_dir="$ROOT/results/$ts"
mkdir -p "$out_dir"

echo "Output directory: $out_dir"
echo "Building once..."
if command -v nvcc >/dev/null 2>&1; then
    make
else
    make NO_CUDA=1
fi

for cfg in "${configs[@]}"; do
    name="$(basename "$cfg" .json)"
    csv="$out_dir/$name.csv"
    echo ""
    echo "========== $name =========="
    ./mis --config "$cfg" --output "$csv"
done

echo ""
echo "All sections finished. CSV files in $out_dir"
