Option Explicit

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Windows functions
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Declare Function GetWindowsDirectory Lib "Kernel" (ByVal lpszSysPath As String, ByVal cbSysPath As Integer) As Integer
Declare Function GetPrivateProfileString Lib "Kernel" (ByVal lpApplicationName As String, ByVal lpKeyName As String, ByVal lpDefault As String, ByVal lpReturnedString As String, ByVal nSize As Integer, ByVal lpFilename As String) As Integer
Declare Function WritePrivateProfileString Lib "Kernel" (ByVal lpApplicationName As String, ByVal lpKeyName As String, ByVal lpString As String, ByVal lplFileName As String) As Integer

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Global variables shared by all modules
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Global sPrfFilNam       As String       ' Default .INI File name
Global sLogFilNam       As String       ' Default .LOG File name
Global sAppWrkDir       As String       ' Default application working directory

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Global constants shared by all modules
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Global Const STRNULDEL = 0              ' Null

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Profile section and key strings
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Global Const PRF_PRFNAMDEF = "LICAPP.INI"
Global Const PRF_CUSREGSEC = "CustomerRegistration"
Global Const PRF_CUSNAMKEY = "CustomerName"
Global Const PRF_CMPNAMKEY = "CompanyName"
Global Const PRF_SERNUMKEY = "SerialNumber"
Global Const PRF_PRDNAMKEY = "ProductNameIndex"

Global Const SI_REGSEC = "Licensee"
Global Const SI_USRKEY = "User="
Global Const SI_CMPKEY = "Company="
Global Const SI_SERKEY = "SerialNumber="
Global Const SI_INSCHR = "InstallationCharacteristics="

Global Const LOG_LOGNAMDEF = "LICAPP.LOG"


Function CRLF () As String
    CRLF = Chr$(13) + Chr$(10)
End Function

Function PrfGetStr (ByVal sSecNam As String, ByVal sKeyNam As String, ByVal sDefVal As String)
    Dim sRetStr As String
    sRetStr = String(255, Chr$(STRNULDEL))

    If (0 <> GetPrivateProfileString(sSecNam, sKeyNam, sDefVal, sRetStr, Len(sRetStr), sPrfFilNam)) Then
      PrfGetStr = Trim$(StrTokStr(sRetStr, Chr$(STRNULDEL)))
    Else
      PrfGetStr = Trim$(sDefVal)
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

