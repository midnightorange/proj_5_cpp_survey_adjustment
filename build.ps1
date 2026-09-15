# Build script when CMake is unavailable (Windows PowerShell + g++)
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$build = Join-Path $root 'build'
New-Item -ItemType Directory -Force -Path $build | Out-Null

$cxx = 'g++'
$std = '-std=c++17'
$opt = '-O2'

Write-Host 'Building CLI...'
& $cxx $std $opt "-I$root\include" "$root\src\matrix.cpp" "$root\src\leveling.cpp" "$root\src\main_cli.cpp" -o "$build\adjustment_cli.exe"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host 'Building tests...'
& $cxx $std $opt "-I$root\include" "$root\src\matrix.cpp" "$root\tests\test_matrix.cpp" -o "$build\test_matrix.exe"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $cxx $std $opt "-I$root\include" "$root\src\matrix.cpp" "$root\src\leveling.cpp" "$root\tests\test_leveling.cpp" -o "$build\test_leveling.exe"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $cxx $std $opt "-I$root\include" "$root\src\matrix.cpp" "$root\src\leveling.cpp" "$root\tests\test_leveling_complex.cpp" -o "$build\test_leveling_complex.exe"
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Set-Location $root
Write-Host ''
Write-Host 'Running tests...'
& "$build\test_matrix.exe"
& "$build\test_leveling.exe"
& "$build\test_leveling_complex.exe"

Write-Host ''
Write-Host 'Running example...'
& "$build\adjustment_cli.exe" "$root\data\leveling_example.txt"
