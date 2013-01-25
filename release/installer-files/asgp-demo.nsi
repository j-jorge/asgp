; Windows installer for Andy's Super Great Park Demo

!include "MUI2.nsh"

;--------------------------------

; The name of the installer
Name "Andy's Super Great Park Demo"

; The file to write
OutFile asgp-demo.exe

; The default installation directory
InstallDir "$LOCALAPPDATA\Andy's Super Great Park Demo"
InstallDirRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\asgp-demo" "UninstallString"

SetCompressor /SOLID lzma

;Request application privileges for Windows Vista
RequestExecutionLevel user

;--------------------------------
; Interface Configuration

!define MUI_HEADERIMAGE
        !define MUI_HEADERIMAGE_BITMAP "banner.bmp"

!define MUI_WELCOMEFINISHPAGE_BITMAP "welcome.bmp" 

;--------------------------------

Var StartMenuFolder

;--------------------------------

!insertmacro MUI_PAGE_WELCOME

!insertmacro MUI_PAGE_DIRECTORY

;Start Menu Folder Page Configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Andy's Super Great Park Demo" 
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  
!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\Readme.txt"
!define MUI_FINISHPAGE_RUN "$INSTDIR\bin\andy-super-great-park-demo.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Launch the game."
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

;--------------------------------
Section "Install"

  ; Set output path to the installation directory.
  SetOutPath "$INSTDIR\bin"
  
  File /r "bin\*.dll"
  File /r "bin\andy-super-great-park-demo.exe"

  SetOutPath "$INSTDIR\share\super-great-park"

  File /r "share\super-great-park\"

  SetOutPath "$INSTDIR\share\super-great-park\level"

  File /r "share\super-great-park\level\*.cl"
  File /r "share\super-great-park\level\1"

  SetOutPath "$INSTDIR\share\locale"

  File /r "share\locale\"

  SetOutPath "$INSTDIR"

  File "asgp.ico"
  File "ReadMe.txt"
  File "ChangeLog"

  CreateDirectory "$SMPROGRAMS\Andy's Super Great Park Demo"

  SetOutPath "$INSTDIR\bin"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    CreateShortCut "$SMPROGRAMS\Andy's Super Great Park Demo\Andy's Super Great Park Demo.lnk" "$INSTDIR\bin\andy-super-great-park-demo.exe" "" "$INSTDIR\asgp.ico"

    SetOutPath "$INSTDIR"

    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"

    CreateShortCut "$SMPROGRAMS\Andy's Super Great Park Demo\Uninstall.lnk" "$INSTDIR\uninst-asgp-demo.exe"

    CreateShortCut "$SMPROGRAMS\Andy's Super Great Park Demo\ReadMe.lnk" "$INSTDIR\ReadMe.txt"

  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd ; end of "Install"

;--------------------------------
Section -PostInstall

  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\asgp-demo" "DisplayName" "Andy's Super Great Park Demo (uninstall)"

  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\asgp-demo" "UninstallString" '"$INSTDIR\uninst-asgp-demo.exe"'

  WriteUninstaller "uninst-asgp-demo.exe"

SectionEnd

UninstallText "Ready to uninstall Andy's Super Great Park Demo."

;--------------------------------
Section "Uninstall"
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\asgp-demo"
  Delete "$INSTDIR\uninst-asgp-demo.exe"
  Delete "$INSTDIR\asgp.ico"
  Delete "$INSTDIR\ReadMe.txt"
  Delete "$INSTDIR\ChangeLog"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Andy's Super Great Park Demo.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\ReadMe.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"

  RMDir /r "$SMPROGRAMS\Andy's Super Great Park Demo"
  RMDir /r "$INSTDIR\share"
  RMDir /r "$INSTDIR\bin"

SectionEnd
