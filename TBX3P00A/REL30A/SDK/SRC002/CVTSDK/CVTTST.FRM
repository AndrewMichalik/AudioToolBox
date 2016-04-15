VERSION 2.00
Begin Form Form1 
   Caption         =   "Wave Convertor"
   ClientHeight    =   4452
   ClientLeft      =   1080
   ClientTop       =   1668
   ClientWidth     =   6720
   ClipControls    =   0   'False
   Height          =   4824
   Icon            =   CVTTST.FRX:0000
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
' PCM Convertor Test: CvtTst.c
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
Dim cbCvtBlk As CVTBLK              ' Function Communication block

Dim mhNulMem As VISMEMHDL           ' VIS memory handle (=0&)
Dim lpNulPol As CVTPOLPRC           ' Cvt poll procedure address (=0&)
Dim lpNulMsg As CVTERRPRC           ' Cvt msg  procedure address (=0&)

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

    ' Convert the file
    ulOutCnt = CvtPCMFil(usSrcHdl, usDstHdl, "", "", 0&, 0, cbCvtBlk)
    MsgBox (Str$(ulOutCnt) + " bytes converted")
    
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
    If (CVTVERNUM <> CvtPCMVer()) Then
        MsgBox ("Wrong Library Version Number!\n")
        End
    End If

    ' Initialize function communication block
    usRetCod = CvtCfgIni(cbCvtBlk)

    ' Initialize default conversion block parameters
    ' (.Wav to .Vox)
    cbCvtBlk.usSrcFmt = WAVHDRFMT           ' Source file format
    cbCvtBlk.usSrcPCM = MPCPCM016           ' Source PCM type
    cbCvtBlk.usSrcChn = 1                   ' Source file chan count
    cbCvtBlk.usSrcBIO = 0                   ' Source bit encoding
    cbCvtBlk.ulSrcFrq = 11025&              ' Source sample frequency
    cbCvtBlk.ulSrcOff = 0&                  ' Source byte offset
    cbCvtBlk.ulSrcLen = &HFFFFFFFF          ' Source byte length
    cbCvtBlk.mhSrcCfg = mhNulMem            ' Source cfg memory buff
    cbCvtBlk.usDstFmt = ALLPURFMT           ' Destination file format
    cbCvtBlk.usDstPCM = DLGPCM004           ' Destination PCM type
    cbCvtBlk.usDstChn = 1                   ' Destination file chan count
    cbCvtBlk.usDstBIO = 0                   ' Destination bit encoding
    cbCvtBlk.ulDstFrq = 6000&               ' Destination sample frequency
    cbCvtBlk.ulDstOff = 0&                  ' Destination byte offset
    cbCvtBlk.ulDstLen = &HFFFFFFFF          ' Destination byte length
    cbCvtBlk.mhDstCfg = mhNulMem            ' Destination cfg memory buff
    cbCvtBlk.flCrpGrd = CRPGRDDEF           ' Auto Crop guard time
    cbCvtBlk.usTDLAAF = 2                   ' Time Domain Anti-alias
    cbCvtBlk.bfTDLFst = False               ' Anti-alias fast math on/off
    cbCvtBlk.bfFFTDTM = False               ' FFT DTMF filter   on/off
    cbCvtBlk.bfFFTAAF = False               ' FFT Anti-aliasing on/off
    cbCvtBlk.bfFFTFEq = False               ' FFT Freq Equalize on/off
    cbCvtBlk.bfFFTRes = False               ' FFT Freq Resample on/off
    cbCvtBlk.usFFTOrd = FFTORDDEF           ' FFT filter size
    cbCvtBlk.flFEqGai = 0                   ' Freq equalizer gain adj (dB)
    cbCvtBlk.ulFEqBot = FEQBOTDEF           ' Freq equalizer bot freq (Hz)
    cbCvtBlk.ulFEqTop = FEQTOPDEF           ' Freq equalizer top freq (Hz)
    cbCvtBlk.usFEqCnt = FEQCNTDEF           ' Freq equalizer point count
    cbCvtBlk.flFEqPnt(1) = FEQVOX001        '  300- 800: +3dB (<300Hz:0dB)
    cbCvtBlk.flFEqPnt(2) = FEQVOX002        '  800-1300: +6dB
    cbCvtBlk.flFEqPnt(3) = FEQVOX003        ' 1300-1800: +6dB
    cbCvtBlk.flFEqPnt(4) = FEQVOX004        ' 1800-2300: +6dB
    cbCvtBlk.flFEqPnt(5) = FEQVOX005        ' 2300-2800: +6dB
    cbCvtBlk.flFEqPnt(6) = FEQVOX006        ' 2800-3300: +6dB
    cbCvtBlk.flFEqPnt(7) = FEQVOX007        ' 3300-3800: +3dB (>3.8Kz:0dB)
    cbCvtBlk.mhGloCfg = mhNulMem            ' Global cfg memory buff
    cbCvtBlk.lpPolDsp = lpNulPol            ' Conversion poll display proc
    cbCvtBlk.ulRsvPol = 0&                  ' Reserved
    cbCvtBlk.lpMsgDsp = lpNulMsg            ' Reserved
    cbCvtBlk.ulRsvMsg = 0&                  ' Reserved
    
'    ' Initialize default conversion block parameters
'    ' (.Vox to .Wav)
'    cbCvtBlk.usSrcFmt = ALLPURFMT           ' Destination file format
'    cbCvtBlk.usSrcPCM = DLGPCM004           ' Destination PCM type
'    cbCvtBlk.usSrcChn = 1                   ' Destination file chan count
'    cbCvtBlk.usSrcBIO = 0                   ' Destination bit encoding
'    cbCvtBlk.ulSrcFrq = 6000&               ' Destination sample frequency
'    cbCvtBlk.ulSrcOff = 0&                  ' Destination byte offset
'    cbCvtBlk.ulSrcLen = &HFFFFFFFF          ' Destination byte length
'    cbCvtBlk.mhSrcCfg = mhNulMem            ' Destination cfg memory buff
'    cbCvtBlk.usDstFmt = WAVHDRFMT           ' Source file format
'    cbCvtBlk.usDstPCM = MPCPCM016           ' Source PCM type
'    cbCvtBlk.usDstChn = 1                   ' Source file chan count
'    cbCvtBlk.usDstBIO = 0                   ' Source bit encoding
'    cbCvtBlk.ulDstFrq = 11025&              ' Source sample frequency
'    cbCvtBlk.ulDstOff = 0&                  ' Source byte offset
'    cbCvtBlk.ulDstLen = &HFFFFFFFF          ' Source byte length
'    cbCvtBlk.mhDstCfg = mhNulMem            ' Source cfg memory buff

    ' Initialize conversion DLL support
    usRetCod = CvtPCMIni(0, 0&, 0&)
    If (SI_CVTNO_ERR <> usRetCod) Then
      Select Case usRetCod
        Case SI_CVTKEYERR
            MsgBox ("Key/License error, 8 second limit")
        Case SI_CVTVERERR
            MsgBox ("Support software module version mismatch, continuing...")
        Case SI_CVTVERERR Or SI_CVTVERERR
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
    usRetCod = CvtPCMTrm()
End Sub

