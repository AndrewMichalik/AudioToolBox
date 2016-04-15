VERSION 2.00
Begin Form frmAbtVer 
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Double
   Caption         =   "Version Information"
   ClientHeight    =   3372
   ClientLeft      =   1176
   ClientTop       =   1536
   ClientWidth     =   6120
   Height          =   3744
   KeyPreview      =   -1  'True
   Left            =   1152
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3372
   ScaleWidth      =   6120
   Top             =   1188
   Width           =   6168
   Begin CommandButton Command1 
      Caption         =   "Edit .Ini File"
      Height          =   375
      Left            =   3240
      TabIndex        =   19
      Top             =   2880
      Width           =   1215
   End
   Begin SSFrame grpTBxEdi 
      Caption         =   "Edition"
      ForeColor       =   &H00000000&
      Height          =   732
      Left            =   120
      TabIndex        =   13
      Top             =   120
      Width           =   5892
      Begin Label lblMfgVer 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "Unknown"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   195
         Left            =   120
         TabIndex        =   12
         Top             =   360
         Width           =   690
      End
      Begin Label lblVerNum 
         Alignment       =   1  'Right Justify
         BackColor       =   &H00C0C0C0&
         Caption         =   "Version 3.00a.1"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   4200
         TabIndex        =   14
         Top             =   360
         Width           =   1572
      End
   End
   Begin SSFrame grpVBXVer 
      Caption         =   "Visual Basic Controls"
      ForeColor       =   &H00000000&
      Height          =   1812
      Left            =   3240
      TabIndex        =   4
      Top             =   960
      Width           =   2772
      Begin Label txtMHSl200 
         BackColor       =   &H00C0C0C0&
         Caption         =   "?"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   1200
         TabIndex        =   25
         Top             =   720
         Width           =   1452
      End
      Begin Label Label1 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "MHSl200"
         Height          =   192
         Left            =   120
         TabIndex        =   24
         Top             =   720
         Width           =   744
      End
      Begin Label lblThreeDVBX 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "ThreeD:"
         Height          =   192
         Left            =   120
         TabIndex        =   2
         Top             =   1440
         Width           =   708
      End
      Begin Label lblMHTabVBX 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "MHTab:"
         Height          =   192
         Left            =   120
         TabIndex        =   3
         Top             =   1080
         Width           =   696
      End
      Begin Label lblCMDialogVBX 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "CMDialog:"
         Height          =   195
         Left            =   120
         TabIndex        =   10
         Top             =   360
         Width           =   885
      End
      Begin Label txtThreeD 
         BackColor       =   &H00C0C0C0&
         Caption         =   "?"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   1200
         TabIndex        =   0
         Top             =   1440
         Width           =   1452
      End
      Begin Label txtMHTab 
         BackColor       =   &H00C0C0C0&
         Caption         =   "?"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   1200
         TabIndex        =   5
         Top             =   1080
         Width           =   1452
      End
      Begin Label txtCMDialog 
         BackColor       =   &H00C0C0C0&
         Caption         =   "?"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   1200
         TabIndex        =   6
         Top             =   360
         Width           =   1452
      End
   End
   Begin CommandButton cmdOk 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   372
      Left            =   4800
      TabIndex        =   9
      Top             =   2880
      Width           =   1215
   End
   Begin SSFrame grpDLLVer 
      Caption         =   "Dynamic Link Libraries"
      ForeColor       =   &H00000000&
      Height          =   2292
      Left            =   120
      TabIndex        =   1
      Top             =   960
      Width           =   3012
      Begin Label txtVer 
         BackColor       =   &H00C0C0C0&
         Caption         =   "?"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   1440
         TabIndex        =   23
         Top             =   1800
         Width           =   1452
      End
      Begin Label txtTBxIns 
         BackColor       =   &H00C0C0C0&
         Caption         =   "?"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   1440
         TabIndex        =   22
         Top             =   1440
         Width           =   1452
      End
      Begin Label lblVer 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "Ver:"
         Height          =   192
         Left            =   120
         TabIndex        =   21
         Top             =   1800
         Width           =   348
      End
      Begin Label lblTBxIns 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "TBxIns:"
         Height          =   192
         Left            =   120
         TabIndex        =   20
         Top             =   1440
         Width           =   624
      End
      Begin Label txtCommDLg 
         BackColor       =   &H00C0C0C0&
         Caption         =   "?"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   255
         Left            =   1440
         TabIndex        =   18
         Top             =   1080
         Width           =   1455
      End
      Begin Label lblCommDlg 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "CommDlg:"
         Height          =   195
         Left            =   120
         TabIndex        =   17
         Top             =   1080
         Width           =   855
      End
      Begin Label txtVBRun300 
         BackColor       =   &H00C0C0C0&
         Caption         =   "?"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   1440
         TabIndex        =   16
         Top             =   360
         Width           =   1452
      End
      Begin Label lblVBRun300DLL 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "VBRun300:"
         Height          =   195
         Left            =   120
         TabIndex        =   15
         Top             =   360
         Width           =   975
      End
      Begin Label txtMHRun400 
         BackColor       =   &H00C0C0C0&
         Caption         =   "?"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   1440
         TabIndex        =   7
         Top             =   720
         Width           =   1452
      End
      Begin Label txtMhRun300 
         BackColor       =   &H00C0C0C0&
         Height          =   252
         Left            =   1200
         TabIndex        =   8
         Top             =   360
         Width           =   1452
      End
      Begin Label lblMhRun400DLL 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "MhRun400:"
         Height          =   195
         Left            =   120
         TabIndex        =   11
         Top             =   720
         Width           =   990
      End
   End
End
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'   Voice Information Systems (1-800-234-VISI) AudTBx Professional Prompt Editor
'                   (c) Copyright Andrew J. Michalik 1987-1995
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Option Explicit

Sub CmdOK_Click ()
    FrmModRet Me, ""
End Sub

Sub Command1_Click ()
  Dim hInst%

  hInst = Shell("notepad " & sPrfFilNam, 3)
End Sub

Sub Form_KeyPress (KeyAscii As Integer)
    If KeyChkEsc(KeyAscii) Then Call CmdOK_Click
End Sub

Sub Form_Load ()
  helpContextID = HLP_MODVER
  Dim iMajVer As Integer
  Dim iMinVer As Integer
  Dim iSubMaj As Integer
  Dim iSubMin As Integer
  Dim sDesStr As String
  Dim iSupTyp As Integer
  sDesStr = String(255, Chr$(STRNULDEL))

  ' Yield to complete re-paints
  DoEvents

  ' Get Manufacturer's version
  lblMfgVer = MFGVERUNK
  
  If (MFGVERDLG = PrfGetStr(PRF_MFGVERKEY, PRF_MFGVERSEC, "")) Then lblMfgVer = MFGVERDLG
  If (MFGVERG21 = PrfGetStr(PRF_MFGVERKEY, PRF_MFGVERSEC, "")) Then lblMfgVer = MFGVERG21
  If (MFGVERNWV = PrfGetStr(PRF_MFGVERKEY, PRF_MFGVERSEC, "")) Then lblMfgVer = MFGVERNWV
  If (MFGVERPTC = PrfGetStr(PRF_MFGVERKEY, PRF_MFGVERSEC, "")) Then lblMfgVer = MFGVERPTC
  If (MFGVERRTX = PrfGetStr(PRF_MFGVERKEY, PRF_MFGVERSEC, "")) Then lblMfgVer = MFGVERRTX
  
  If (MFGVERUNK = lblMfgVer) Then MsgBox "Warning! Unknown edition in .INI file, using defaults...", MB_ICONEXCLAMATION

  ' Get version numbers for DLL's
  txtVBRun300.Caption = GetFileVersion("VBRun300.DLL")
  txtMhRun400.Caption = GetFileVersion("MhRun400.DLL")
  txtCommDlg.Caption = GetFileVersion("CommDlg.DLL")
  txtTBxIns.Caption = GetFileVersion("TBxIns.DLL")
  txtVer.Caption = GetFileVersion("Ver.DLL")

  ' Get version numbers for VBX's
  txtCMDialog.Caption = GetFileVersion("CMDialog.VBX")
  txtMHTab.Caption = GetFileVersion("MHTab.VBX")
  txtMHSl200.Caption = GetFileVersion("MHSl200.VBX")
  txtThreeD.Caption = GetFileVersion("ThreeD.VBX")

  ' Yield to complete re-paints
  DoEvents

End Sub

