VERSION 2.00
Begin Form frmLicApp 
   BackColor       =   &H00FFFFFF&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "License App"
   ClientHeight    =   5328
   ClientLeft      =   1764
   ClientTop       =   1596
   ClientWidth     =   5196
   Height          =   5700
   Icon            =   LIC_APP.FRX:0000
   Left            =   1740
   LinkTopic       =   "Form1"
   ScaleHeight     =   5328
   ScaleWidth      =   5196
   Top             =   1248
   Width           =   5244
   Begin Frame frmTitle 
      BackColor       =   &H00FFFFFF&
      ForeColor       =   &H00FFFFFF&
      Height          =   5052
      Left            =   120
      TabIndex        =   10
      Top             =   120
      Width           =   4932
      Begin ComboBox cboProduct 
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "Times New Roman"
         FontSize        =   13.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   420
         Left            =   240
         Sorted          =   -1  'True
         TabIndex        =   4
         Top             =   3600
         Width           =   4452
      End
      Begin TextBox edtCustomer_Name 
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "Times New Roman"
         FontSize        =   13.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   468
         Left            =   240
         TabIndex        =   0
         Top             =   1080
         Width           =   4452
      End
      Begin CommandButton btnOK 
         Caption         =   "&OK"
         Height          =   372
         Left            =   840
         TabIndex        =   5
         Top             =   4320
         Width           =   1092
      End
      Begin CommandButton btnExit 
         Caption         =   "E&xit"
         Height          =   372
         Left            =   2880
         TabIndex        =   6
         Top             =   4320
         Width           =   1092
      End
      Begin TextBox edtCompany_Name 
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "Times New Roman"
         FontSize        =   13.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   468
         Left            =   240
         TabIndex        =   2
         Top             =   1920
         Width           =   4452
      End
      Begin TextBox edtSerial_Number 
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "Times New Roman"
         FontSize        =   13.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   468
         Left            =   240
         TabIndex        =   3
         Top             =   2760
         Width           =   4452
      End
      Begin Label txtProduct 
         Caption         =   "Product Name"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   240
         TabIndex        =   11
         Top             =   3360
         Width           =   1332
      End
      Begin Label txtCustomer 
         Caption         =   "Customer Name"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   240
         TabIndex        =   1
         Top             =   840
         Width           =   1932
      End
      Begin Label txtCompany 
         Caption         =   "Company Name"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   240
         TabIndex        =   7
         Top             =   1680
         Width           =   1692
      End
      Begin Label txtSerial 
         Caption         =   "Serial Number"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   240
         TabIndex        =   8
         Top             =   2520
         Width           =   1692
      End
      Begin Label txtTitle 
         Alignment       =   2  'Center
         Caption         =   "Customer Registration Form"
         FontBold        =   0   'False
         FontItalic      =   -1  'True
         FontName        =   "Monotype Corsiva"
         FontSize        =   19.2
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   492
         Left            =   120
         TabIndex        =   9
         Top             =   240
         Width           =   4692
      End
   End
End
Option Explicit

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Installation Library functions
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Declare Function InsDLLIni Lib "tbxins.dll" (ByVal usReqCod As Integer, ByVal ulPrm001 As Long, ByVal ulPrm002 As String) As Integer
Declare Function ChkRegSer Lib "tbxins.dll" (ByVal usRsv001 As Integer, ByVal szSerNum As String) As Integer
Declare Function ChkRegIns Lib "tbxins.dll" (ByVal usReqCod As Integer, ByVal ulPrm001 As Long, ByVal szInsStr As String) As Long

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Dim Name_Array(5)  As String

Sub btnExit_Click ()
    End
End Sub

Sub btnOK_Click ()
    On Error GoTo btnOK_ClickError
    
    Dim sFilNam, iFilNum, sWriteRegInfo, lInsChr   ' Declare variables.
    
    ' Check installation characteristics for valid data.
    lInsChr = ChkInsChr()
    If (0 = lInsChr) Then Exit Sub
    
    ' Create registration information strings.
    sWriteRegInfo = "[" & cboProduct & " " & SI_REGSEC & "]" & CRLF()
    sWriteRegInfo = sWriteRegInfo & SI_USRKEY & edtCustomer_Name & CRLF()
    sWriteRegInfo = sWriteRegInfo & SI_CMPKEY & edtCompany_Name & CRLF()
    sWriteRegInfo = sWriteRegInfo & SI_SERKEY & edtSerial_Number & CRLF()
    sWriteRegInfo = sWriteRegInfo & SI_INSCHR & Trim$(Str(lInsChr))
    
    ' Get file name and delete original license file.
    sFilNam = "A:" & Name_Array(cboProduct.ItemData(cboProduct.ListIndex))
    If SafFilExi(sFilNam, 0) Then Kill sFilNam
    
    ' Write registration info to license file.
    iFilNum = FreeFile                              ' Determine file number.
    Open sFilNam For Output As iFilNum              ' Open file.
    Print #iFilNum, sWriteRegInfo                   ' Write reg info to file.
    Close iFilNum                                   ' Close all files.
    
    ' Append registration info to log file
    iFilNum = FreeFile                              ' Determine file number.
    Open sLogFilNam For Append As iFilNum           ' Open file.
    Print #iFilNum, Date$ & CRLF() & sWriteRegInfo & CRLF() ' Write reg info to file.
    Close iFilNum                                   ' Close all files.
    
    ' Display registration information
    MsgBox sWriteRegInfo, 0, "Registration Information"
    
    ' Update default product for next time
    ' Load previous customer data
    PrfSetStr PRF_CUSREGSEC, PRF_CUSNAMKEY, edtCustomer_Name
    PrfSetStr PRF_CUSREGSEC, PRF_CMPNAMKEY, edtCompany_Name
    PrfSetStr PRF_CUSREGSEC, PRF_SERNUMKEY, edtSerial_Number
    PrfSetStr PRF_CUSREGSEC, PRF_PRDNAMKEY, Str(cboProduct.ListIndex)
    Exit Sub

btnOK_ClickError:
    MsgBox ("Error trying to write file; Check diskette!")
    Exit Sub

End Sub

Function ChkInsChr () As Long
    Dim lInsChr As Long

    ' Calculate installation characteristics checksum
    lInsChr = 0
    lInsChr = ChkRegIns(0, lInsChr, edtCustomer_Name)
    lInsChr = ChkRegIns(0, lInsChr, edtCompany_Name)
    lInsChr = ChkRegIns(0, lInsChr, edtSerial_Number)
    If (0 = lInsChr) Then
	MsgBox "Invalid registration information"
	Exit Function
    End If
    
    ' Set license information
    If (0 = ChkRegSer(0, edtSerial_Number) Or ("" = edtSerial_Number)) Then
	MsgBox "Invalid Serial Number"
	Exit Function
    End If
    
    ' Return success value
    ChkInsChr = lInsChr

End Function

Sub Form_Load ()
    Dim iRetCod As Integer

    ' Save application working directory spec (may be different from executable directory)
    sAppWrkDir = CurDir$
    
    ' Set working directory to directory containing the application.
    ' This will insure that DLL's can be found (if not in path)
    ChDrive App.Path
    ChDir App.Path

    ' Initialize name of .INI file
    If ("" = sPrfFilNam) Then sPrfFilNam = PrfGetDir() & PRF_PRFNAMDEF

    ' Initialize name of .LOG file; Append full path name
    sLogFilNam = App.Path
    If Right$(sLogFilNam, 1) <> "\" Then sLogFilNam = sLogFilNam + "\"
    sLogFilNam = sLogFilNam & LOG_LOGNAMDEF

    ' Initialize Installation DLL support
    iRetCod = InsDLLIni(0, 0&, "")
  
    ' Fill cboProduct and ItemData array with
    ' corresponding items in sorted order.
    cboProduct.AddItem "VFEdit"
    cboProduct.ItemData(cboProduct.NewIndex) = 1
    cboProduct.AddItem "ToolBox"
    cboProduct.ItemData(cboProduct.NewIndex) = 2
    cboProduct.AddItem "TI/F DLL"
    cboProduct.ItemData(cboProduct.NewIndex) = 3
    cboProduct.AddItem "Scribe"
    cboProduct.ItemData(cboProduct.NewIndex) = 4
    cboProduct.AddItem "VoxFonts"
    cboProduct.ItemData(cboProduct.NewIndex) = 5
    cboProduct.ListIndex = Val(PrfGetStr(PRF_CUSREGSEC, PRF_PRDNAMKEY, Str(0)))

    'Defining each item in the Name_Array
    Name_Array(1) = "VFEDIT.LIC"
    Name_Array(2) = "TBXLIC.TXT"
    Name_Array(3) = "TIFDLL.LIC"
    Name_Array(4) = "SCRIBE.LIC"
    Name_Array(5) = "VOXFNT.LIC"

    ' Load previous customer data
    edtCustomer_Name = PrfGetStr(PRF_CUSREGSEC, PRF_CUSNAMKEY, "")
    edtCompany_Name = PrfGetStr(PRF_CUSREGSEC, PRF_CMPNAMKEY, "")
    edtSerial_Number = PrfGetStr(PRF_CUSREGSEC, PRF_SERNUMKEY, "")

End Sub

Function PrfGetDir () As String
    Dim sRetStr As String
    sRetStr = String(255, Chr$(STRNULDEL))

    If (0 <> GetWindowsDirectory(sRetStr, Len(sRetStr))) Then
	 PrfGetDir = Trim$(StrTokStr(sRetStr, Chr$(STRNULDEL))) & "\"
    Else PrfGetDir = ""
    End If
End Function

