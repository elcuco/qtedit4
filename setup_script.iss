; Inno Setup script example

[Setup]
AppName=qteidt4
AppVersion=0.1
DefaultDirName={pf}\qtedit4
DefaultGroupName=qtedit4
UninstallDisplayIcon={app}\bin\qtedit4.exe
OutputDir=Output
OutputBaseFilename=qtedit4-win64
Compression=lzma
SolidCompression=yes

[Files]
Source: "build\dist\windows-msvc\bin\qtedit.exe"; DestDir: "{app}"; Flags: ignoreversion
;Source: "build\dist\share\YourAppName\styles\*"; DestDir: "{app}\styles"; Flags: recursesubdirs

[Icons]
Name: "{group}\qtedit4"; Filename: "{app}\qtedit4.exe"
;Name: "{commondesktop}\YourAppName"; Filename: "{app}\YourApp.exe"; Tasks: desktopicon

;[Tasks]
;Name: desktopicon; Description: "Create a desktop icon"; GroupDescription: "Additional icons:"
