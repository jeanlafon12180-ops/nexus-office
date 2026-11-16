#define AppName "Nexus Office"
#define AppVersion "0.2.0"
#define AppPublisher "Nexus"
#define AppExeName "nexus-office.exe"

#ifndef SourceDir
  #define SourceDir "..\\..\\dist\\bin"
#endif
#ifndef HelperDir
  #define HelperDir "..\\..\\dist\\share\\nexus-office"
#endif
#ifndef OutputDir
  #define OutputDir "..\\.."
#endif

[Setup]
AppId={{A0C7A86B-2D34-4B6E-9F25-0D37D6B86D12}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#AppPublisher}
DefaultDirName={autopf}\Nexus Office
DefaultGroupName=Nexus Office
DisableProgramGroupPage=yes
OutputDir={#OutputDir}
OutputBaseFilename=NexusOffice-Setup-0.2.0-x64
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
UninstallDisplayIcon={app}\{#AppExeName}

[Languages]
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Créer une icône sur le Bureau"; GroupDescription: "Options supplémentaires :"; Flags: unchecked

[Files]
Source: "{#SourceDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\Nexus Office"; Filename: "{app}\{#AppExeName}"
Name: "{autodesktop}\Nexus Office"; Filename: "{app}\{#AppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#AppExeName}"; Description: "Lancer Nexus Office"; Flags: postinstall nowait skipifsilent
