#ifndef BuildRoot
  #error BuildRoot must point to the verified build-windows directory
#endif
#ifndef DistDir
  #error DistDir must point to the output directory
#endif
[Setup]
AppId={{143CC6AA-FE5D-43B6-A7A5-8EC2A8D13E28}
AppName=Virtual Cortex
AppVersion=6.18.0
AppPublisher=VirtualCortex
DefaultDirName={autopf}\Virtual Cortex
DefaultGroupName=Virtual Cortex
UninstallDisplayIcon={app}\Virtual Cortex.exe
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0.17763
PrivilegesRequired=admin
OutputDir={#DistDir}
OutputBaseFilename=VirtualCortex-6.18.0-Windows-x64-Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
[Files]
Source: "{#BuildRoot}\VirtualCortex_artefacts\Release\Standalone\Virtual Cortex.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildRoot}\VirtualCortex_artefacts\Release\VST3\Virtual Cortex.vst3\*"; DestDir: "{commoncf64}\VST3\Virtual Cortex.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#DistDir}\TESTING-NOTES.txt"; DestDir: "{app}"; Flags: ignoreversion
[Icons]
Name: "{group}\Virtual Cortex"; Filename: "{app}\Virtual Cortex.exe"
Name: "{group}\Uninstall Virtual Cortex"; Filename: "{uninstallexe}"
[Run]
Filename: "{app}\Virtual Cortex.exe"; Description: "Launch Virtual Cortex"; Flags: nowait postinstall skipifsilent
