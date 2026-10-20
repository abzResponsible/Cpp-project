$ErrorActionPreference = "Stop"
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vsPath = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vsPath) {
    throw "Visual Studio C++ tools were not found."
}
$vcvars = Join-Path $vsPath "VC\Auxiliary\Build\vcvars64.bat"
if (-not (Test-Path $vcvars)) {
    throw "vcvars64.bat was not found at $vcvars"
}

New-Item -ItemType Directory -Force -Path "$PSScriptRoot\build" | Out-Null
$src = Join-Path $PSScriptRoot "src\server.cpp"
$out = Join-Path $PSScriptRoot "build\quiet-pages.exe"

$cmd = "`"$vcvars`" && cl /nologo /EHsc /std:c++17 /O2 /Fe:`"$out`" `"$src`" ws2_32.lib"
cmd /c $cmd
if ($LASTEXITCODE -ne 0) {
    throw "Compile failed."
}
Write-Host "Built $out"
