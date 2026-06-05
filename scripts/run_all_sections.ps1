$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
Set-Location $Root

$configs = @(
    "configs/section0_csr_justify.json",
    "configs/section1_scaling_n.json",
    "configs/section2_thread_scaling.json",
    "configs/section3_topology.json",
    "configs/section4_summary.json"
)

$ts = Get-Date -Format "yyyy-MM-dd_HHmm"
$outDir = Join-Path $Root "results\$ts"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

Write-Host "Output directory: $outDir"
Write-Host "Building once..."
if (Get-Command nvcc -ErrorAction SilentlyContinue) { make } else { make NO_CUDA=1 }
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

foreach ($cfg in $configs) {
    $name = [System.IO.Path]::GetFileNameWithoutExtension($cfg)
    $csv = Join-Path $outDir "$name.csv"
    Write-Host "`n========== $name =========="
    & ".\mis.exe" --config $cfg --output $csv
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Failed on $cfg"
        exit $LASTEXITCODE
    }
}

Write-Host "`nAll sections finished. CSV files in $outDir"
