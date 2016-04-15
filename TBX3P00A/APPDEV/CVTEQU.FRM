VERSION 2.00
Begin Form frmCvtEqu 
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Double
   Caption         =   "Frequency Equalizer"
   ClientHeight    =   4344
   ClientLeft      =   840
   ClientTop       =   1632
   ClientWidth     =   4416
   ControlBox      =   0   'False
   FillColor       =   &H00C0C0C0&
   Height          =   4716
   KeyPreview      =   -1  'True
   Left            =   816
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4344
   ScaleWidth      =   4416
   Top             =   1284
   Width           =   4464
   Begin SSPanel pnlFrqEqu 
      BackColor       =   &H00C0C0C1&
      BevelInner      =   2  'Raised
      BevelOuter      =   0  'None
      BorderWidth     =   2
      Height          =   3132
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   4212
      Begin SSFrame grpEquBnd 
         ForeColor       =   &H00000000&
         Height          =   2892
         Left            =   120
         TabIndex        =   1
         Top             =   120
         Width           =   3972
         Begin CommandButton cmdEquRes 
            Caption         =   "Reset"
            Height          =   252
            Left            =   120
            TabIndex        =   13
            Top             =   2520
            Width           =   732
         End
         Begin MhSlide ctlEquBnd 
            BackColor       =   &H00C0C0C0&
            ForeColor       =   &H00C0C0C0&
            Height          =   2172
            Index           =   0
            LargeDown       =   10
            LargeUp         =   10
            Left            =   120
            Max             =   100
            Min             =   0
            PictureSlide    =   CVTEQU.FRX:0000
            Style           =   0  'Vertical
            TabIndex        =   2
            Top             =   120
            Value           =   50
            Width           =   372
         End
         Begin MhSlide ctlEquBnd 
            BackColor       =   &H00C0C0C0&
            ForeColor       =   &H00C0C0C0&
            Height          =   2172
            Index           =   1
            LargeDown       =   10
            LargeUp         =   10
            Left            =   600
            Max             =   100
            Min             =   0
            PictureSlide    =   CVTEQU.FRX:028A
            Style           =   0  'Vertical
            TabIndex        =   3
            Top             =   120
            Value           =   50
            Width           =   372
         End
         Begin MhSlide ctlEquBnd 
            BackColor       =   &H00C0C0C0&
            ForeColor       =   &H00C0C0C0&
            Height          =   2172
            Index           =   2
            LargeDown       =   10
            LargeUp         =   10
            Left            =   1080
            Max             =   100
            Min             =   0
            PictureSlide    =   CVTEQU.FRX:0514
            Style           =   0  'Vertical
            TabIndex        =   4
            Tag             =   "Audio Conversion, Frequency Band"
            Top             =   120
            Value           =   50
            Width           =   372
         End
         Begin MhSlide ctlEquBnd 
            BackColor       =   &H00C0C0C0&
            ForeColor       =   &H00C0C0C0&
            Height          =   2172
            Index           =   3
            LargeDown       =   10
            LargeUp         =   10
            Left            =   1560
            Max             =   100
            Min             =   0
            PictureSlide    =   CVTEQU.FRX:079E
            Style           =   0  'Vertical
            TabIndex        =   5
            Top             =   120
            Value           =   50
            Width           =   372
         End
         Begin MhSlide ctlEquBnd 
            BackColor       =   &H00C0C0C0&
            ForeColor       =   &H00C0C0C0&
            Height          =   2172
            Index           =   4
            LargeDown       =   10
            LargeUp         =   10
            Left            =   2040
            Max             =   100
            Min             =   0
            PictureSlide    =   CVTEQU.FRX:0A28
            Style           =   0  'Vertical
            TabIndex        =   6
            Top             =   120
            Value           =   50
            Width           =   372
         End
         Begin MhSlide ctlEquBnd 
            BackColor       =   &H00C0C0C0&
            ForeColor       =   &H00C0C0C0&
            Height          =   2172
            Index           =   5
            LargeDown       =   10
            LargeUp         =   10
            Left            =   2520
            Max             =   100
            Min             =   0
            PictureSlide    =   CVTEQU.FRX:0CB2
            Style           =   0  'Vertical
            TabIndex        =   7
            Top             =   120
            Value           =   50
            Width           =   372
         End
         Begin MhSlide ctlEquBnd 
            BackColor       =   &H00C0C0C0&
            ForeColor       =   &H00C0C0C0&
            Height          =   2172
            Index           =   6
            LargeDown       =   10
            LargeUp         =   10
            Left            =   3000
            Max             =   100
            Min             =   0
            PictureSlide    =   CVTEQU.FRX:0F3C
            Style           =   0  'Vertical
            TabIndex        =   8
            Top             =   120
            Value           =   50
            Width           =   372
         End
         Begin MhSlide ctlEquBnd 
            BackColor       =   &H00C0C0C0&
            ForeColor       =   &H00C0C0C0&
            Height          =   2172
            Index           =   7
            LargeDown       =   10
            LargeUp         =   10
            Left            =   3480
            Max             =   100
            Min             =   0
            PictureSlide    =   CVTEQU.FRX:11C6
            Style           =   0  'Vertical
            TabIndex        =   9
            Top             =   120
            Value           =   50
            Width           =   372
         End
         Begin Label lblFrqRng 
            Alignment       =   1  'Right Justify
            BackColor       =   &H00C0C0C0&
            Caption         =   "Max"
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   255
            Index           =   2
            Left            =   3000
            TabIndex        =   12
            Top             =   2280
            Width           =   855
         End
         Begin Label lblFrqRng 
            Alignment       =   2  'Center
            BackColor       =   &H00C0C0C0&
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   255
            Index           =   1
            Left            =   1560
            TabIndex        =   11
            Top             =   2280
            Width           =   855
         End
         Begin Label lblFrqRng 
            BackColor       =   &H00C0C0C0&
            Caption         =   "0"
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   255
            Index           =   0
            Left            =   240
            TabIndex        =   10
            Top             =   2280
            Width           =   855
         End
      End
   End
   Begin CommandButton cmdHlpBut 
      Caption         =   "Help"
      Height          =   375
      Left            =   360
      TabIndex        =   15
      Top             =   3840
      Width           =   1215
   End
   Begin CommandButton CmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   2880
      TabIndex        =   14
      Top             =   3360
      Width           =   1215
   End
   Begin CommandButton cmdCancel 
      Caption         =   "Cancel"
      Height          =   375
      Left            =   2880
      TabIndex        =   16
      Top             =   3840
      Width           =   1215
   End
   Begin Label txtCvtEqu 
      BackColor       =   &H00C0C0C0&
      FontBold        =   0   'False
      FontItalic      =   0   'False
      FontName        =   "MS Sans Serif"
      FontSize        =   7.8
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      Height          =   192
      Left            =   120
      TabIndex        =   17
      Top             =   4320
      Visible         =   0   'False
      Width           =   4212
   End
End

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'   Voice Information Systems (1-800-234-VISI) VFEdit Professional Prompt Editor
'                   (c) Copyright Andrew J. Michalik 1987-1996
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Option Explicit


''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Note: Due to odd repaint problem, set panel frmFrqEqu background
'       color on this form to &H00C0C0C1& (ie, gray + 1)
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
' Frequency Equalizer / Analyzer variables
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Const EQUBNDMAX = 100#                  ' Equalizer band maximum

Dim fEquGaiAdj As Single                   ' Gain Adjust
Dim fEquBotFrq As Single                   ' Equalizer bottom frequency
Dim fEquTopFrq As Single                   ' Equalizer top frequency

Sub cmdCancel_Click ()
    FrmModRet Me, ""
End Sub

Sub cmdEquRes_Click ()
    Dim ii
    ' Reset equalizer settings
    For ii = 0 To EQUBNDCNT - 1
        ctlEquBnd(ii).Value = EQUBNDMAX / 2
    Next ii

End Sub

Sub cmdHlpBut_Click ()
  SendKeys "{F1}"
End Sub

Sub CmdOK_Click ()
    Dim ii
    
    ' Store last equalizer settings
    For ii = 0 To EQUBNDCNT - 1
        PrfSetStr PRF_EFFCFGSEC, PRF_EQUBNDKEY & ii, DblRndAsc(EquBndTodB(ctlEquBnd(ii).Value), 1)
    Next ii

    ' Update equalizer command line
    txtCvtEqu = EquStrMak()
    
    ' Return equalizer arguments
    FrmModRet Me, txtCvtEqu

End Sub

Function EquBndTodB (ByVal iBndPct As Integer) As Single
    ' Note: The slide appears to go from 0 (top) to 100 (bottom), max range is +/-12 dB
    Dim fdB As Single
    fdB = EQUSLDRNG * (.5 - (iBndPct / EQUBNDMAX))
    fdB = Min(fdB, EQUSLDRNG)
    fdB = Max(fdB, -EQUSLDRNG)
    EquBndTodB = fdB
End Function

Function EqudBToBnd (fdB As Single) As Integer
    ' Note: The slide appears to go from 0 (top) to 100 (bottom), max range is +/-12 dB
    fdB = Min(fdB, EQUSLDRNG)
    fdB = Max(fdB, -EQUSLDRNG)
    EqudBToBnd = EQUBNDMAX * (.5 - (fdB / EQUSLDRNG))
End Function

Function EquStrMak () As String
    Dim sEquPrm As String
    Dim bEquRes As Integer
    Dim ii
    
    ' Read last equalizer settings and generate equalizer string
    ' Test if equalizer has been reset
    bEquRes = True
    For ii = 0 To EQUBNDCNT - 1
        Dim fBndVal As Single
        fBndVal = EquBndTodB(ctlEquBnd(ii).Value)
        sEquPrm = sEquPrm & "," & Trim$(Str$(fBndVal))
        If (0 <> fBndVal) Then bEquRes = False
    Next ii
    If (bEquRes) Then sEquPrm = ""
    
    ' Return string
    EquStrMak = sEquPrm

End Function

Sub Form_Activate ()
    Dim sInpArg As String
    Dim ii As Integer

    ' Retrieve input arguments from tag
    sInpArg = tag
    fEquGaiAdj = Val(StrTokStr(sInpArg, Chr$(TBXARGDEL)))
    fEquBotFrq = Val(StrTokStr(sInpArg, Chr$(TBXARGDEL)))
    fEquTopFrq = Val(StrTokStr(sInpArg, Chr$(TBXARGDEL)))

    ' Initialize equalizer frequency range legend
    If (0 <> fEquTopFrq) Then
        lblFrqRng(0) = DblRndAsc((fEquBotFrq / 1#), 1)
        lblFrqRng(1) = DblRndAsc((fEquTopFrq / 2#), 1)
        lblFrqRng(2) = DblRndAsc((fEquTopFrq / 1#), 1)
    End If

    ' Yield to complete re-paints
    DoEvents
    
End Sub

Sub Form_KeyPress (KeyAscii As Integer)
    ' Insure that it is safe to exit
    If (KeyChkEsc(KeyAscii)) Then Call cmdCancel_Click
End Sub

Sub Form_Load ()
    HelpContextID = HLP_CVTEQU
    Dim ii As Integer

    ' Read last equalizer settings
    For ii = 0 To EQUBNDCNT - 1
        ctlEquBnd(ii).Value = EqudBToBnd(Val(PrfGetStr(PRF_EFFCFGSEC, PRF_EQUBNDKEY & ii, "")))
    Next ii

    ' Update equalizer command line
    txtCvtEqu = EquStrMak()

End Sub

