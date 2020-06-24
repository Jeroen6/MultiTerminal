; Inno Setup 5.5.5          
[Setup]
#define ApplicationName 'MultiTerminal'
#define ApplicationVersion GetFileVersion('.\deploy\MultiTerminal.exe')
AppName={#ApplicationName}
AppVerName={#ApplicationName} {#ApplicationVersion}
VersionInfoVersion={#ApplicationVersion}

OutputBaseFilename={#ApplicationName} {#ApplicationVersion} installer

; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{91D112F3-BA62-4D7A-8326-8AAFC3D102F9}
AppPublisher=github.com/jeroen6
AppPublisherURL=github.com/jeroen6
AppSupportURL=github.com/jeroen6
AppUpdatesURL=github.com/jeroen6
DefaultDirName={pf}\MultiTerminal
DefaultGroupName=MultiTerminal
OutputDir=.

Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Registry]


[Files]
Source: ".\deploy\MultiTerminal.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\deploy\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\MultiTerminal"; Filename: "{app}\MultiTerminal.exe"
Name: "{commondesktop}\MultiTerminal"; Filename: "{app}\MultiTerminal.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\MultiTerminal.exe"; Description: "{cm:LaunchProgram,MultiTerminal}"; Flags: nowait postinstall skipifsilent
