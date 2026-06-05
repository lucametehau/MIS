param(
    [string]$Config = "configs/smoke_test.json",
    [switch]$WarmCache,
    [switch]$DryRun,
    [string]$Output = ""
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
Set-Location $Root

Write-Host "Building (NO_CUDA=1 if nvcc unavailable)..."
if (Get-Command nvcc -ErrorAction SilentlyContinue) { make } else { make NO_CUDA=1 }
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$ts = Get-Date -Format "yyyy-MM-dd_HHmm"
$outDir = Join-Path $Root "results\$ts"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

$configBase = [System.IO.Path]::GetFileNameWithoutExtension($Config)
$csvPath = if ($Output) { $Output } else { Join-Path $outDir "$configBase.csv" }

$args = @("--config", $Config, "--output", $csvPath)
if ($WarmCache) { $args += "--warm-cache" }
if ($DryRun) { $args += "--dry-run" }

Write-Host "Running: mis.exe $($args -join ' ')"
& ".\mis.exe" @args
exit $LASTEXITCODE
