; ultimatestunts.nsi
;
; This script is based on example2.nsi 
;
; It will install makensisw.exe into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "Ultimate Stunts"

; The file to write
OutFile "setup.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\Ultimate Stunts"

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Ultimate_Stunts" "Install_Dir"


SetCompressor bzip2

LicenseData copying

;--------------------------------
; Pages

Page license
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Ultimate Stunts (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.

  SetOutPath $INSTDIR
  
  ; Put file there
  File *.bat
  File *.dll
  File *.ico
  File *.conf
  File *.txt
  File copying
  File /r data
  File /r doc
  File /r bin
  CreateDirectory $INSTDIR\saveddata
  SetFileAttributes saveddata NORMAL
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\Ultimate_Stunts "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ultimate_Stunts" "DisplayName" "Ultimate Stunts"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ultimate_Stunts" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ultimate_Stunts" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ultimate_Stunts" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; The stuff to install
Section "OpenAL (required)"
  SectionIn RO

  SetOutPath $INSTDIR
  File oalinst.exe
  Exec '"$INSTDIR\oalinst.exe"'
SectionEnd

; Optional sections (can be disabled by the user)

Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\Ultimate Stunts"
  CreateShortCut "$SMPROGRAMS\Ultimate Stunts\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Ultimate Stunts\Ultimate Stunts.lnk" "$INSTDIR\ustunts.bat" "" "$INSTDIR\ultimatestunts.ico" 0
  CreateShortCut "$SMPROGRAMS\Ultimate Stunts\Track Editor.lnk" "$INSTDIR\ustuntstrackedit.bat" "" "$INSTDIR\ultimatestunts.ico" 0
  CreateShortCut "$SMPROGRAMS\Ultimate Stunts\Manual.lnk" "$INSTDIR\doc\index.htm"
SectionEnd

Section "Desktop Shortcut"
  CreateShortCut "$DESKTOP\Ultimate Stunts.lnk" "$INSTDIR\ustunts.bat" "" "$INSTDIR\ultimatestunts.ico" 0
SectionEnd


;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Ultimate_Stunts"
  DeleteRegKey HKLM SOFTWARE\Ultimate_Stunts

  ; Remove files and uninstaller
  Delete $INSTDIR\copying
  Delete $INSTDIR\*.conf
  Delete $INSTDIR\*.dat
  Delete $INSTDIR\*.txt
  Delete $INSTDIR\*.ico
  Delete $INSTDIR\*.dll
  Delete $INSTDIR\*.exe
  Delete $INSTDIR\*.bat
  Delete $INSTDIR\*.log

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Ultimate Stunts\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Ultimate Stunts"
  Delete "$DESKTOP\Ultimate Stunts.lnk"
  RMDir /r $INSTDIR\data
  RMDir /r $INSTDIR\doc
  RMDir /r $INSTDIR\bin
  RMDir "$INSTDIR"

SectionEnd