VERSION 2.00
Begin Form frmCvtFtr 
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Double
   Caption         =   "Filter Configure"
   ClientHeight    =   2772
   ClientLeft      =   1620
   ClientTop       =   1452
   ClientWidth     =   5172
   Height          =   3144
   KeyPreview      =   -1  'True
   Left            =   1596
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2772
   ScaleWidth      =   5172
   Top             =   1104
   Width           =   5220
   Begin SSFrame grpSIgFtr 
      Caption         =   "&Signal Filters"
      ForeColor       =   &H00000000&
      Height          =   735
      Left            =   120
      TabIndex        =   5
      Top             =   1920
      Width           =   3495
      Begin CheckBox chkDTMFtr 
         BackColor       =   &H00C0C0C0&
         Caption         =   "DTMF Notch Filter"
         Height          =   255
         Left            =   240
         TabIndex        =   6
         Tag             =   "Audio Conversion, DTMF Notch Filter"
         Top             =   360
         Width           =   3135
      End
   End
   Begin SSFrame grpFrqCvt 
      Caption         =   "&Frequency Conversion"
      ForeColor       =   &H00000000&
      Height          =   1755
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   3495
      Begin ComboBox cboResAlg 
         Height          =   300
         Left            =   240
         Style           =   2  'Dropdown List
         TabIndex        =   4
         Tag             =   "Audio Conversion, Resample Algorithm"
         Top             =   1260
         Width           =   3135
      End
      Begin ComboBox cboAAFAlg 
         Height          =   300
         Left            =   240
         Style           =   2  'Dropdown List
         TabIndex        =   2
         Tag             =   "Audio Conversion, Anti-aliasing Filter"
         Top             =   600
         Width           =   3135
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
         Height          =   255
         Left            =   240
         TabIndex        =   3
         Top             =   1020
         Width           =   3015
      End
   End
   Begin CommandButton cmdCancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   3720
      TabIndex        =   9
      Top             =   720
      Width           =   1335
   End
   Begin CommandButton cmdOk 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   3720
      TabIndex        =   8
      Top             =   240
      Width           =   1365
   End
   Begin CommandButton cmdHelp 
      Caption         =   "Help"
      Height          =   375
      Left            =   3720
      TabIndex        =   7
      Top             =   1620
      Width           =   1335
   End
End
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'   Voice Information Systems (1-800-234-VISI) Audio ToolBox Digital Processor
'                   (c) Copyright Andrew J. Michalik 1987-1995
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Option Explicit

Sub cboAAFAlg_Click ()
  UpdCtlClr
End Sub

Sub cboResAlg_Click ()
  UpdCtlClr
End Sub

Sub chkDTMFtr_Click ()
  UpdCtlClr
End Sub

Sub cmdCancel_Click ()
  Dim ii As Integer
  ' Restore Control contents
  For ii = 0 To Controls.Count - 1
    PrfCtlGet Controls(ii)
  Next ii

  FrmModRet Me, ""
End Sub

Sub cmdHelp_Click ()
  SendKeys "{F1}"
End Sub

Sub CmdOK_Click ()
  Dim ii As Integer
  
  ' Save Control contents
  For ii = 0 To Controls.Count - 1
    PrfCtlSet Controls(ii)
  Next ii

  FrmModRet Me, ""
End Sub

Sub Form_Activate ()
  ' Set focus to OK button
  cmdOK.SetFocus
End Sub

Sub Form_KeyPress (KeyAscii As Integer)
  If KeyChkEsc(KeyAscii) Then Call cmdCancel_Click
End Sub

Sub Form_Load ()
  helpContextID = HLP_FTRCFG
  Dim ii As Integer
  
  ' Initialize anti-aliasing filter types
  cboAAFAlg.AddItem AAFALGNRM
  cboAAFAlg.AddItem AAFALGOFF
  cboAAFAlg.AddItem AAFALGFST
  cboAAFAlg.AddItem AAFALGQ6I
  cboAAFAlg.AddItem AAFALGQ6F
  cboAAFAlg.AddItem AAFALGFFT
  cboAAFAlg.ListIndex = 0

  ' Initialize resample algorithm types
  cboResAlg.AddItem RESALGNRM
  cboResAlg.AddItem RESALGFFT
  cboResAlg.ListIndex = 0
  
  ' Restore Control contents
  For ii = 0 To Controls.Count - 1
    PrfCtlGet Controls(ii)
  Next ii

  ' Update control colors
  UpdCtlClr

  ' Yield to complete re-paints
  DoEvents

End Sub

Sub UpdCtlClr ()
  ' Gray all options
  lblAAFAlg.ForeColor = lLblDisClr
  cboAAFAlg.ForeColor = lValDisClr
  lblResAlg.ForeColor = lLblDisClr
  cboResAlg.ForeColor = lValDisClr
  chkDTMFtr.ForeColor = lLblDisClr

  ' Check anti-aliasing filter setting
  If (AAFALGNRM <> cboAAFAlg) Then
    lblAAFAlg.ForeColor = lLblEnaClr
    cboAAFAlg.ForeColor = lValEnaClr
  End If

  ' Check resample algorithm setting
  If (RESALGNRM <> cboResAlg) Then
    lblResAlg.ForeColor = lLblEnaClr
    cboResAlg.ForeColor = lValEnaClr
  End If

  ' Check DTMF notch filter setting
  If (chkDTMFtr) Then
    chkDTMFtr.ForeColor = lLblEnaClr
  End If

End Sub

