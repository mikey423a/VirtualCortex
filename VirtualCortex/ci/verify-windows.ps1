$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$root = Split-Path $PSScriptRoot -Parent
$build = Join-Path $root 'build-windows'
$vst = Join-Path $build 'VirtualCortex_artefacts/Release/VST3/Virtual Cortex.vst3'
$exe = Join-Path $build 'VirtualCortex_artefacts/Release/Standalone/Virtual Cortex.exe'
$tools = Join-Path $build 'validation-tools'
New-Item -ItemType Directory -Force $tools | Out-Null
Invoke-WebRequest 'https://github.com/Tracktion/pluginval/releases/download/v1.0.4/pluginval_Windows.zip' -OutFile "$tools/pluginval.zip"
Expand-Archive "$tools/pluginval.zip" "$tools/pluginval" -Force
$validator = @(Get-ChildItem "$tools/pluginval" -Recurse -File -Filter pluginval.exe)
if ($validator.Count -ne 1) { throw 'pluginval executable missing or ambiguous' }
& $validator[0].FullName --strictness-level 5 --validate $vst 2>&1 | Tee-Object "$build/vst3-verification.txt"
if ($LASTEXITCODE -ne 0) { throw "VST3 validation failed: $LASTEXITCODE" }
# Startup test only: this runner cannot test the user's Anubis hardware.
$p = Start-Process -FilePath $exe -PassThru
try {
    Start-Sleep -Seconds 10
    $p.Refresh()
    if ($p.HasExited) { throw "Standalone exited during startup: $($p.ExitCode)" }
    'PASS: standalone remained running for 10 seconds. Audio hardware and Anubis not tested.' | Set-Content "$build/standalone-verification.txt"
} finally {
    $p.Refresh()
    if (-not $p.HasExited) {
        $null = $p.CloseMainWindow()
        if (-not $p.WaitForExit(5000)) { Stop-Process -Id $p.Id -Force }
    }
}
