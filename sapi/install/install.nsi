; Script generated with the Venis Install Wizard

; Define your application name
!define APPNAME "HTS SAPI Voices"
!define APPNAMEANDVERSION "HTS SAPI Voices 0.9"

; Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES\HTSSAPIVoices"
InstallDirRegKey HKLM "Software\${APPNAME}" ""
OutFile "htssapi.exe"

; Modern interface settings
!include "MUI.nsh"
!include "Library.nsh"
!include "x64.nsh"

!define MUI_ABORTWARNING

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\..\license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Set languages (first is default language)
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL

Section -FinishSection

  ; Install 32-bit voices on all systems
	SetOutPath "$INSTDIR\x86"
	SetOverwrite try
	File "..\bin\IEShims.dll"
	File "..\bin\atl110.dll"
	;File "..\bin\htstts.dll"
	File "..\bin\msvcp110.dll"
	File "..\bin\msvcr110.dll"
	SetOverwrite on
	File "..\bin\register-voice.exe"	
	
	!define LIBRARY_COM
	!define LIBRARY_IGNORE_VERSION
	
	;	Register 32-bit DLL
	!insertmacro InstallLib REGDLL NOTSHARED NOREBOOT_NOTPROTECTED "..\bin\htstts.dll" "$INSTDIR\x86\htstts.dll" "$SYSDIR"
	
	WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteUninstaller "$INSTDIR\uninstall.exe"

	; Startmenu
	CreateDirectory "$SMPROGRAMS\HTS SAPI Voices"
	CreateShortCut "$SMPROGRAMS\HTS SAPI Voices\Uninstall.lnk" "$INSTDIR\uninstall.exe"	
			
	; Install 64-bit voices only on 64-bit systems	
	${If} ${RunningX64}
		SetOutPath "$INSTDIR\x64"
		SetOverwrite try
		File "..\bin64\IEShims.dll"
		File "..\bin64\atl110.dll"	
		File "..\bin64\msvcp110.dll"
		File "..\bin64\msvcr110.dll"
		SetOverwrite on
		File "..\bin64\register-voice.exe"		
		
		; Register 64-bit DLL
		!define LIBRARY_X64		
		!insertmacro InstallLib REGDLL NOTSHARED NOREBOOT_NOTPROTECTED "..\bin64\htstts.dll" "$INSTDIR\x64\htstts.dll" "$SYSDIR"
		!undef LIBRARY_X64
	${EndIf}		
			
	

SectionEnd

Section "Leopold (Austrian German)" Section1

	; Set Section properties
	SetOverwrite on

	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\"
	File "..\..\data\leo.htsvoice"
	File "..\..\data\leo.rules"
	
	; Register the new voice
	Exec '"$INSTDIR\x86\register-voice.exe" "$INSTDIR\leo.htsvoice" "Leopold" "de-at" "male" "adult" "$INSTDIR\x86\leo.log" "$INSTDIR\leo.rules"'
	
	${If} ${RunningX64}
		Exec '"$INSTDIR\x64\register-voice.exe" "$INSTDIR\leo.htsvoice" "Leopold" "de-at" "male" "adult" "$INSTDIR\x64\leo.log" "$INSTDIR\leo.rules"'
	${EndIf}		
	
SectionEnd

; ADD NEW VOICES HERE


; Modern install component descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${Section1} ""
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;Uninstall section
Section Uninstall

	; Unregister voices 
	Exec '"$INSTDIR\x86\register-voice.exe" "/u" "$INSTDIR\leo.htsvoice" "Leopold" "de-at" "male" "adult" "$INSTDIR\x86\leo.log" "$INSTDIR\leo.rules"'
	
	${If} ${RunningX64}
		Exec '"$INSTDIR\x64\register-voice.exe" "/u" "$INSTDIR\leo.htsvoice" "Leopold" "de-at" "male" "adult" "$INSTDIR\x64\leo.log" "$INSTDIR\leo.rules"'
	${EndIf}		
	
	; ADD NEW VOICE REMOVALS HERE


	;Remove from registry...
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
	DeleteRegKey HKLM "SOFTWARE\${APPNAME}"

	; Delete self
	Delete "$INSTDIR\uninstall.exe"

	; Delete Shortcuts
	Delete "$SMPROGRAMS\HTS SAPI Voices\Uninstall.lnk"
	
	; Unregister DLL
	!insertmacro UnInstallLib REGDLL NOTSHARED NOREBOOT_NOTPROTECTED "$INSTDIR\x86\htstts.dll"

	; Clean up HTS SAPI Voices
	Delete "$INSTDIR\x86\IEShims.dll"
	Delete "$INSTDIR\x86\atl110.dll"
	Delete "$INSTDIR\x86\msvcp110.dll"
	Delete "$INSTDIR\x86\msvcr110.dll"
	Delete "$INSTDIR\x86\register-voice.exe"
	
	RMDir "$INSTDIR\x86"
	
	; Install 64-bit voices only on 64-bit systems	
	${If} ${RunningX64}
		; UnRegister 64-bit DLL
		!define LIBRARY_X64		
		!insertmacro UnInstallLib REGDLL NOTSHARED NOREBOOT_NOTPROTECTED "$INSTDIR\x64\htstts.dll" 
		!undef LIBRARY_X64	
	
		Delete "$INSTDIR\x64\IEShims.dll"
		Delete "$INSTDIR\x64\atl110.dll"
		Delete "$INSTDIR\x64\msvcp110.dll"
		Delete "$INSTDIR\x64\msvcr110.dll"
		Delete "$INSTDIR\x64\register-voice.exe"		
		
		RMDir "$INSTDIR\x64"
	${EndIf}			
	
	Delete "$INSTDIR\*"		

	; Remove remaining directories
	RMDir "$SMPROGRAMS\HTS SAPI Voices"
	RMDir "$INSTDIR\"
	

SectionEnd

; On initialization
Function .onInit

	!insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

; eof