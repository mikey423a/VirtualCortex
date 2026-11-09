# Build Virtual Cortex locally on Windows; GitHub is not required.
# Example: .\build-windows.ps1 -JucePath 'C:\SDK\JUCE'
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$JucePath,
    [string]$BuildDir = 'build-windows',
    [switch]$SkipTests,
    [switch]$EnableAsio,
    [string]$AsioSdkPath = '',
    [string]$NamPath = ''
)
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$root = $PSScriptRoot
# Fail early on unsupported host environments; JUCE Windows builds require MSVC.
if ($env:OS -ne 'Windows_NT') {
    throw 'This build script must run on Windows with Visual Studio 2022 C++ tools installed.'
}
if (-not (Get-Command 'vswhere.exe' -ErrorAction SilentlyContinue)) {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (Test-Path $vswhere) {
        $vsInstall = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        if (-not $vsInstall) { throw 'Visual Studio 2022 Desktop development with C++ workload is required.' }
    } else {
        Write-Warning 'vswhere.exe not found: CMake will verify the Visual Studio 2022 generator.'
    }
}

$juce = [System.IO.Path]::GetFullPath($JucePath)
if (-not (Test-Path (Join-Path $juce 'CMakeLists.txt'))) {
    throw "JUCE source not found at '$juce'. Extract the JUCE 8.0.10 source and pass its root with -JucePath."
}
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    throw 'CMake not found. Install CMake and add it to PATH.'
}
if (-not $SkipTests -and -not (Get-Command ctest -ErrorAction SilentlyContinue)) {
    throw 'CTest not found. Install a CMake distribution that includes ctest and add it to PATH.'
}
$build = if ([System.IO.Path]::IsPathRooted($BuildDir)) {
    [System.IO.Path]::GetFullPath($BuildDir)
} else {
    [System.IO.Path]::GetFullPath((Join-Path $root $BuildDir))
}
# Avoid shipping binaries left over from an earlier successful build.
# CMake's generated artefact folder is cleared before this build begins.
$oldArtifacts = Join-Path $build 'VirtualCortex_artefacts'
$buildStart = Get-Date
# Avoid stale cached configuration from a previous JUCE checkout / build option.
$cmakeCache = Join-Path $build 'CMakeCache.txt'
if (Test-Path $cmakeCache) { Remove-Item $cmakeCache -Force }

if (Test-Path $oldArtifacts) { Remove-Item -Recurse -Force $oldArtifacts }
New-Item -ItemType Directory -Force -Path $build | Out-Null
$log = Join-Path $build 'windows-build.log'
if (Test-Path $log) { Remove-Item $log -Force }
function Invoke-LoggedCommand {
    param([string]$Step, [string[]]$Arguments)
    Add-Content -Path $log -Value "`n=== $Step ==="
    # Preserve stderr and the actual cmake exit code; don't treat Tee-Object
    # succeeding as evidence that the compiler succeeded.
    $ErrorActionPreference = 'Continue'
    try {
        & cmake @Arguments 2>&1 | Tee-Object -FilePath $log -Append
        $code = $LASTEXITCODE
    } finally {
        $ErrorActionPreference = 'Stop'
    }
    if ($code -ne 0) { throw "$Step failed (exit code $code). See $log" }
}

$asioOption = if ($EnableAsio) { '-DVC_ENABLE_ASIO=ON' } else { '-DVC_ENABLE_ASIO=OFF' }
Write-Host "Windows build: $(if ($EnableAsio) { 'ASIO enabled (SDK required)' } else { 'default Windows audio backend' })" 
Invoke-LoggedCommand -Step 'Configure' -Arguments @('-S', $root, '-B', $build, '-G', 'Visual Studio 17 2022', '-A', 'x64', '-DBUILD_TESTING=ON', "-DVC_JUCE_SOURCE_DIR=$juce", '-DVC_ALLOW_JUCE_FETCH=OFF', $asioOption, "-DVC_ASIO_SDK_DIR=$AsioSdkPath", "-DVC_NAM_SOURCE_DIR=$NamPath")
Invoke-LoggedCommand -Step 'Build' -Arguments @('--build', $build, '--config', 'Release', '--parallel', '2')
if (-not $SkipTests) {
    Add-Content -Path $log -Value "`n=== CTest ==="
    $ErrorActionPreference = 'Continue'
    try {
        & ctest --test-dir $build -C Release --output-on-failure 2>&1 | Tee-Object -FilePath $log -Append
        $testExit = $LASTEXITCODE
    } finally {
        $ErrorActionPreference = 'Stop'
    }
    if ($testExit -ne 0) { throw "Automated tests failed (code $testExit). See $log" }
}
$artifactRoot = Join-Path $build 'VirtualCortex_artefacts\Release'
$standalone = Join-Path $artifactRoot 'Standalone'
$vst = Join-Path $artifactRoot 'VST3'
if (-not (Test-Path $standalone) -or -not (Test-Path $vst)) {
    throw "Standalone or VST3 output folder is missing. See $log"
}
$exes = @(Get-ChildItem $standalone -Recurse -File -Filter '*.exe' -ErrorAction Stop | Where-Object { $_.Name -like '*Virtual*Cortex*' })
# JUCE's Windows VST3 bundle includes Contents/x86_64-win/*.vst3.
$modules = @(Get-ChildItem $vst -Recurse -File -Filter '*.vst3' -ErrorAction Stop | Where-Object { $_.FullName -match '[/\\]Contents[/\\]x86_64-win[/\\]' })
# The outputs must be freshly produced, not merely present on disk.
$exes = @($exes | Where-Object { $_.LastWriteTime -ge $buildStart.AddSeconds(-3) })
$modules = @($modules | Where-Object { $_.LastWriteTime -ge $buildStart.AddSeconds(-3) })
if ($exes.Count -eq 0 -or $modules.Count -eq 0) {
    throw "Expected standalone EXE and VST3 module were not found in $artifactRoot"
}
$package = Join-Path $build 'VirtualCortex-Windows-x64.zip'
if (Test-Path $package) { Remove-Item $package -Force }
Compress-Archive -Path $standalone,$vst -DestinationPath $package -ErrorAction Stop
if (-not (Test-Path $package) -or (Get-Item $package).Length -eq 0) {
    throw 'Windows package was not created correctly.'
}
# Provide reproducible evidence of what was packaged.
$manifest = Join-Path $build 'windows-build-manifest.txt'
@(
    'Virtual Cortex 6.23.0',
    ('Built: ' + (Get-Date).ToUniversalTime().ToString('o')),
    ('JUCE source: ' + $juce),
    ('ASIO enabled: ' + [bool]$EnableAsio),
    ('Standalone: ' + ($exes | ForEach-Object { $_.FullName } | Out-String).Trim()),
    ('VST3 module: ' + ($modules | ForEach-Object { $_.FullName } | Out-String).Trim()),
    ('Package SHA256: ' + (Get-FileHash $package -Algorithm SHA256).Hash)
) | Set-Content -Path $manifest -Encoding UTF8
Write-Host "Build succeeded. Package: $package"
Write-Host "Build manifest: $manifest"
Get-FileHash $package -Algorithm SHA256 | Format-List
Write-Host 'Audio note: ASIO requires -EnableAsio and a JUCE-compatible Steinberg ASIO SDK. Anubis ASIO has not been verified.'
