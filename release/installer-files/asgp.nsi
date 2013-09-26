; Windows installer for Andy's Super Great Park

!include "MUI2.nsh"

;--------------------------------

; The name of the installer
Name "Andy's Super Great Park"

; The file to write
OutFile asgp.exe

; The default installation directory
InstallDir "$LOCALAPPDATA\Andy's Super Great Park"
InstallDirRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\asgp" "UninstallString"

SetCompressor /SOLID lzma

;Request application privileges for Windows Vista
RequestExecutionLevel user

;--------------------------------
; Interface Configuration

!define MUI_HEADERIMAGE
        !define MUI_HEADERIMAGE_BITMAP "banner.bmp"

!define MUI_WELCOMEFINISHPAGE_BITMAP "welcome.bmp" 

;Show all languages, despite user's codepage
!define MUI_LANGDLL_ALLLANGUAGES

;--------------------------------
;Language Selection Dialog Settings

;Remember the installer language
!define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 
!define MUI_LANGDLL_REGISTRY_KEY "Software\Andy's Super Great Park" 
!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

;--------------------------------

Var StartMenuFolder

;--------------------------------

!insertmacro MUI_PAGE_WELCOME

!insertmacro MUI_PAGE_DIRECTORY

;Start Menu Folder Page Configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Andy's Super Great Park" 
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  
!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\Readme.txt"
!define MUI_FINISHPAGE_RUN "$INSTDIR\bin\andy-super-great-park.exe"
!insertmacro MUI_PAGE_FINISH

;--------------------------------
; Languages
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "Portuguese"

!insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------
Section "Install"

  ; Set output path to the installation directory.
  SetOutPath "$INSTDIR\bin"
  
  File /r "bin\*.dll"
  File /r "bin\andy-super-great-park.exe"

  SetOutPath "$INSTDIR\share\super-great-park"

  File /r "share\super-great-park\"

  SetOutPath "$INSTDIR\share\locale"

  File /r "share\locale\"

  SetOutPath "$INSTDIR"

  File "asgp.ico"
  File "ReadMe.txt"
  File "ChangeLog"

  CreateDirectory "$SMPROGRAMS\Andy's Super Great Park"

  SetOutPath "$INSTDIR\bin"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    CreateShortCut "$SMPROGRAMS\Andy's Super Great Park\Andy's Super Great Park.lnk" "$INSTDIR\bin\andy-super-great-park.exe" "" "$INSTDIR\asgp.ico"

    SetOutPath "$INSTDIR"

    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"

    CreateShortCut "$SMPROGRAMS\Andy's Super Great Park\Uninstall.lnk" "$INSTDIR\uninst-asgp.exe"

    CreateShortCut "$SMPROGRAMS\Andy's Super Great Park\ReadMe.lnk" "$INSTDIR\ReadMe.txt"

  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd ; end of "Install"

;--------------------------------
;Installer Functions

Function .onInit

  !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

;--------------------------------
Section -PostInstall

  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\asgp" "DisplayName" "Andy's Super Great Park (uninstall)"

  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\asgp" "UninstallString" '"$INSTDIR\uninst-asgp.exe"'

  WriteUninstaller "uninst-asgp.exe"

SectionEnd

;UninstallText "Ready to uninstall Andy's Super Great Park."

;--------------------------------
Section "Uninstall"
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\asgp"
  DeleteRegKey /ifempty HKCU "Software\Andy's Super Great Park"

  Delete "$INSTDIR\uninst-asgp.exe"
  Delete "$INSTDIR\asgp.ico"
  Delete "$INSTDIR\ReadMe.txt"
  Delete "$INSTDIR\ChangeLog"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Andy's Super Great Park.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\ReadMe.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"

  RMDir /r "$SMPROGRAMS\Andy's Super Great Park"
  RMDir /r "$INSTDIR\share"
  RMDir /r "$INSTDIR\bin"

SectionEnd

;--------------------------------
;Uninstaller Functions

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE
  
FunctionEnd
