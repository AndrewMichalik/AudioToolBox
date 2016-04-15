VERSION 2.00
Begin Form frmFtrCfg 
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Double
   Caption         =   "Filter Configure"
   ClientHeight    =   3012
   ClientLeft      =   4488
   ClientTop       =   3948
   ClientWidth     =   5628
   Height          =   3384
   KeyPreview      =   -1  'True
   Left            =   4464
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3012
   ScaleWidth      =   5628
   Top             =   3600
   Width           =   5676
   Begin SSFrame Frame3D1 
      Caption         =   "&Signal Filters:"
      Font3D          =   0  'None
      ForeColor       =   &H00000000&
      Height          =   732
      Left            =   120
      TabIndex        =   8
      Top             =   2160
      Width           =   3732
      Begin ComboBox swQx 
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   288
         Index           =   0
         Left            =   2400
         Style           =   2  'Dropdown List
         TabIndex        =   9
         Top             =   360
         Width           =   1092
      End
      Begin Label Label6 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "DTMF Filter"
         Height          =   192
         Left            =   240
         TabIndex        =   10
         Top             =   360
         Width           =   2052
      End
   End
   Begin SSFrame grpFrqCnv 
      Caption         =   "&Frequency Conversion"
      Font3D          =   0  'None
      ForeColor       =   &H00000000&
      Height          =   1872
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   3732
      Begin ComboBox cboResAlg 
         Height          =   288
         Left            =   240
         Style           =   2  'Dropdown List
         TabIndex        =   4
         Top             =   1380
         Width           =   3252
      End
      Begin ComboBox cboAAFAlg 
         Height          =   288
         Left            =   240
         Style           =   2  'Dropdown List
         TabIndex        =   2
         Top             =   600
         Width           =   3252
      End
      Begin Label lblAAFAlg 
         BackColor       =   &H00C0C0C0&
         Caption         =   "&Anti-aliasing Filter:"
         Height          =   252
         Left            =   240
         TabIndex        =   1
         Top             =   360
         Width           =   3012
      End
      Begin Label lblResAlg 
         BackColor       =   &H00C0C0C0&
         Caption         =   "&Resample Algorithm:"
         Height          =   252
         Left            =   240
         TabIndex        =   3
         Top             =   1140
         Width           =   3012
      End
   End
   Begin CommandButton cmdCancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   4080
      TabIndex        =   7
      Top             =   600
      Width           =   1335
   End
   Begin CommandButton cmdOk 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   4080
      TabIndex        =   6
      Top             =   120
      Width           =   1365
   End
   Begin CommandButton cmdHlpBut 
      Caption         =   "Help"
      Height          =   375
      Left            =   4080
      TabIndex        =   5
      Top             =   1500
      Visible         =   0   'False
      Width           =   1335
   End
End
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'   Voice Information Systems (1-800-234-VISI) VFEdit Professional Prompt Editor
'                   (c) Copyright Andrew J. Michalik 1987-1995
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Option Explicit

Sub cmdCancel_Click ()
    FrmModRet Me, ""
End Sub

Sub cmdOK_Click ()
    PrfSetStr PRF_FTRCFGSEC, PRF_CFGAAFKEY, Str$(cboAAFAlg.ItemData(cboAAFAlg.ListIndex))
    PrfSetStr PRF_FTRCFGSEC, PRF_CFGRESKEY, Str$(cboResAlg.ItemData(cboResAlg.ListIndex))
    FrmModRet Me, Str$(cboAAFAlg.ItemData(cboAAFAlg.ListIndex)) & Chr$(TBXARGDEL) & Str$(cboResAlg.ItemData(cboResAlg.ListIndex))
End Sub

Sub Form_Activate ()
    Dim sInpVar As String
    Dim iDefAlg As Integer
    Dim iAlgEnu As Integer
    Dim sDesStr As String
    sDesStr = String(255, Chr$(STRNULDEL))

    ' Retrieve input arguments from tag: OffSet, Length, Format
    sInpVar = tag
    Caption = StrTokStr(sInpVar, Chr$(TBXARGDEL))
    
    ' Scan for supported anti-aliasing filter algorithms; set default
    iAlgEnu = 0
    cboAAFAlg.Clear
'    iDefAlg = Val(PrfGetStr(PRF_FTRCFGSEC, PRF_CFGAAFKEY, Str$(AAFALGNRM)))
'    While (AudFilAAFEnu(iAlgEnu, sDesStr, Len(sDesStr)))
'        cboAAFAlg.AddItem (StrTokStr(sDesStr, Chr$(STRNULDEL)))
'        cboAAFAlg.ItemData(cboAAFAlg.NewIndex) = iAlgEnu
'        If (iAlgEnu = iDefAlg) Then cboAAFAlg.ListIndex = cboAAFAlg.NewIndex
'        sDesStr = String(255, Chr$(STRNULDEL))
'    Wend
'    If (-1 = cboAAFAlg.ListIndex) Then cboAAFAlg.ListIndex = FndLstIdx(cboAAFAlg, AAFALGNRM)

    ' Scan for supported resample algorithms; set default
    iAlgEnu = 0
    cboResAlg.Clear
'    iDefAlg = Val(PrfGetStr(PRF_FTRCFGSEC, PRF_CFGRESKEY, Str$(RESALGNRM)))
'    While (AudFilResEnu(iAlgEnu, sDesStr, Len(sDesStr)))
'        cboResAlg.AddItem (StrTokStr(sDesStr, Chr$(STRNULDEL)))
'        cboResAlg.ItemData(cboResAlg.NewIndex) = iAlgEnu
'        If (iAlgEnu = iDefAlg) Then cboResAlg.ListIndex = cboResAlg.NewIndex
'        sDesStr = String(255, Chr$(STRNULDEL))
'    Wend
'    If (-1 = cboResAlg.ListIndex) Then cboResAlg.ListIndex = FndLstIdx(cboResAlg, RESALGNRM)

End Sub

Sub Form_KeyPress (KeyAscii As Integer)
    If KeyChkEsc(KeyAscii) Then Call cmdCancel_Click
End Sub

Sub Form_Load ()
    ' Yield to complete re-paints
    DoEvents
End Sub

