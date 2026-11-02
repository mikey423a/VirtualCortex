$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$root = Split-Path $PSScriptRoot -Parent
$build = Join-Path $root 'build-windows'
$dist = Join-Path $root 'dist'
foreach ($evidence in @('standalone-verification.txt', 'vst3-verification.txt')) {
    if (-not (Test-Path "$build/$evidence")) { throw "Missing verification evidence: $evidence" }
}
New-Item -ItemType Directory -Force $dist | Out-Null
@'
Virtual Cortex 6.3.0 Windows x64 test build
Built with JUCE 8.0.10 and ASIO enabled.
CI requires CTest regression success, pluginval level 5 success and a standalone
startup test before packaging. Anubis input/output and Windows 11 user audio
behavior still need testing. The installer is unsigned.
Select your Merging ASIO device and channels in the standalone audio settings.
'@ | Set-Content "$dist/TESTING-NOTES.txt"
$iscc = 'C:/Program Files (x86)/Inno Setup 6/ISCC.exe'
if (-not (Test-Path $iscc)) {
    choco install innosetup --version=6.4.3 -y --no-progress
    if ($LASTEXITCODE -ne 0) { throw 'Inno Setup installation failed' }
}
if (-not (Test-Path $iscc)) { throw 'Inno Setup compiler not found' }
& $iscc "/DBuildRoot=$build" "/DDistDir=$dist" "$root/installer/VirtualCortex.iss"
if ($LASTEXITCODE -ne 0) { throw 'Installer compilation failed' }
$setup = Join-Path $dist 'VirtualCortex-6.3.0-Windows-x64-Setup.exe'
if (-not (Test-Path $setup)) { throw 'Installer output missing' }
# Test installer file deployment and uninstall without launching audio devices.
$testDir = Join-Path $env:RUNNER_TEMP 'VirtualCortex-Install-Test'
$p = Start-Process -FilePath $setup -ArgumentList @('/VERYSILENT', '/SUPPRESSMSGBOXES', '/NORESTART', "/DIR=`"$testDir`"", "/LOG=`"$dist/install-test.log`"") -PassThru -Wait
if ($p.ExitCode -ne 0) { throw "Installer failed: $($p.ExitCode)" }
$installed = Join-Path $testDir 'Virtual Cortex.exe'
$original = Join-Path $build 'VirtualCortex_artefacts/Release/Standalone/Virtual Cortex.exe'
if (-not (Test-Path $installed)) { throw 'Installed EXE missing' }
if ((Get-FileHash $installed).Hash -ne (Get-FileHash $original).Hash) { throw 'Installed EXE does not match build' }
$vstInstalled = Join-Path ${env:CommonProgramFiles} 'VST3/Virtual Cortex.vst3/Contents/x86_64-win/Virtual Cortex.vst3'
if (-not (Test-Path $vstInstalled)) { throw 'Installed VST3 module missing' }
$p = Start-Process -FilePath "$testDir/unins000.exe" -ArgumentList @('/VERYSILENT','/SUPPRESSMSGBOXES','/NORESTART',"/LOG=`"$dist/uninstall-test.log`"") -PassThru -Wait
if ($p.ExitCode -ne 0) { throw "Uninstall failed: $($p.ExitCode)" }
if ((Test-Path $installed) -or (Test-Path $vstInstalled)) { throw 'Uninstall left application binaries installed' }
Compress-Archive -Path "$build/VirtualCortex_artefacts/Release/VST3/Virtual Cortex.vst3" -DestinationPath "$dist/VirtualCortex-6.3.0-VST3-x64.zip" -Force
Copy-Item $original "$dist/Virtual Cortex.exe"
Get-ChildItem $dist -File | Where-Object Extension -in '.exe','.zip' | Get-FileHash -Algorithm SHA256 | Format-Table -AutoSize | Out-String -Width 4096 | Set-Content "$dist/SHA256SUMS.txt"
