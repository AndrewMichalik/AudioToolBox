VERSION 2.00
Begin Form frmEnvOpt 
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Double
   Caption         =   "Environment Options"
   ClientHeight    =   2508
   ClientLeft      =   1164
   ClientTop       =   2628
   ClientWidth     =   5052
   Height          =   2880
   KeyPreview      =   -1  'True
   Left            =   1140
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2508
   ScaleWidth      =   5052
   Top             =   2280
   Visible         =   0   'False
   Width           =   5100
   Begin SSFrame grpUsrPrf 
      Caption         =   "&Preferences"
      ForeColor       =   &H00000000&
      Height          =   2292
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   3372
      Begin ComboBox cboExiMod 
         Height          =   288
         Left            =   120
         Style           =   2  'Dropdown List
         TabIndex        =   8
         Tag             =   "Environment Options, Exit Mode"
         Top             =   1200
         Width           =   3132
      End
      Begin TextBox edtPrgDir 
         Height          =   315
         Left            =   120
         TabIndex        =   4
         Tag             =   "Environment Options, Program Files"
         Top             =   1800
         Width           =   3135
      End
      Begin ComboBox cboExeMod 
         Height          =   300
         Left            =   120
         Style           =   2  'Dropdown List
         TabIndex        =   2
         Tag             =   "Environment Options, Execution Mode"
         Top             =   600
         Width           =   3135
      End
      Begin Label lblExiMod 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "E&xit Mode"
         Height          =   192
         Left            =   120
         TabIndex        =   9
         Top             =   960
         Width           =   828
      End
      Begin Label lblPrgDir 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "&Program Files:"
         Height          =   192
         Left            =   120
         TabIndex        =   3
         Top             =   1560
         Width           =   1236
      End
      Begin Label lblExeMod 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "&Execution Mode:"
         Height          =   195
         Left            =   120
         TabIndex        =   1
         Top             =   360
         Width           =   1380
      End
   End
   Begin CommandButton cmdCancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   3600
      TabIndex        =   7
      Top             =   600
      Width           =   1335
   End
   Begin CommandButton cmdOk 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   3600
      TabIndex        =   6
      Top             =   120
      Width           =   1365
   End
   Begin CommandButton cmdHlpBut 
      Caption         =   "Help"
      Height          =   375
      Left            =   3600
      TabIndex        =   5
      Top             =   2040
      Width           =   1335
   End
End
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'   Voice Information Systems (1-800-234-VISI) VFEdit Professional Prompt Editor
'                   (c) Copyright Andrew J. Michalik 1987-1995
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

Option Explicit

Sub cboExeMod_Click ()
  UpdCtlClr
End Sub

Sub cboExiMod_Click ()
  UpdCtlClr
End Sub

Sub cmdCancel_Click ()
  FrmModRet Me, ""
End Sub

Sub cmdHlpBut_Click ()
  SendKeys "{F1}"
End Sub

Sub CmdOK_Click ()
  Dim ii As Integer
  
  ' Verify valid program directory
  If EdtDirBad(edtPrgDir) Then Exit Sub
  
  ' Save Control contents
  For ii = 0 To Controls.Count - 1
    PrfCtlSet Controls(ii)
  Next ii

  FrmModRet Me, ""

End Sub

Sub edtPrgDir_Change ()
  UpdCtlClr
End Sub

Sub edtPrgDir_LostFocus ()
  ' Force default value if nothing entered
  If ("" = Trim(edtPrgDir)) Then edtPrgDir = ""
  
End Sub

Sub Form_Activate ()
  ' Set focus to OK button
  cmdOK.SetFocus
End Sub

Sub Form_KeyPress (KeyAscii As Integer)
  If KeyChkEsc(KeyAscii) Then Call cmdCancel_Click
End Sub

Sub Form_Load ()
  helpContextID = HLP_ENVOPT
  Dim ii As Integer
    
  ' Load execution mode selection box
  cboExeMod.Clear
  cboExeMod.AddItem (EXEMODNRM)
  cboExeMod.ItemData(cboExeMod.NewIndex) = 1
  cboExeMod.AddItem (EXEMODMIN)
  cboExeMod.ItemData(cboExeMod.NewIndex) = 2
  cboExeMod.AddItem (EXEMODHID)
  cboExeMod.ItemData(cboExeMod.NewIndex) = 0
  cboExeMod.ListIndex = 0

  ' Load exit mode selection box
  cboExiMod.Clear
  cboExiMod.AddItem (EXIMODNRM)
  cboExiMod.ItemData(cboExiMod.NewIndex) = EXIALLEXI
  cboExiMod.AddItem (EXIMODERR)
  cboExiMod.ItemData(cboExiMod.NewIndex) = EXIERRPAU
  cboExiMod.AddItem (EXIMODWRN)
  cboExiMod.ItemData(cboExiMod.NewIndex) = EXIWRNPAU
  cboExiMod.AddItem (EXIMODALL)
  cboExiMod.ItemData(cboExiMod.NewIndex) = 3
  cboExiMod.ListIndex = 0

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
  lblExeMod.ForeColor = lLblDisClr
  cboExeMod.ForeColor = lValDisClr
  cboExiMod.ForeColor = lValDisClr
  lblPrgDir.ForeColor = lLblDisClr
  edtPrgDir.ForeColor = lValDisClr

  ' Check execution mode setting
  If (EXEMODNRM <> cboExeMod) Then
    lblExeMod.ForeColor = lLblEnaClr
    cboExeMod.ForeColor = lValEnaClr
  End If
  
  ' Check execution mode setting
  If (EXIMODNRM <> cboExiMod) Then
    lblExiMod.ForeColor = lLblEnaClr
    cboExiMod.ForeColor = lValEnaClr
  End If
  
  ' Check program directory setting
  If (0 <> Len(edtPrgDir)) Then
    lblPrgDir.ForeColor = lLblEnaClr
    edtPrgDir.ForeColor = lValEnaClr
  End If


End Sub

