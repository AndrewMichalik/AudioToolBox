''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'   Voice Information Systems (1-800-234-VISI) AudTBx Professional Prompt Editor
'                   (c) Copyright Andrew J. Michalik 1987-1995
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Option Explicit

' Windows API functions
Declare Function GetModuleUsage Lib "Kernel" (ByVal hModule As Integer) As Integer
Declare Sub GetTempFileName Lib "Kernel" (ByVal cDriveLetter As Integer, ByVal lpPrefixString As String, ByVal wUnique As Integer, ByVal lpTempFileName As String)
Declare Function GetWindowsDirectory Lib "Kernel" (ByVal lpszSysPath As String, ByVal cbSysPath As Integer) As Integer
Declare Function GetPrivateProfileString Lib "Kernel" (ByVal lpApplicationName As String, ByVal lpKeyName As String, ByVal lpDefault As String, ByVal lpReturnedString As String, ByVal nSize As Integer, ByVal lpFileName As String) As Integer
Declare Function WritePrivateProfileString Lib "Kernel" (ByVal lpApplicationName As String, ByVal lpKeyName As String, ByVal lpString As String, ByVal lplFileName As String) As Integer
Declare Function GetSystemMetrics Lib "User" (ByVal iReqFlg As Integer) As Integer
Declare Function GetTickCount Lib "User" () As Long
Declare Function WinHelp Lib "User" (ByVal hWnd As Integer, ByVal lpHelpFile As String, ByVal wCommand As Integer, dwData As Any) As Integer
Declare Function WinExec Lib "Kernel" (ByVal lpCmdLin As String, ByVal nCmdShow As Integer) As Integer

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Constants
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' GetSystemMetrics
Global Const SM_CXMin = 28
Global Const SM_CYMin = 29

' Dialog box positioning
Global Const CTR_MIDMIDNON = 1

Function PrfGetDir () As String
    Dim sRetStr As String
    sRetStr = String(255, Chr$(STRNULDEL))

    If (0 <> GetWindowsDirectory(sRetStr, Len(sRetStr))) Then
         PrfGetDir = Trim$(StrTokStr(sRetStr, Chr$(STRNULDEL))) & "\"
    Else PrfGetDir = ""
    End If
End Function

Function PrfGetStr (ByVal sSecNam As String, ByVal sKeyNam As String, ByVal sDefVal As String)
    Dim sRetStr As String
    sRetStr = String(255, Chr$(STRNULDEL))

    If (0 <> GetPrivateProfileString(sSecNam, sKeyNam, sDefVal, sRetStr, Len(sRetStr), sPrfFilNam)) Then
         PrfGetStr = Trim$(StrTokStr(sRetStr, Chr$(STRNULDEL)))
    Else PrfGetStr = Trim$(sDefVal)
    End If
End Function

Sub PrfSetStr (ByVal sSecNam As String, ByVal sKeyNam As String, ByVal sDefVal As String)
    Dim iRetCod As Integer
    iRetCod = WritePrivateProfileString(sSecNam, sKeyNam, sDefVal, sPrfFilNam)
End Sub

Function SafFilExi (ByVal sFilNam As String, iFilAtr As Integer) As Integer
    On Error GoTo SafFilExiErr

    ' Test if file is safe for input; Initialize to bad
    SafFilExi = False

    ' Verify valid directory specification
    iFilAtr = GetAttr(sFilNam)
    If (-1 = iFilAtr) Then Exit Function

    ' Return success
    SafFilExi = True
    Exit Function

SafFilExiErr:
    On Error GoTo 0
    iFilAtr = -1
    Resume Next

End Function

Function SafGetDir (ByVal sDirStr As String) As String
    On Error GoTo SafGetDirErr
    Dim iFilAtr As Integer

    ' Accept null string as valid (.Exe Working) directory
    If ("" = sDirStr) Then sDirStr = sAppWrkDir
    
    ' If not "\" only then add or subtract "\"
    If (Len(sDirStr) > 1) Then
        ' Add full path and "\" if drive specification only
        If (Right(sDirStr, 1) = ":") Then
            ' Side effect of CurDir$(x) causes drive change; force back to original
            Dim sOrgDir As String
            sOrgDir = CurDir$
            sDirStr = CurDir$(sDirStr)
            ChDrive (sOrgDir)
        End If

        ' Strip "\" by default (any path other than drive root)
        If (Right(sDirStr, 1) = "\") Then sDirStr = Left(sDirStr, Len(sDirStr) - 1)
        
        ' Add full path and "\" if drive specification only
        If (Right(sDirStr, 1) = ":") Then sDirStr = sDirStr & "\"
    End If
         
    ' Verify valid directory specification
    iFilAtr = GetAttr(sDirStr) And ATTR_DIRECTORY
    
    ' If bad directory spec, reset to valid (.Exe Working) directory
    If (0 = iFilAtr) Then sDirStr = sAppWrkDir
    
    ' Add "\" if not already present
    If (Right(sDirStr, 1) <> "\") Then sDirStr = sDirStr & "\"
    SafGetDir = sDirStr
    Exit Function

SafGetDirErr:
    On Error GoTo 0
    Resume Next

End Function

Sub SafWndSta (fActFrm As Form, iWndSta)
    ' Set form and control focus
    On Error GoTo SafWndStaErr
    If (MAXIMIZED <> iWndSta) Then fActFrm.WindowState = iWndSta
    Exit Sub

SafWndStaErr:
    On Error GoTo 0
    Resume Next
End Sub

Function StrTokStr (sInpStr As String, ByVal sDelStr As String)
    ' Extract a token delimited string
    Dim iCurPos As Integer

    If ((sInpStr = Null) Or (sDelStr = Null) Or (0 = Len(sDelStr))) Then
        StrTokStr = sInpStr
        sInpStr = ""
        Exit Function
    End If

    iCurPos = InStr(sInpStr, sDelStr)
    If (0 = iCurPos) Then
        StrTokStr = sInpStr
        sInpStr = ""
        Exit Function
    End If

    StrTokStr = Left(sInpStr, iCurPos - 1)
    sInpStr = Right(sInpStr, Len(sInpStr) - iCurPos - Len(sDelStr) + 1)

End Function

Function CRLF () As String
    CRLF = Chr$(13) + Chr$(10)
End Function

Function DblRndAsc (ByVal dbSrcVal As Double, ByVal iDecCnt As Integer) As String
    Dim sSrcVal As String
    Dim sFmtStr As String
    Dim ii As Integer

    ' Convert a floating pointing value to a rounded ascii string
    If (0 = dbSrcVal) Then
        DblRndAsc = "0"
        Exit Function
    End If

    ' Round to required precision
    dbSrcVal = DblRndDbl(dbSrcVal, iDecCnt)

    ' Generate format string
    sFmtStr = "."
    For ii = 1 To iDecCnt
        sFmtStr = sFmtStr + "0"
    Next ii
    DblRndAsc = Format$(dbSrcVal, sFmtStr)

End Function

Function DblRndDbl (ByVal dbSrcVal As Double, ByVal iDecCnt As Integer) As Double
    Const Log_10 = 2.302585093              ' Natural log of 10
    Dim dbPrcFac As Double

    ' Compute precision factor
    dbPrcFac = Exp(iDecCnt * Log_10)

    ' Compute rounding value
    DblRndDbl = Int(((dbSrcVal * dbPrcFac) + .5)) / dbPrcFac

End Function

Function EdtDirBad (cEdtCtl As Control)
    ' Add "\" if not already present
    If ("" <> Trim$(cEdtCtl.Text)) And (Right(cEdtCtl.Text, 1) <> "\") Then cEdtCtl.Text = cEdtCtl.Text & "\"
    
    ' Default to directory OK, exit if test succeeds
    EdtDirBad = False
    If (("" = Trim$(cEdtCtl.Text)) Or (cEdtCtl.Text = SafGetDir(cEdtCtl.Text))) Then Exit Function

    ' Highlight and beep
    cEdtCtl.SetFocus
    cEdtCtl.SelStart = 0
    cEdtCtl.SelLength = Len(cEdtCtl)
    EdtDirBad = True
    Beep
End Function

Sub FrmCtrFoc (frmParFrm As Form, frmChdFrm As Form, iCtrFlg As Integer)
    If TypeOf frmParFrm Is frmAudTBx Then
        ' Center a child form over a parent form.
        FrmCtrMov frmChdFrm, frmParFrm.Left, frmParFrm.Width, frmParFrm.Top, frmParFrm.Height
    Else
        ' Center after Adding in parent position
        FrmCtrMov frmChdFrm, frmAudTBx.Left + frmParFrm.Left, frmParFrm.Width, frmAudTBx.Top + frmParFrm.Top, frmParFrm.Height
    End If

    ' Position mouse of control with focus
    If (0 <> iCtrFlg) Then
    End If

End Sub

Sub FrmCtrMov (frmChdFrm As Form, ByVal iParLft As Integer, ByVal iParWid As Integer, ByVal iParTop As Integer, ByVal iParHgt As Integer)
    Dim iLftPos As Integer
    Dim iTopPos As Integer

    ' Get left offset
    iLftPos = iParLft + ((iParWid - frmChdFrm.Width) / 2)
    If (iLftPos + frmChdFrm.Width > screen.Width) Then iLftPos = screen.Width - frmChdFrm.Width
    If (iLftPos < 0) Then iLftPos = 0

    ' Get top offset
    iTopPos = iParTop + ((iParHgt - frmChdFrm.Height) / 2)
    If (iTopPos + frmChdFrm.Height > screen.Height) Then iTopPos = screen.Height - frmChdFrm.Height
    If (iTopPos < 0) Then iTopPos = 0

    ' Center the child form
    frmChdFrm.Move iLftPos, iTopPos

End Sub

Sub FrmModRet (fUnlFrm As Form, ByVal viRetCod As Variant)
    ' This is a modular input / output handler for modal forms
    ' Note that form is hidden, not unloaded. This prevents a possible
    ' loop if this routine is called by the form's unload event
    fUnlFrm.Tag = viRetCod
    fUnlFrm.Hide
End Sub

Function FrmModShw (frmParent As Form, fShwFrm As Form, ByVal vInpVal As Variant, ByVal iCtrFlg As Integer) As Variant
    ' This is a modular input / output handler for modal forms
    ' Note: One quirk is that the tag field is not
    '       available in the Form_Load event

    ' First set form "input"
    fShwFrm.Tag = vInpVal
    
    ' Center and show
    FrmCtrFoc frmParent, fShwFrm, iCtrFlg
    fShwFrm.Show modal
    
    ' Return response
    FrmModShw = fShwFrm.Tag
    Unload fShwFrm

End Function

Function KeyChkEsc (KeyAscii As Integer) As Integer
    If (KEY_ESCAPE = KeyAscii) Then
        KeyAscii = 0
        KeyChkEsc = True
        Exit Function
    End If
    KeyChkEsc = False
End Function

Function KeyChkRet (KeyAscii As Integer) As Integer
    If (KEY_RETURN = KeyAscii) Then
        KeyAscii = 0
        KeyChkRet = True
        Exit Function
    End If
    KeyChkRet = False
End Function

Function Max (ByVal vVal1 As Variant, ByVal vVal2 As Variant) As Variant
    Max = vVal1
    If (Val(vVal2) > Val(vVal1)) Then Max = vVal2
End Function

Function Min (ByVal vVal1 As Variant, ByVal vVal2 As Variant) As Variant
    Min = vVal1
    If (Val(vVal2) < Val(vVal1)) Then Min = vVal2
End Function

Sub PrfCtlGet (cCtl As Control)
  On Error GoTo PrfCtlGetErr
  Dim iCtl_OK As Integer
  Dim iCtlEna As Integer

  ' Get the .INI file value for the specified control
  Dim sCtlTag As String
  Dim sCtlSec As String
  Dim sCtlKey As String

  ' Skip if not approved type
  ' Note: "Is TypOf" cannot be nested
  iCtl_OK = False
  If TypeOf cCtl Is TextBox Then iCtl_OK = True
  If TypeOf cCtl Is ListBox Then iCtl_OK = True
  If TypeOf cCtl Is CheckBox Then iCtl_OK = True
  If TypeOf cCtl Is ComboBox Then iCtl_OK = True
  If (Not iCtl_OK) Then Exit Sub
  
  ' Set default background and foreground color
  If (cCtl.ForeColor = cCtl.BackColor) Then cCtl.ForeColor = Not cCtl.BackColor
  cCtl.BackColor = PrfGetStr(PRF_ENVOPTSEC, PRF_BAKCLRKEY, cCtl.BackColor)
  cCtl.ForeColor = PrfGetStr(PRF_ENVOPTSEC, PRF_FORCLRKEY, cCtl.ForeColor)

  ' Set default font name, size, color
  cCtl.FontName = PrfGetStr(PRF_ENVOPTSEC, PRF_FNTNAMKEY, cCtl.FontName)
  cCtl.FontSize = PrfGetStr(PRF_ENVOPTSEC, PRF_FNTSIZKEY, cCtl.FontSize)
  
  ' Retrieve .INI section and key
  sCtlTag = cCtl.Tag
  sCtlSec = Trim(StrTokStr(sCtlTag, Chr$(TBXARGDEL)))
  sCtlKey = Trim(StrTokStr(sCtlTag, Chr$(TBXARGDEL)))
  If (("" = Trim(sCtlSec)) Or ("" = Trim(sCtlKey))) Then Exit Sub
  
  ' Note: the drop down combobox appears to have a bug where the text is selected
  ' after setting the font name or size
  If TypeOf cCtl Is ComboBox Then If cCtl.Style = 0 Then cCtl.SelText = 0
  
  ' Read .INI setting
  If TypeOf cCtl Is TextBox Then cCtl = PrfGetStr(sCtlSec, sCtlKey, cCtl)
  If TypeOf cCtl Is CheckBox Then cCtl = PrfGetStr(sCtlSec, sCtlKey, cCtl)
  If TypeOf cCtl Is ComboBox Then cCtl.ListIndex = Val(PrfGetStr(sCtlSec, sCtlKey, "0"))
  Exit Sub

PrfCtlGetErr:
  Resume Next

End Sub

Sub PrfCtlSet (cCtl As Control)
  ' Set the .INI file value for the specified control
  Dim sCtlTag As String
  Dim sCtlSec As String
  Dim sCtlKey As String

  ' Skip if menu or label
  If TypeOf cCtl Is Menu Then Exit Sub
  If TypeOf cCtl Is Label Then Exit Sub
  
  ' Retrieve .INI section and key
  sCtlTag = cCtl.Tag
  sCtlSec = Trim(StrTokStr(sCtlTag, Chr$(TBXARGDEL)))
  sCtlKey = Trim(StrTokStr(sCtlTag, Chr$(TBXARGDEL)))
  If (("" = Trim(sCtlSec)) Or ("" = Trim(sCtlKey))) Then Exit Sub
  
  ' Write .INI setting
  If TypeOf cCtl Is TextBox Then PrfSetStr sCtlSec, sCtlKey, cCtl
  If TypeOf cCtl Is CheckBox Then PrfSetStr sCtlSec, sCtlKey, cCtl
  If TypeOf cCtl Is ComboBox Then PrfSetStr sCtlSec, sCtlKey, cCtl.ListIndex

End Sub

Sub Wait (lWaiTim As Long)
    Dim lCurTim As Long
  
    ' Wait a specified number of milliseconds
    lCurTim = GetTickCount()
    Do While (GetTickCount() < lCurTim + lWaiTim)
        DoEvents
    Loop

End Sub

