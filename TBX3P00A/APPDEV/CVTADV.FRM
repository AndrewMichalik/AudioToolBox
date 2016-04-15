VERSION 2.00
Begin Form frmCvtAdv 
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Double
   Caption         =   "Conversion Advanced Options"
   ClientHeight    =   3612
   ClientLeft      =   1008
   ClientTop       =   1788
   ClientWidth     =   5412
   Height          =   3984
   HelpContextID   =   30
   KeyPreview      =   -1  'True
   Left            =   984
   LinkTopic       =   "Form1"
   ScaleHeight     =   3612
   ScaleWidth      =   5412
   Top             =   1440
   Visible         =   0   'False
   Width           =   5460
   Begin CommandButton cmdHelp 
      Caption         =   "&Help"
      Height          =   372
      Left            =   3960
      TabIndex        =   17
      Top             =   1800
      Width           =   1332
   End
   Begin CommandButton cmdBrowse 
      Caption         =   "&Browse..."
      Enabled         =   0   'False
      Height          =   372
      Left            =   3960
      TabIndex        =   16
      TabStop         =   0   'False
      Top             =   1320
      Width           =   1332
   End
   Begin CommandButton cmdOk 
      Caption         =   "OK"
      Height          =   375
      Left            =   3960
      TabIndex        =   18
      Top             =   2640
      Width           =   1365
   End
   Begin CommandButton cmdCancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   3960
      TabIndex        =   19
      Top             =   3120
      Width           =   1335
   End
   Begin SSFrame grpBytFmt 
      Caption         =   "&Data Byte Format"
      ForeColor       =   &H00000000&
      Height          =   1095
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   5175
      Begin ComboBox cboSwpInp 
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   300
         Left            =   3840
         Style           =   2  'Dropdown List
         TabIndex        =   6
         Tag             =   "Audio Conversion, Swap Input"
         Top             =   360
         Width           =   1215
      End
      Begin ComboBox cboSwpOut 
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   300
         Left            =   3840
         Style           =   2  'Dropdown List
         TabIndex        =   8
         Tag             =   "Audio Conversion, Swap Output"
         Top             =   720
         Width           =   1215
      End
      Begin TextBox edtBytPos 
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   288
         Left            =   1680
         TabIndex        =   2
         Tag             =   "Audio Conversion, Byte Position"
         Text            =   "0"
         Top             =   360
         Width           =   732
      End
      Begin TextBox edtBytLen 
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   288
         Left            =   1680
         TabIndex        =   4
         Tag             =   "Audio Conversion, Byte Length"
         Text            =   "all"
         Top             =   720
         Width           =   732
      End
      Begin Label lblSwpInp 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "Swap Input:"
         Height          =   195
         Left            =   2640
         TabIndex        =   5
         Top             =   360
         Width           =   960
      End
      Begin Label lblSwpOut 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "Swap Output:"
         Height          =   195
         Left            =   2640
         TabIndex        =   7
         Tag             =   "Audio Conversion, Swap Input"
         Top             =   720
         Width           =   1095
      End
      Begin Label lblBytLen 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "Length (bytes):"
         Height          =   192
         Left            =   120
         TabIndex        =   3
         Top             =   720
         Width           =   1236
      End
      Begin Label lblBytPos 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "Position (bytes)"
         Height          =   195
         Left            =   120
         TabIndex        =   1
         Top             =   360
         Width           =   1320
      End
   End
   Begin SSFrame grpCfgFil 
      Caption         =   "&Configuration Files"
      ForeColor       =   &H00000000&
      Height          =   2175
      Left            =   120
      TabIndex        =   9
      Top             =   1320
      Width           =   3735
      Begin TextBox edtCfgGlo 
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   285
         Left            =   120
         TabIndex        =   11
         Tag             =   "Audio Conversion, Configuration Global"
         Top             =   600
         Width           =   3495
      End
      Begin TextBox edtCfgInp 
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   285
         Left            =   120
         TabIndex        =   13
         Tag             =   "Audio Conversion, Configuration Input"
         Top             =   1200
         Width           =   3495
      End
      Begin TextBox edtCfgOut 
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   285
         Left            =   120
         TabIndex        =   15
         Tag             =   "Audio Conversion, Configuration Output"
         Top             =   1800
         Width           =   3495
      End
      Begin Label lblCfgGlo 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "Global:"
         Height          =   195
         Left            =   120
         TabIndex        =   10
         Top             =   360
         Width           =   615
      End
      Begin Label lblCfgInp 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "Input:"
         Height          =   195
         Left            =   120
         TabIndex        =   12
         Top             =   960
         Width           =   450
      End
      Begin Label lblCfgOut 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "Output:"
         Height          =   195
         Left            =   120
         TabIndex        =   14
         Top             =   1560
         Width           =   585
      End
   End
End
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'   Voice Information Systems (1-800-234-VISI) Audio ToolBox Digital Processor
'                   (c) Copyright Andrew J. Michalik 1987-1995
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Option Explicit

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Dim cBrwActCtl       As Control         ' Previously active control

Sub cboSwpInp_Click ()
  UpdCtlClr
End Sub

Sub cboSwpOut_Click ()
  UpdCtlClr
End Sub

Sub cmdCancel_Click ()
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

Sub edtBytLen_Change ()
  UpdCtlClr
End Sub

Sub edtBytLen_LostFocus ()
  If (0 = Val(edtBytLen)) Then edtBytLen = OPTALLDEF
End Sub

Sub edtBytPos_Change ()
  UpdCtlClr
End Sub

Sub edtBytPos_LostFocus ()
  If (0 = Val(edtBytPos)) Then edtBytPos = 0
End Sub

Sub edtCfgGlo_Change ()
  UpdCtlClr
End Sub

Sub edtCfgGlo_GotFocus ()
  ' Set currently active control & enable browse button
  Set cBrwActCtl = edtCfgGlo
  cmdBrowse.Enabled = True
End Sub

Sub edtCfgGlo_LostFocus ()
  ' Force default value if nothing entered
  If ("" = Trim(edtCfgGlo)) Then edtCfgGlo = ""
  
  ' Disable browse button (unless browse is getting focus)
  If (cmdBrowse <> ActiveControl) Then cmdBrowse.Enabled = False
End Sub

Sub edtCfgInp_Change ()
  UpdCtlClr
End Sub

Sub edtCfgInp_GotFocus ()
  ' Set currently active control & enable browse button
  Set cBrwActCtl = edtCfgInp
  cmdBrowse.Enabled = True
End Sub

Sub edtCfgInp_LostFocus ()
  ' Force default value if nothing entered
  If ("" = Trim(edtCfgInp)) Then edtCfgInp = ""
  
  ' Disable browse button (unless browse is getting focus)
  If (cmdBrowse <> ActiveControl) Then cmdBrowse.Enabled = False
End Sub

Sub edtCfgOut_Change ()
  UpdCtlClr
End Sub

Sub edtCfgOut_GotFocus ()
  ' Set currently active control & enable browse button
  Set cBrwActCtl = edtCfgOut
  cmdBrowse.Enabled = True
End Sub

Sub edtCfgOut_LostFocus ()
  ' Force default value if nothing entered
  If ("" = Trim(edtCfgOut)) Then edtCfgOut = ""
  
  ' Disable browse button (unless browse is getting focus)
  If (cmdBrowse <> ActiveControl) Then cmdBrowse.Enabled = False
End Sub

Sub Form_Activate ()
  ' Set focus to OK button
  cmdOK.SetFocus
End Sub

Sub Form_KeyPress (KeyAscii As Integer)
  If KeyChkEsc(KeyAscii) Then Call cmdCancel_Click
End Sub

Sub Form_Load ()
  helpContextID = HLP_CVTADV
  
  Dim ii As Integer
  
  ' Load swap comboboxes
  cboSwpInp.AddItem OPTNONDEF
  cboSwpInp.AddItem CVTSWPBIT
  cboSwpInp.AddItem CVTSWPNIB
  cboSwpInp.AddItem CVTSWPBYT
  cboSwpInp.AddItem CVTSWPWRD
  For ii = 0 To cboSwpInp.ListCount - 1
    cboSwpOut.AddItem cboSwpInp.List(ii)
  Next ii
  
  ' Set combobox default values
  cboSwpInp.ListIndex = 0
  cboSwpOut.ListIndex = 0

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
  lblBytPos.ForeColor = lLblDisClr
  edtBytPos.ForeColor = lValDisClr
  lblBytLen.ForeColor = lLblDisClr
  edtBytLen.ForeColor = lValDisClr
  lblSwpInp.ForeColor = lLblDisClr
  cboSwpInp.ForeColor = lValDisClr
  lblSwpOut.ForeColor = lLblDisClr
  cboSwpOut.ForeColor = lValDisClr
  lblCfgGlo.ForeColor = lLblDisClr
  edtCfgGlo.ForeColor = lValDisClr
  lblCfgInp.ForeColor = lLblDisClr
  edtCfgInp.ForeColor = lValDisClr
  lblCfgOut.ForeColor = lLblDisClr
  edtCfgOut.ForeColor = lValDisClr

  ' Check Input Position Offset
  If (0 <> Val(edtBytPos)) Then
    lblBytPos.ForeColor = lLblEnaClr
    edtBytPos.ForeColor = lValEnaClr
  End If

  ' Check Input Length
  If (0 <> Val(edtBytLen)) Then
    lblBytLen.ForeColor = lLblEnaClr
    edtBytLen.ForeColor = lValEnaClr
  End If

  ' Check swap input
  If (0 <> StrComp(OPTNONDEF, cboSwpInp, 1)) Then
    lblSwpInp.ForeColor = lLblEnaClr
    cboSwpInp.ForeColor = lValEnaClr
  End If

  ' Check swap output
  If (0 <> StrComp(OPTNONDEF, cboSwpOut, 1)) Then
    lblSwpOut.ForeColor = lLblEnaClr
    cboSwpOut.ForeColor = lValEnaClr
  End If

  ' Check global configuration file
  If ("" <> Trim(edtCfgGlo)) Then
    lblCfgGlo.ForeColor = lLblEnaClr
    edtCfgGlo.ForeColor = lValEnaClr
  End If
  
  ' Check input configuration file
  If ("" <> Trim(edtCfgInp)) Then
    lblCfgInp.ForeColor = lLblEnaClr
    edtCfgInp.ForeColor = lValEnaClr
  End If
  
  ' Check output configuration file
  If ("" <> Trim(edtCfgOut)) Then
    lblCfgOut.ForeColor = lLblEnaClr
    edtCfgOut.ForeColor = lValEnaClr
  End If
  
End Sub

