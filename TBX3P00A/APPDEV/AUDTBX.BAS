''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'   Voice Information Systems (1-800-234-VISI) AudTBx Professional Prompt Editor
'                   (c) Copyright Andrew J. Michalik 1987-1995
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Option Explicit

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Installation Library functions
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Declare Function InsDLLIni Lib "tbxins.dll" (ByVal usReqCod As Integer, ByVal ulPrm001 As Long, ByVal ulPrm002 As String) As Integer
Declare Function ChkRegSer Lib "tbxins.dll" (ByVal usRsv001 As Integer, ByVal szSerNum As String) As Integer

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Prototype declarations
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Declare Function GetModuleHandle Lib "Kernel" (ByVal lpModuleName As String) As Integer
Declare Function GetModuleFileName Lib "Kernel" (ByVal hModule As Integer, ByVal lpFilename As String, ByVal nSize As Integer) As Integer

Declare Function GetFileVersionInfoSize Lib "ver.dll" (ByVal lpszFileName As String, lpdwHandle As Long) As Long
Declare Function GetFileVersionInfo Lib "ver.dll" (ByVal lpszFileName As String, ByVal lpdwHandle As Long, ByVal cbbuf As Long, ByVal lpvdata As String) As Integer
Declare Function VerQueryValue Lib "ver.dll" (ByVal lpvBlock As String, ByVal lpszSubBlock As String, lplpBuffer As Long, lpcb As Integer) As Integer
Declare Function lstrcpyn Lib "Kernel" (ByVal lpszString1 As Any, ByVal lpszString2 As Long, ByVal cChars As Integer) As Long

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Global variables shared by all modules
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Global sPrfFilNam       As String       ' Default .INI File name
Global sAppWrkDir       As String       ' Default application working director
Global lLblEnaClr       As Long         ' Option label enable color - eliminate in future release?
Global lLblDisClr       As Long         ' Option label disable color - eliminate in future release?
Global lValEnaClr       As Long         ' Option value enable color
Global lValDisClr       As Long         ' Option value disable color

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Tab Numbers
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Global Const TABPCMCVT = 0              ' PCM Convert
Global Const TABSNDCHP = 1              ' Sound chop
Global Const TABIDXCHP = 2              ' Index chop
Global Const TABIDXLST = 3              ' Index list
Global Const TABIDXPAK = 4              ' Index pack
Global Const TABIDXREB = 5              ' Index rebuild

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Context sensitive help
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Global Const HLP_AUDTBX = 2             ' Audio ToolBox
Global Const HLP_ABTVIS = 4             ' About VISI
Global Const HLP_TCHSUP = 6             ' Technical Support
Global Const HLP_OTHPRD = 8             ' Other products
Global Const HLP_TABINC = 10            ' Tab help increment
Global Const HLP_TABBAS = 20            ' Tab help base number
Global Const HLP_FTRCFG = 22            ' Filter configuration
Global Const HLP_CVTADV = 24            ' Conversion advanced options
Global Const HLP_CVTEQU = 26            ' Conversion frequency equalizer
Global Const HLP_ENVOPT = 100           ' Environment options
Global Const HLP_MODVER = 200           ' Module verification

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Constants
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Default values
Global Const STRNULDEL = 0              ' Null
Global Const TBXARGDEL = 44             ' Comma - argument delimeter
Global Const WRKDIRDEF = ""             ' Environment temporary directory default
Global Const CURDIRTXT = "Current directory is "
Global Const OPTNONDEF = "none"         ' Option none
Global Const OPTDEFDEF = "default"      ' Option none
Global Const OPTALLDEF = "all"          ' Option all
Global Const OPTVOXDEF = ".vox"         ' Option vox extension
Global Const OPTTXTDEF = ".txt"         ' Option txt extension
Global Const LBLENADEF = &H80000008     ' Option label enable color - eliminate in future release?
Global Const LBLDISDEF = &H80000008     ' Option label disable color - eliminate in future release?
Global Const VALENADEF = &H80000008     ' Option value enable color
Global Const VALDISDEF = &H808080       ' Option value disable color
'Global Const VALDISDEF = &H8000&        ' Option value disable color

' Message strings
Global Const APPINIERR = "Error during initialization, continue?"
Global Const MODVERVER = ", Version "   ' Module version
Global Const MODVERUNK = "Unknown"      ' Module version unknown
Global Const MODVERNOL = " (not loaded)"' Module not loaded

' File open dialog
Global Const FILOPNAUD = "Audio File"   ' File open audio dialog
Global Const FILOPNBAT = "Batch File"   ' File open batch dialog

' Manufacturer version
Global Const MFGVERUNK = "Unknown"      ' Unknown
Global Const MFGVERDLG = "Dialogic"     ' Dialogic
Global Const MFGVERG21 = "G.721 CCITT"  ' CCITT
Global Const MFGVERNWV = "NewVoice"     ' NewVoice
Global Const MFGVERPTC = "Perception"   ' Perception Technology
Global Const MFGVERRTX = "Rhetorex"     ' Rhetorex
Global Const MFGVERMPC = "Multimedia"   ' Multimedia

' Indexed file default values
Global Const IDXFRQDEF = 6000           ' Index frequency default
Global Const IDXPOSDEF = 1              ' Index at position default

' Conversion defaults
Global Const SNDTHRDEF = 2              ' Chop sound threshold default
Global Const CRPGRDDEF = .05            ' Crop guard time default
Global Const VOLNRMDEF = .2             ' Volume normalize % over range
Global Const CMPEXPDEF = 6              ' Dynamic Compressor / Expander default

' Anti-aliasing filter types
Global Const AAFALGOFF = "Anti-alias off"
Global Const AAFALGFST = "Anti-alias 2nd order integer (fast)"
Global Const AAFALGNRM = "Anti-alias 2nd order float (normal)"
Global Const AAFALGQ6I = "Anti-alias 6th order integer (better)"
Global Const AAFALGQ6F = "Anti-alias 6th order float (best)"
Global Const AAFALGFFT = "Anti-alias FFT FIR (best)"

' Resample algorithm types
Global Const RESALGNRM = "Resample linear (normal)"
Global Const RESALGFFT = "Resample FFT (best)"

' Signal filter types
Global Const DTMNOTFTR = "DTMF notch filter"

' Byte swap types
Global Const CVTSWPBIT = "bit"
Global Const CVTSWPNIB = "nibble"
Global Const CVTSWPBYT = "byte"
Global Const CVTSWPWRD = "word"

' Environment Options
Global Const EXEMODNRM = "Normal"           ' Execution mode Normal
Global Const EXEMODMIN = "Minimized"        ' Execution mode Iconic
Global Const EXEMODHID = "Hidden"           ' Execution mode hidden
Global Const EXIMODNRM = "Normal"           ' Exit Mode: Always exit
Global Const EXIALLEXI = 0                  ' Exit Mode: Always exit
Global Const EXIMODERR = "Pause on error"   ' Exit Mode: Succ & warn exit, err pause
Global Const EXIERRPAU = 1                  ' Exit Mode: Succ & warn exit, err pause
Global Const EXIMODWRN = "Pause on warning" ' Exit Mode: Succ exit, err & warn pause"
Global Const EXIWRNPAU = 2                  ' Exit Mode: Succ exit, err & warn pause"
Global Const EXIMODALL = "Pause always"     ' Exit Mode: Always pause
Global Const EXIALLPAU = 3                  ' Exit Mode: Always exit


' Profile section and key strings
Global Const PRF_PRFNAMDEF = "AUDTBX.INI"
Global Const PRF_ENVOPTSEC = "Environment Options"
Global Const PRF_FORCLRKEY = "ForeColor"
Global Const PRF_BAKCLRKEY = "BackColor"
Global Const PRF_FNTNAMKEY = "FontName"
Global Const PRF_FNTSIZKEY = "FontSize"
Global Const PRF_LBLENAKEY = "LabelEnabledColor"
Global Const PRF_LBLDISKEY = "LabelDisabledColor"
Global Const PRF_VALENAKEY = "ValueEnabledColor"
Global Const PRF_VALDISKEY = "ValueDisabledColor"

Global Const PRF_SCRPOSSEC = "Screen Position"
Global Const PRF_SCRSTAKEY = "WindowState"
Global Const PRF_SCRLFTKEY = "PositionLeft"
Global Const PRF_SCRTOPKEY = "PositionTop"
Global Const PRF_TABPOSSEC = "Tab Position"
Global Const PRF_MFGVERKEY = "Edition"
Global Const PRF_MFGVERSEC = "Manufacturer"
Global Const PRF_ACTFLDKEY = "Active Folder"
Global Const PRF_FTRCFGSEC = "Filter Configure"
Global Const PRF_CFGAAFKEY = "Anti-aliasing"
Global Const PRF_CFGRESKEY = "Resample"
Global Const PRF_EFFCFGSEC = "Audio Conversion"
Global Const PRF_EQUBNDKEY = "EqualizerBand"

' Graphic Equalizer/ Analyzer constants
Global Const EQUBNDCNT = 8              ' Number of equalizer bands
Global Const EQUSLDRNG = 24             ' Range of equalizer slider control
Global Const EQUGAIDEF = 0              ' Gain (in dB)
Global Const EQUFFTORD = 8              ' Equalizer FFT order (Ord^2 = # of frequency points)
Global Const ANATIMDEF = 32             ' Number of time points in equalizer graph
Global Const ANAFFTORD = 4              ' Analyzer FFT order (Ord^2 = # of frequency points)

' Registration Info
Global Const PRF_REGLICFIL = "AudTBx.Lic"
' Global Const PRF_REGLICSEC = "ToolBox Licensee"
Global Const PRF_REGLICSEC = "Apprentice Licensee"
Global Const PRF_REGUSRKEY = "User"
Global Const PRF_REGCMPKEY = "Company"
Global Const PRF_REGSERKEY = "SerialNumber"

' Module version numbers
Global Const PRF_MODUSESEC = "Modules"

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Globals types shared by all modules
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Type TBXDOCTYP
    sPCMDes As String
    sPCMTyp As String
End Type

Global ttDocTypInp() As TBXDOCTYP
Global ttDocTypOut() As TBXDOCTYP

Sub ErrMsgExi (sErrMsg As String)
    If ("" <> sErrMsg) Then MsgBox sErrMsg, MB_ICONEXCLAMATION + MB_OK, frmAudTBx.Caption
    Unload frmAudTBx
End Sub

Function GetFileVersion (FileToCheck As String) As String
    Dim lpdwHandle As Long
    Dim VersionInfoSize As Long
    Dim lpvdata As String
    Dim VersionInfo As Integer
    Dim lplpBuffer As Long
    Dim lpcb As Integer
    Dim ptrFixed As Integer
    Dim TransString As String
    Dim ptrString As Integer
    Dim fixedstr As String
    Dim stringcopy As Long
    Dim i As Integer
    Dim char As String
    Dim nextchar As String
    Dim TransValue As Long
    Dim TransTable As String

    On Error Resume Next

    VersionInfoSize& = GetFileVersionInfoSize(FileToCheck, lpdwHandle&)
    
    If VersionInfoSize& = 0 Then
        GetFileVersion = MODVERUNK
        Exit Function
    End If
    
    lpvdata$ = String(VersionInfoSize&, Chr$(STRNULDEL))
    VersionInfo% = GetFileVersionInfo(FileToCheck, lpdwHandle&, VersionInfoSize&, lpvdata$)
    ptrFixed% = VerQueryValue(lpvdata$, "\FILEVERSION", lplpBuffer&, lpcb%)
    
    If ptrFixed% = 0 Then
        ' Take a shot with the hardcoded TransString
        TransString$ = "040904E4"
        ptrString% = VerQueryValue(lpvdata$, "\StringFileInfo\" & TransString$ & "\CompanyName", lplpBuffer&, lpcb%)
        If ptrString% <> 0 Then GoTo GetValues
        ptrFixed% = VerQueryValue(lpvdata$, "\", lplpBuffer&, lpcb%)
        If ptrFixed% = 0 Then
            GetFileVersion = MODVERUNK
            Exit Function
        Else
            TransString$ = ""
            fixedstr$ = String(lpcb% + 1, Chr(0))
            stringcopy& = lstrcpyn(fixedstr$, lplpBuffer&, lpcb% + 1)
            For i = lpcb% To 1 Step -1
                char$ = Hex(Asc(Mid(fixedstr$, i, 1)))
                If Len(char$) = 1 Then
                    char$ = "0" + char$
                End If
                TransString$ = TransString$ + char$
                If Len(TransString$ & nextchar$) Mod 8 = 0 Then
                    TransString$ = "&H" & TransString$
                    TransValue& = Val(TransString$)
                    TransString$ = ""
                End If
            Next i
        End If
    End If
    
    TransTable$ = String(lpcb% + 1, Chr(0))
    TransString$ = String(0, Chr(0))
    stringcopy& = lstrcpyn(TransTable$, lplpBuffer&, lpcb% + 1)
    
    For i = 1 To lpcb%
        char$ = Hex(Asc(Mid(TransTable$, i, 1)))
        If Len(char$) = 1 Then
            char$ = "0" + char$
        End If
        
        If Len(TransString$ & nextchar$) Mod 4 = 0 Then
            nextchar$ = char$
        Else
            TransString$ = TransString$ + char$ + nextchar$
            nextchar$ = ""
            char$ = ""
        End If
    Next i

GetValues:
    ptrString% = VerQueryValue(lpvdata$, "\StringFileInfo\" & TransString$ & "\FileVersion", lplpBuffer&, lpcb%)
    
    If ptrString% = 1 Then
        TransTable$ = String(lpcb%, Chr(0))
        stringcopy& = lstrcpyn(TransTable$, lplpBuffer&, lpcb% + 1)
        GetFileVersion = StrTokStr(TransTable$, Chr$(STRNULDEL))    ' Eliminate null char
    
    Else
        GetFileVersion = MODVERUNK
    End If
End Function

Function GetLoadedModule (sModFilNam As String) As String
  Dim hInst%, sModPthNam$

  ' Indicate "not loaded" as default
  GetLoadedModule = sModFilNam & MODVERNOL
  
  ' get instance handle of module
  hInst = GetModuleHandle(sModFilNam)

  If (hInst > 0) Then
    sModPthNam = Space$(256)
    If (GetModuleFileName(hInst, sModPthNam, Len(sModPthNam)) > 0) Then GetLoadedModule = Trim$(StrTokStr(sModPthNam, Chr$(STRNULDEL)))
  End If
End Function

Sub HelpSearch ()
    Dim i%
    i = WinHelp(frmAudTBx.hWnd, app.HelpFile, HELP_PARTIALKEY, ByVal "")
End Sub

Function QryFilNam (ByVal sFilNam As String, sDlgCap As String) As String
  On Error GoTo QryFilNam_err
  
  Dim Canceled As Integer
  Dim iErrCnt As Integer

  Canceled = False
  
  ' Convert to lower and compare extensions
  sFilNam = LCase(sFilNam)
  If (sFilNam Like "*.bat") Then
    frmAudTBx.dlgCom.Filter = "Batch Files (*.bat)|*.bat|All Files (*.*)|*.*"
  ElseIf (sFilNam Like "*.txt") Then
    frmAudTBx.dlgCom.Filter = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*"
  ElseIf (sFilNam Like "*.vap") Then
    frmAudTBx.dlgCom.Filter = "Indexed Files (*.vap)|*.vap|All Files (*.*)|*.*"
  ElseIf (sFilNam Like "*.vox") Then
    frmAudTBx.dlgCom.Filter = "Voice Files (*.vox)|*.vox|All Files (*.*)|*.*"
  ElseIf (sFilNam Like "*.wav") Then
    frmAudTBx.dlgCom.Filter = "Wave Files (*.wav)|*.wav|All Files (*.*)|*.*"
  Else
    frmAudTBx.dlgCom.Filter = "All Files (*.*)|*.*"
  End If


'  Select Case LCase(sFilExt)
'    Case "*.bat"
'      frmAudTBx.dlgCom.Filter = "Batch Files (*.bat)|*.bat|All Files (*.*)|*.*"
'
'    Case "*.txt"
'      frmAudTBx.dlgCom.Filter = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*"
'
'    Case "*.vap"
'      frmAudTBx.dlgCom.Filter = "Index Files (*.vap)|*.vap|All Files (*.*)|*.*"
'
'    Case "*.vox"
'      frmAudTBx.dlgCom.Filter = "Voice Files (*.vox)|*.vox|All Files (*.*)|*.*"
'
'    Case "*.wav"
'      frmAudTBx.dlgCom.Filter = "Wave Files (*.wav)|*.wav|All Files (*.*)|*.*"
'
'    Case Else
'      frmAudTBx.dlgCom.Filter = "All Files (*.*)|*.*"
'  End Select

  frmAudTBx.dlgCom.FilterIndex = 0
  frmAudTBx.dlgCom.DefaultExt = sFilNam
  frmAudTBx.dlgCom.Filename = sFilNam
  frmAudTBx.dlgCom.CancelError = True
  frmAudTBx.dlgCom.Flags = OFN_HIDEREADONLY + OFN_SHAREAWARE
  frmAudTBx.dlgCom.DialogTitle = sDlgCap

  frmAudTBx.dlgCom.Action = 1
  
  If Not Canceled Then QryFilNam = LCase(frmAudTBx.dlgCom.Filename)

  Exit Function

QryFilNam_err:
  ' If problem is invalid file name, retry once
  If ((0 = iErrCnt) And (Err = FNERR_INVALIDFILENAME)) Then
    frmAudTBx.dlgCom.Filename = ""
    iErrCnt = 1
    Resume
  End If
  
  ' Set error condition to canceled
  Canceled = (Err = CDERR_CANCEL)
  
  Resume Next
End Function

Sub VerifyModule (sModFulNam$)
  ' Compare the versions of modules in memory to those installed on disk
  Dim hInst As Integer
  Dim sModFilNam As String
  Dim sModPthNam As String
  Dim sVerLod As String
  Dim sVerExp As String
  Dim sVerSav As String

  ' extract module name w/o path
  sModFilNam = sModFulNam

  Do While InStr(sModFilNam, "\") > 0
    sModFilNam = Mid$(sModFilNam, InStr(sModFilNam, "\") + 1)
  Loop

  sModPthNam = GetLoadedModule(sModFilNam)
  sVerLod = GetFileVersion(sModPthNam)   ' loaded
  sVerExp = GetFileVersion(sModFulNam)   ' expected
  sVerSav = PrfGetStr(PRF_MODUSESEC, sModFilNam, "")
  'Debug.Print sModFilNam; ": Exp="; sVerExp; ", loaded="; sVerLod; ", saved="; sVerSav
  
  If (sVerLod <> sVerExp) And (sVerSav <> sVerLod) Then
      FrmCtrMov frmModVer, 0, Screen.Width, 0, Screen.Height

      frmModVer.txtModFnd = LCase(sModPthNam) & MODVERVER & sVerLod  ' loaded module
      frmModVer.txtModExp = LCase(sModFulNam) & MODVERVER & sVerExp  ' expected module

      frmModVer.Show MODAL

      If frmModVer.chkSaveInfo Then PrfSetStr PRF_MODUSESEC, sModFilNam, sVerLod
    
      Unload frmModVer
    
      ' Yield to complete re-paints
      DoEvents
  End If

End Sub

