VERSION 2.00
Begin Form frmAbtTBx 
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Double
   Caption         =   "About Audio ToolBox ®"
   ClientHeight    =   4668
   ClientLeft      =   1296
   ClientTop       =   1776
   ClientWidth     =   6372
   Height          =   5040
   KeyPreview      =   -1  'True
   Left            =   1272
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4668
   ScaleWidth      =   6372
   Top             =   1428
   Width           =   6420
   Begin SSFrame grpRegLic 
      Caption         =   "This product is licensed to:"
      ForeColor       =   &H00000000&
      Height          =   1212
      Left            =   120
      TabIndex        =   11
      Top             =   3360
      Width           =   4452
      Begin Label lblRegLic 
         BackColor       =   &H00C0C0C0&
         Caption         =   "Warning: Unlicensed copy!"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   732
         Left            =   120
         TabIndex        =   12
         Top             =   360
         Width           =   4212
      End
   End
   Begin SSFrame grpVIS 
      ForeColor       =   &H00000000&
      Height          =   2175
      Left            =   120
      TabIndex        =   0
      Top             =   0
      Width           =   6135
      Begin Label lblem_adr 
         Alignment       =   1  'Right Justify
         BackColor       =   &H00C0C0C0&
         Caption         =   "sales@vinfo.com"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   3720
         TabIndex        =   5
         Top             =   1440
         Width           =   2292
      End
      Begin Label lblDomSal 
         Alignment       =   1  'Right Justify
         BackColor       =   &H00C0C0C0&
         Caption         =   "Sales: 1-800-234-VISI"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   3720
         TabIndex        =   1
         Top             =   240
         Width           =   2292
      End
      Begin Label lblIntSal 
         Alignment       =   1  'Right Justify
         BackColor       =   &H00C0C0C0&
         Caption         =   "Int'l: +310-392-8780"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   3720
         TabIndex        =   3
         Top             =   840
         Width           =   2292
      End
      Begin Label lblDomFax 
         Alignment       =   1  'Right Justify
         BackColor       =   &H00C0C0C0&
         Caption         =   "Fax: 1-800-234-FXIT"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   3720
         TabIndex        =   2
         Top             =   540
         Width           =   2292
      End
      Begin Label lblIntFax 
         Alignment       =   1  'Right Justify
         BackColor       =   &H00C0C0C0&
         Caption         =   "Fax: +310-392-5511"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   3720
         TabIndex        =   4
         Top             =   1140
         Width           =   2292
      End
      Begin Label lblNetNam 
         Alignment       =   1  'Right Justify
         BackColor       =   &H00C0C0C0&
         Caption         =   "www.vinfo.com"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   252
         Left            =   3720
         TabIndex        =   6
         Top             =   1740
         Width           =   2292
      End
      Begin Image imgVISLgo 
         Height          =   1608
         Left            =   120
         Picture         =   ABTTBX.FRX:0000
         Top             =   120
         Width           =   3468
      End
   End
   Begin CommandButton cmdVerInf 
      Caption         =   "&System Info..."
      Height          =   372
      Left            =   4680
      TabIndex        =   14
      Top             =   3720
      Width           =   1572
   End
   Begin CommandButton cmdOk 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   372
      Left            =   4680
      TabIndex        =   15
      Top             =   4200
      Width           =   1572
   End
   Begin PictureBox picVFELgo 
      AutoSize        =   -1  'True
      BorderStyle     =   0  'None
      Height          =   384
      Left            =   120
      Picture         =   ABTTBX.FRX:4DF2
      ScaleHeight     =   384
      ScaleWidth      =   384
      TabIndex        =   7
      TabStop         =   0   'False
      Top             =   2280
      Width           =   384
   End
   Begin Label lblVerTxt 
      Alignment       =   2  'Center
      BackColor       =   &H00C0C0C0&
      FontBold        =   -1  'True
      FontItalic      =   0   'False
      FontName        =   "MS Sans Serif"
      FontSize        =   9.6
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      Height          =   315
      Left            =   720
      TabIndex        =   9
      Top             =   2640
      Width           =   4935
   End
   Begin Label lblLckCod 
      Alignment       =   2  'Center
      BackColor       =   &H00C0C0C0&
      Caption         =   "00000000"
      FontBold        =   0   'False
      FontItalic      =   0   'False
      FontName        =   "MS Sans Serif"
      FontSize        =   7.8
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      Height          =   255
      Left            =   4680
      TabIndex        =   13
      Top             =   3360
      Width           =   1575
   End
   Begin Label lblCopyRight 
      Alignment       =   2  'Center
      BackColor       =   &H00C0C0C0&
      Caption         =   "Copyright © 1987-1996 Andrew J. Michalik."
      FontBold        =   0   'False
      FontItalic      =   0   'False
      FontName        =   "MS Sans Serif"
      FontSize        =   7.8
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      Height          =   255
      Left            =   1440
      TabIndex        =   10
      Top             =   3000
      Width           =   3495
   End
   Begin Label lblAudTBx 
      Alignment       =   2  'Center
      BackColor       =   &H00C0C0C0&
      Caption         =   "Audio ToolBox ® Apprentice"
      FontBold        =   -1  'True
      FontItalic      =   0   'False
      FontName        =   "MS Sans Serif"
      FontSize        =   9.6
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      Height          =   315
      Left            =   720
      TabIndex        =   8
      Top             =   2280
      Width           =   4935
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

Sub cmdVerInf_Click ()
    Dim sRetArg As String
    sRetArg = FrmModShw(frmAudTBx, frmAbtVer, "", CTR_MIDMIDNON)
End Sub

Sub Form_Activate ()
    Dim sRegTxt As String
    ' Retrieve input arguments from tag
    lblVerTxt = tag

    ' Retrieve license information
    sRegTxt = RegGetStr(PRF_REGLICSEC, PRF_REGUSRKEY, "")
    If ("" <> sRegTxt) Then sRegTxt = sRegTxt & CRLF()
    sRegTxt = sRegTxt & RegGetStr(PRF_REGLICSEC, PRF_REGCMPKEY, "")
    If ("" <> sRegTxt) Then sRegTxt = sRegTxt & CRLF()
    sRegTxt = sRegTxt & RegGetStr(PRF_REGLICSEC, PRF_REGSERKEY, "")
    
    ' Set license information
    If ChkRegSer(0, RegGetStr(PRF_REGLICSEC, PRF_REGSERKEY, "")) And ("" <> sRegTxt) Then lblRegLic = sRegTxt

    'Set focus to OK button
    cmdOK.SetFocus

    ' Yield to complete re-paints
    DoEvents

End Sub

Sub Form_KeyPress (KeyAscii As Integer)
    If KeyChkEsc(KeyAscii) Then Call CmdOK_Click
End Sub

Sub Form_Load ()
    helpContextID = HLP_ABTVIS
    
    ' Yield to complete re-paints
    DoEvents

End Sub

Private Function RegGetStr (ByVal sSecNam As String, ByVal sKeyNam As String, ByVal sDefVal As String)
    Dim sRetStr As String
    sRetStr = String(255, Chr$(STRNULDEL))

    ' Check application original directory for registration file
    If (0 <> GetPrivateProfileString(sSecNam, sKeyNam, sDefVal, sRetStr, Len(sRetStr), SafGetDir(App.Path) & PRF_REGLICFIL)) Then
         RegGetStr = Trim$(StrTokStr(sRetStr, Chr$(STRNULDEL)))
    Else RegGetStr = Trim$(sDefVal)
    End If
End Function

