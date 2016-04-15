VERSION 2.00
Begin Form frmModVer 
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Double
   Caption         =   "Module Verification"
   ClientHeight    =   2292
   ClientLeft      =   1092
   ClientTop       =   1536
   ClientWidth     =   5652
   Height          =   2664
   HelpContextID   =   32
   Left            =   1068
   LinkTopic       =   "Form1"
   ScaleHeight     =   2292
   ScaleWidth      =   5652
   Top             =   1188
   Width           =   5700
   Begin SSCheck chkSaveInfo 
      Caption         =   "Set expected to found version"
      FontBold        =   0   'False
      FontItalic      =   0   'False
      FontName        =   "MS Sans Serif"
      FontSize        =   7.8
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      Height          =   252
      Left            =   120
      TabIndex        =   7
      Top             =   1800
      Width           =   2532
   End
   Begin CommandButton cmdHelp 
      Caption         =   "Help"
      Height          =   375
      Left            =   3240
      TabIndex        =   1
      Top             =   1800
      Width           =   1095
   End
   Begin CommandButton cmdContinue 
      Caption         =   "Continue"
      Default         =   -1  'True
      Height          =   375
      Left            =   4440
      TabIndex        =   0
      Top             =   1800
      Width           =   1095
   End
   Begin SSFrame grpModVer 
      Caption         =   "Module"
      ForeColor       =   &H00000000&
      Height          =   1575
      Left            =   120
      TabIndex        =   6
      Top             =   120
      Width           =   5415
      Begin Label lblModFnd 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "Found:"
         Height          =   192
         Left            =   120
         TabIndex        =   2
         Top             =   960
         Width           =   576
      End
      Begin Label txtModFnd 
         BackColor       =   &H00C0C0C0&
         Caption         =   "Found"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   192
         Left            =   240
         TabIndex        =   3
         Top             =   1200
         Width           =   5088
      End
      Begin Label lblModExp 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "Expected:"
         Height          =   192
         Left            =   120
         TabIndex        =   4
         Top             =   360
         Width           =   840
      End
      Begin Label txtModExp 
         BackColor       =   &H00C0C0C0&
         Caption         =   "Expected"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   192
         Left            =   240
         TabIndex        =   5
         Top             =   600
         Width           =   5088
      End
   End
End
Option Explicit

Dim saveCursor As Integer

Sub cmdContinue_Click ()
    screen.MousePointer = saveCursor
    Hide
End Sub

Sub cmdHelp_Click ()
  SendKeys "{F1}"
End Sub

Sub Form_Load ()
  helpContextID = HLP_MODVER
  
  saveCursor = screen.MousePointer
  screen.MousePointer = DEFAULT
End Sub

