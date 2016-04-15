VERSION 2.00
Begin Form Form1 
   Caption         =   "Wave Convertor"
   ClientHeight    =   4452
   ClientLeft      =   1080
   ClientTop       =   1668
   ClientWidth     =   6720
   ClipControls    =   0   'False
   Height          =   4824
   Icon            =   IDXTST.FRX:0000
   Left            =   1056
   LinkTopic       =   "Form1"
   ScaleHeight     =   4452
   ScaleWidth      =   6720
   Top             =   1320
   Width           =   6768
   Begin CommandButton btnFilCvt 
      Caption         =   "&Convert"
      Enabled         =   0   'False
      Height          =   495
      Left            =   5280
      TabIndex        =   8
      Top             =   3060
      Width           =   1215
   End
   Begin CommandButton btnExit 
      Caption         =   "E&xit"
      Height          =   495
      Left            =   5280
      TabIndex        =   6
      Top             =   3780
      Width           =   1215
   End
   Begin FileListBox filFilLst 
      Height          =   1980
      Left            =   240
      Pattern         =   "*.wav"
      TabIndex        =   3
      Top             =   540
      Width           =   2895
   End
   Begin DirListBox filDirLst 
      Height          =   1605
      Left            =   3600
      TabIndex        =   2
      Top             =   900
      Width           =   2895
   End
   Begin DriveListBox filDrvLst 
      Height          =   315
      Left            =   3600
      TabIndex        =   0
      Top             =   540
      Width           =   2895
   End
   Begin Label txtDstFil 
      BorderStyle     =   1  'Fixed Single
      Height          =   315
      Left            =   240
      TabIndex        =   10
      Top             =   3900
      Width           =   4815
   End
   Begin Label lblDstFil 
      Caption         =   "Destination File:"
      Height          =   255
      Left            =   240
      TabIndex        =   9
      Top             =   3600
      Width           =   2895
   End
   Begin Label lblSrcFil 
      Caption         =   "Source File:"
      Height          =   255
      Left            =   240
      TabIndex        =   4
      Top             =   2820
      Width           =   2895
   End
   Begin Label lblDrvDir 
      Caption         =   "&Drive && Directory:"
      Height          =   255
      Left            =   3600
      TabIndex        =   1
      Top             =   240
      Width           =   2895
   End
   Begin Label lblFilLst 
      Caption         =   "&Wave Files:"
      Height          =   255
      Left            =   240
      TabIndex        =   7
      Top             =   240
      Width           =   2895
   End
   Begin Label txtSrcFil 
      BorderStyle     =   1  'Fixed Single
      Height          =   315
      Left            =   240
      TabIndex        =   5
      Top             =   3120
      Width           =   4815
   End
End
Option Explicit

' ------------------------------------------------------------------------
' Indexed File Test: IdxTst.c
' Copyright (c) 1987-1996 Andrew J. Michalik
'
' You have a royalty-free right to use, modify, reproduce and
' distribute the Sample Files (and/or any modified version) in any
' way you find useful, provided that you agree that VISI has no
' warranty obligations or liability for any Sample Application Files
' which are modified.
'
' ------------------------------------------------------------------------

' ------------------------------------------------------------------------
' ------------------------------------------------------------------------
Dim cbIdxBlk As IDXBLK              ' Function Communication block

Dim mhNulMem As VISMEMHDL           ' VIS memory handle (=0&)
Dim lpNulPol As IDXPOLPRC           ' Idx poll procedure address (=0&)
Dim lpNulMsg As IDXERRPRC           ' Idx msg  procedure address (=0&)

Sub btnExit_Click ()
    End
End Sub

Sub btnFilCvt_Click ()
    Dim usRetCod As Integer
    ' Convert the selected .Wav file.
    usRetCod = CvtWavFil(txtSrcFil.Caption, txtDstFil.Caption)

End Sub

Function CvtWavFil (ByVal szSrcFil As String, ByVal szDstFil As String) As Integer
    Dim usSrcHdl As Integer
    Dim usDstHdl As Integer
    Dim ulOutCnt As Long
      
    ' Open source and destination files
    Open szSrcFil For Binary Access Read As #1
    usSrcHdl = FileAttr(1, 2)

    Open szDstFil For Binary Access Write As #2
    usDstHdl = FileAttr(2, 2)

'ajm
'    ' Convert the file
'    ulOutCnt = CvtPCMFil(usSrcHdl, usDstHdl, "", "", 0&, 0, cbIdxBlk)
'    MsgBox (Str$(ulOutCnt) + " bytes converted")
    
    ' Close source and destination files
    Close #1
    Close #2

End Function

Sub filDirLst_Change ()
    ' When directory is changed, update path in Files control.
    filFilLst.Path = filDirLst.Path

End Sub

Sub filDrvLst_Change ()
    ' When drive is changed, update directory control.
    ' (This also causes a Change event for the directory control).
    filDirLst.Path = filDrvLst.Drive

End Sub

Sub filFilLst_DblClick ()
    ' When at the root level (for example, C:\) the Path property
    ' has a backslash (\) at the end.  When at any other level,
    ' there is no final \.  This code handles either case to build
    ' the complete path and filename of the selected file.
    If Right(filFilLst.Path, 1) <> "\" Then
        txtSrcFil.Caption = filFilLst.Path & "\" & filFilLst.FileName
    Else
        txtSrcFil.Caption = filFilLst.Path & filFilLst.FileName
    End If

    ' Set the destination file name
    txtDstFil.Caption = Left(txtSrcFil.Caption, Len(txtSrcFil.Caption) - 3) + "vox"
    
    ' Enable convertor
    btnFilCvt.Enabled = True

End Sub

Sub Form_Load ()
    Dim usRetCod As Integer

    ' Set drive and path for controls to drive
    ' and directory where this application is located.
    filDrvLst.Drive = App.Path
    filDirLst.Path = App.Path

    ' Check the version numbers of support modules (Win DLL's)
    If (IDXVERNUM <> IdxFilVer()) Then
        MsgBox ("Wrong Library Version Number!\n")
        End
    End If

    ' Initialize function communication block
    usRetCod = IdxCfgIni(cbIdxBlk)

    ' Initialize default conversion block parameters
    ' (.Wav to .Vox)
'    cbIdxBlk.usSrcFmt = WAVHDRFMT           ' Source file format
'    cbIdxBlk.usSrcPCM = MPCPCM016           ' Source PCM type
'    cbIdxBlk.usSrcChn = 1                   ' Source file chan count
'    cbIdxBlk.usSrcBIO = 0                   ' Source bit encoding
'    cbIdxBlk.ulSrcFrq = 11025&              ' Source sample frequency
'    cbIdxBlk.ulSrcOff = 0&                  ' Source byte offset
'    cbIdxBlk.ulSrcLen = &HFFFFFFFF          ' Source byte length
'    cbIdxBlk.mhSrcCfg = mhNulMem            ' Source cfg memory buff
'    cbIdxBlk.usDstFmt = ALLPURFMT           ' Destination file format
'    cbIdxBlk.usDstPCM = DLGPCM004           ' Destination PCM type
'    cbIdxBlk.usDstChn = 1                   ' Destination file chan count
'    cbIdxBlk.usDstBIO = 0                   ' Destination bit encoding
'    cbIdxBlk.ulDstFrq = 6000&               ' Destination sample frequency
'    cbIdxBlk.ulDstOff = 0&                  ' Destination byte offset
'    cbIdxBlk.ulDstLen = &HFFFFFFFF          ' Destination byte length
'    cbIdxBlk.mhDstCfg = mhNulMem            ' Destination cfg memory buff
'    cbIdxBlk.flCrpGrd = CRPGRDDEF           ' Auto Crop guard time
'    cbIdxBlk.usTDLAAF = 2                   ' Time Domain Anti-alias
'    cbIdxBlk.bfTDLFst = False               ' Anti-alias fast math on/off
'    cbIdxBlk.bfFFTDTM = False               ' FFT DTMF filter   on/off
'    cbIdxBlk.bfFFTAAF = False               ' FFT Anti-aliasing on/off
'    cbIdxBlk.bfFFTFEq = False               ' FFT Freq Equalize on/off
'    cbIdxBlk.bfFFTRes = False               ' FFT Freq Resample on/off
'    cbIdxBlk.usFFTOrd = FFTORDDEF           ' FFT filter size
'    cbIdxBlk.flFEqGai = 0                   ' Freq equalizer gain adj (dB)
'    cbIdxBlk.ulFEqBot = FEQBOTDEF           ' Freq equalizer bot freq (Hz)
'    cbIdxBlk.ulFEqTop = FEQTOPDEF           ' Freq equalizer top freq (Hz)
'    cbIdxBlk.usFEqCnt = FEQCNTDEF           ' Freq equalizer point count
'    cbIdxBlk.flFEqPnt(1) = FEQVOX001        '  300- 800: +3dB (<300Hz:0dB)
'    cbIdxBlk.flFEqPnt(2) = FEQVOX002        '  800-1300: +6dB
'    cbIdxBlk.flFEqPnt(3) = FEQVOX003        ' 1300-1800: +6dB
'    cbIdxBlk.flFEqPnt(4) = FEQVOX004        ' 1800-2300: +6dB
'    cbIdxBlk.flFEqPnt(5) = FEQVOX005        ' 2300-2800: +6dB
'    cbIdxBlk.flFEqPnt(6) = FEQVOX006        ' 2800-3300: +6dB
'    cbIdxBlk.flFEqPnt(7) = FEQVOX007        ' 3300-3800: +3dB (>3.8Kz:0dB)
'    cbIdxBlk.mhGloCfg = mhNulMem            ' Global cfg memory buff
'    cbIdxBlk.lpPolDsp = lpNulPol            ' Conversion poll display proc
'    cbIdxBlk.ulRsvPol = 0&                  ' Reserved
'    cbIdxBlk.lpMsgDsp = lpNulMsg            ' Reserved
'    cbIdxBlk.ulRsvMsg = 0&                  ' Reserved
    
    ' Initialize conversion DLL support
    usRetCod = IdxFilIni(0, 0&, 0&)
    If (SI_IDXNO_ERR <> usRetCod) Then
      Select Case usRetCod
        Case SI_IDXKEYERR
            MsgBox ("Key/License error, 8 second limit")
        Case SI_IDXVERERR
            MsgBox ("Support software module version mismatch, continuing...")
        Case SI_IDXVERERR Or SI_IDXVERERR
            MsgBox ("Key/License error, 8 second limit")
            MsgBox ("Support software module version mismatch, continuing...")
        Case Else
            MsgBox ("Unknown Error!")
            End
      End Select
    End If
    
End Sub

Sub Form_Unload (Cancel As Integer)
    Dim usRetCod As Integer
    usRetCod = IdxFilTrm()
End Sub

