#define MyAppName "Appeal"
#define MyAppVersion "1.0.0-beta.1"
#define MyAppPublisher "Red Whistle"
#define MyAppExeName "Appeal.exe"

[Setup]
AppId={{A63D1B7D-4251-45A4-9151-6E6B6553B4E8}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=yes
OutputDir=..\..\Dist
OutputBaseFilename=Appeal-{#MyAppVersion}-windows-x64-installer
Compression=lzma
SolidCompression=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
SetupIconFile=..\..\Resources\Graphics\Icons\red_whistle_icon.ico
UninstallDisplayIcon={app}\{#MyAppExeName}

[Files]
Source: "..\..\Appeal.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\Resources\*"; DestDir: "{userdocs}\Appeal\Resources"; Flags: ignoreversion recursesubdirs createallsubdirs onlyifdoesntexist
Source: "..\..\README.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\LICENSE"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\VERSION"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\Appeal"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{userdocs}\Appeal"

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "Launch Appeal"; Flags: nowait postinstall skipifsilent; WorkingDir: "{userdocs}\Appeal"
