VERSION 2.00
Begin Form frmAudTBx 
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   3  'Fixed Double
   Caption         =   "Audio ToolBox ® Apprentice"
   ClientHeight    =   6000
   ClientLeft      =   2760
   ClientTop       =   3756
   ClientWidth     =   7788
   Height          =   6696
   Icon            =   AUDTBX.FRX:0000
   Left            =   2736
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   ScaleHeight     =   6000
   ScaleWidth      =   7788
   Top             =   3084
   Visible         =   0   'False
   Width           =   7836
   Begin CommandButton cmdBrowse 
      Caption         =   "&Browse..."
      Enabled         =   0   'False
      Height          =   375
      Left            =   6240
      TabIndex        =   2
      TabStop         =   0   'False
      Top             =   840
      Width           =   1332
   End
   Begin CommandButton cmdHelp 
      Caption         =   "&Help"
      Height          =   372
      Left            =   6240
      TabIndex        =   109
      Top             =   1320
      Width           =   1332
   End
   Begin CommandButton cmdExit 
      Caption         =   "E&xit"
      Height          =   372
      Left            =   6240
      TabIndex        =   108
      Top             =   1800
      Width           =   1332
   End
   Begin CommonDialog dlgCom 
      Left            =   7320
      Top             =   120
   End
   Begin MhTab tabAudTbx 
      AssociateChildren=   -1  'True
      BorderColor     =   &H80000006&
      BorderStyle     =   1  'Single Line
      FillColor       =   &H8000000F&
      Folders         =   6
      Height          =   4575
      Left            =   120
      LightColor      =   &H80000005&
      PageBevelSize   =   AUDTBX.FRX:0302
      PictureVAlignment=   2  'Center
      ShadowColor     =   &H80000010&
      TabBorderStyle  =   1  'Rounded
      TabCaption      =   AUDTBX.FRX:0DD4
      TabFormat       =   AUDTBX.FRX:0E29
      TabIndex        =   0
      TabOffset       =   12
      TabsPerRow      =   3
      Top             =   120
      VAlignment      =   2  'Center
      Width           =   7575
      Begin CommandButton cmdCvtEqu 
         Caption         =   "E&qualize..."
         Height          =   375
         Left            =   6120
         TabIndex        =   25
         Top             =   3480
         Width           =   1332
      End
      Begin SSFrame grpIXpFtr 
         Caption         =   "Filters"
         ForeColor       =   &H00000000&
         Height          =   1335
         Left            =   3600
         TabIndex        =   32
         Top             =   120
         Width           =   2415
         Begin ListBox lstCvtFtr 
            BackColor       =   &H00FFFFFF&
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   792
            Left            =   120
            TabIndex        =   16
            Top             =   360
            Width           =   2172
         End
      End
      Begin SSFrame grpCvtOpt 
         Caption         =   "Options"
         ForeColor       =   &H00000000&
         Height          =   1815
         Left            =   3600
         TabIndex        =   33
         Top             =   1560
         Width           =   3855
         Begin CheckBox chkCvtDCE 
            BackColor       =   &H00C0C0C0&
            Caption         =   "Dynamic Comp/Expand (dB):"
            Height          =   252
            Left            =   120
            TabIndex        =   21
            Tag             =   "Audio Conversion, Dynamic Compressor / Expander"
            Top             =   1080
            Width           =   2772
         End
         Begin TextBox edtCvtDCE 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   3000
            TabIndex        =   22
            Tag             =   "Audio Conversion, Dynamic Compress / Expand Level"
            Text            =   "6"
            Top             =   1080
            Width           =   735
         End
         Begin CheckBox chkCvtNrm 
            BackColor       =   &H00C0C0C0&
            Caption         =   "Normalize (%over range):"
            Height          =   252
            Left            =   120
            TabIndex        =   19
            Tag             =   "Audio Conversion, Normalize"
            Top             =   720
            Width           =   2772
         End
         Begin TextBox edtCvtNrm 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   3000
            TabIndex        =   20
            Tag             =   "Audio Conversion, Percent Over Range"
            Text            =   "1"
            Top             =   720
            Width           =   735
         End
         Begin TextBox edtCvtVol 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   3000
            TabIndex        =   23
            Tag             =   "Audio Conversion, Volume Adjust"
            Text            =   "+0"
            Top             =   1440
            Width           =   735
         End
         Begin TextBox edtCvtCrp 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   3000
            TabIndex        =   18
            Tag             =   "Audio Conversion, Guard Time"
            Text            =   ".05"
            Top             =   360
            Width           =   735
         End
         Begin CheckBox chkCvtCrp 
            BackColor       =   &H00C0C0C0&
            Caption         =   "Auto-Crop (guard time in sec):"
            Height          =   252
            Left            =   120
            TabIndex        =   17
            Tag             =   "Audio Conversion, Auto-Crop"
            Top             =   360
            Width           =   2772
         End
         Begin Label lblCvtVol 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Volume Adjust (dB):"
            Height          =   192
            Left            =   120
            TabIndex        =   35
            Top             =   1440
            Width           =   1644
         End
      End
      Begin SSFrame grpCvtDst 
         ForeColor       =   &H00000000&
         Height          =   1815
         Left            =   120
         TabIndex        =   28
         Top             =   1920
         Width           =   3375
         Begin CheckBox chkCvtBak 
            BackColor       =   &H00C0C0C0&
            Caption         =   "Backup"
            Height          =   252
            Left            =   2280
            TabIndex        =   12
            Tag             =   "Audio Conversion, Backup"
            Top             =   195
            Value           =   1  'Checked
            Width           =   972
         End
         Begin TextBox edtCvtDstFrq 
            Alignment       =   2  'Center
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   288
            Left            =   2040
            TabIndex        =   15
            Tag             =   "Audio Conversion, Destination Frequency"
            Text            =   "default"
            Top             =   1440
            Width           =   1212
         End
         Begin ComboBox cboCvtDstDat 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   300
            Left            =   120
            Style           =   2  'Dropdown List
            TabIndex        =   14
            Tag             =   "Audio Conversion, Destination Data Type"
            Top             =   1080
            Width           =   3135
         End
         Begin TextBox edtCvtDst 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   120
            TabIndex        =   13
            Tag             =   "Audio Conversion, Destination Audio File"
            Text            =   "*.vox"
            Top             =   480
            Width           =   3135
         End
         Begin Label lblCvtDstFrq 
            Alignment       =   1  'Right Justify
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Frequency (kHz):"
            Height          =   192
            Left            =   504
            TabIndex        =   31
            Top             =   1440
            Width           =   1428
         End
         Begin Label lblCvtDstDat 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "File Data Type:"
            Height          =   192
            Left            =   120
            TabIndex        =   30
            Top             =   840
            Width           =   1296
         End
         Begin Label lblCvtDst 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Destination Audio File:"
            Height          =   192
            Left            =   120
            TabIndex        =   29
            Top             =   240
            Width           =   1884
         End
      End
      Begin SSFrame grpCvtSrc 
         Height          =   1815
         Left            =   120
         TabIndex        =   7
         Top             =   120
         Width           =   3375
         Begin TextBox edtCvtSrc 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   120
            TabIndex        =   9
            Tag             =   "Audio Conversion, Source Audio File"
            Text            =   "*.wav"
            Top             =   480
            Width           =   3135
         End
         Begin ComboBox cboCvtSrcDat 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   300
            Left            =   120
            Style           =   2  'Dropdown List
            TabIndex        =   10
            Tag             =   "Audio Conversion, Source Data Type"
            Top             =   1080
            Width           =   3135
         End
         Begin TextBox edtCvtSrcFrq 
            Alignment       =   2  'Center
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   288
            Left            =   2040
            TabIndex        =   11
            Tag             =   "Audio Conversion, Source Frequency"
            Text            =   "default"
            Top             =   1440
            Width           =   1212
         End
         Begin Label lblCvtSrc 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Source Audio File:"
            Height          =   195
            Left            =   120
            TabIndex        =   8
            Top             =   240
            Width           =   1530
         End
         Begin Label lblCvtSrcDat 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "File Data Type:"
            Height          =   192
            Left            =   120
            TabIndex        =   26
            Top             =   840
            Width           =   1296
         End
         Begin Label lblCvtSrcFrq 
            Alignment       =   1  'Right Justify
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Frequency (kHz):"
            Height          =   192
            Left            =   120
            TabIndex        =   27
            Top             =   1440
            Width           =   1812
         End
      End
      Begin SSFrame grpRebRep 
         Caption         =   "Insert / Delete / Replace"
         ForeColor       =   &H00000000&
         Height          =   1935
         Left            =   -74880
         TabIndex        =   99
         Top             =   1800
         Width           =   7335
         Begin SSFrame grpRebBeg 
            ForeColor       =   &H00000000&
            Height          =   612
            Left            =   120
            TabIndex        =   100
            Top             =   240
            Width           =   2532
            Begin TextBox edtRebBeg 
               FontBold        =   0   'False
               FontItalic      =   0   'False
               FontName        =   "MS Sans Serif"
               FontSize        =   7.8
               FontStrikethru  =   0   'False
               FontUnderline   =   0   'False
               Height          =   288
               Left            =   1680
               TabIndex        =   91
               Tag             =   "Index Rebuild, Begin at Index #"
               Text            =   "1"
               Top             =   240
               Width           =   732
            End
            Begin Label lblRebBeg 
               AutoSize        =   -1  'True
               BackColor       =   &H00C0C0C0&
               Caption         =   "Begin at Index#:"
               Height          =   192
               Left            =   120
               TabIndex        =   101
               Top             =   240
               Width           =   1332
            End
         End
         Begin SSFrame grpRebRepFil 
            ForeColor       =   &H00000000&
            Height          =   1575
            Left            =   2760
            TabIndex        =   105
            Top             =   240
            Width           =   4455
            Begin TextBox edtRebTxt 
               FontBold        =   0   'False
               FontItalic      =   0   'False
               FontName        =   "MS Sans Serif"
               FontSize        =   7.8
               FontStrikethru  =   0   'False
               FontUnderline   =   0   'False
               Height          =   285
               Left            =   120
               TabIndex        =   95
               Tag             =   "Index Rebuild, Replace Text with File"
               Top             =   1080
               Width           =   4215
            End
            Begin TextBox edtRebVox 
               FontBold        =   0   'False
               FontItalic      =   0   'False
               FontName        =   "MS Sans Serif"
               FontSize        =   7.8
               FontStrikethru  =   0   'False
               FontUnderline   =   0   'False
               Height          =   285
               Left            =   120
               TabIndex        =   94
               Tag             =   "Index Rebuild, Replace Vox with File"
               Top             =   480
               Width           =   4215
            End
            Begin Label lblRebTxt 
               AutoSize        =   -1  'True
               BackColor       =   &H00C0C0C0&
               Caption         =   "Replace Text with File:"
               Height          =   192
               Left            =   120
               TabIndex        =   107
               Top             =   840
               Width           =   1896
            End
            Begin Label lblRebVox 
               AutoSize        =   -1  'True
               BackColor       =   &H00C0C0C0&
               Caption         =   "Replace Vox with File:"
               Height          =   192
               Left            =   120
               TabIndex        =   106
               Top             =   240
               Width           =   1848
            End
         End
         Begin SSFrame grpRebInsDel 
            ForeColor       =   &H00000000&
            Height          =   972
            Left            =   120
            TabIndex        =   102
            Top             =   840
            Width           =   2532
            Begin TextBox edtRebDel 
               FontBold        =   0   'False
               FontItalic      =   0   'False
               FontName        =   "MS Sans Serif"
               FontSize        =   7.8
               FontStrikethru  =   0   'False
               FontUnderline   =   0   'False
               Height          =   288
               Left            =   1680
               TabIndex        =   93
               Tag             =   "Index Rebuild, Delete Count"
               Text            =   "0"
               Top             =   600
               Width           =   732
            End
            Begin TextBox edtRebIns 
               FontBold        =   0   'False
               FontItalic      =   0   'False
               FontName        =   "MS Sans Serif"
               FontSize        =   7.8
               FontStrikethru  =   0   'False
               FontUnderline   =   0   'False
               Height          =   288
               Left            =   1680
               TabIndex        =   92
               Tag             =   "Index Rebuild, Insert Count"
               Text            =   "0"
               Top             =   240
               Width           =   732
            End
            Begin Label lblRebDel 
               AutoSize        =   -1  'True
               BackColor       =   &H00C0C0C0&
               Caption         =   "Delete Count:"
               Height          =   192
               Left            =   120
               TabIndex        =   104
               Top             =   600
               Width           =   1128
            End
            Begin Label lblRebIns 
               AutoSize        =   -1  'True
               BackColor       =   &H00C0C0C0&
               Caption         =   "Insert Count:"
               Height          =   192
               Left            =   120
               TabIndex        =   103
               Top             =   240
               Width           =   1032
            End
         End
      End
      Begin SSFrame grpLstFil 
         ForeColor       =   &H00000000&
         Height          =   975
         Left            =   -74880
         TabIndex        =   66
         Top             =   120
         Width           =   3375
         Begin TextBox edtLstSrc 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   120
            TabIndex        =   68
            Tag             =   "Index List, Source Index File"
            Text            =   "*.vap"
            Top             =   480
            Width           =   3135
         End
         Begin Label lblLstSrc 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Source Index File:"
            Height          =   192
            Left            =   120
            TabIndex        =   67
            Top             =   240
            Width           =   1500
         End
      End
      Begin SSFrame grpLstOpt 
         Caption         =   "Options"
         ForeColor       =   &H00000000&
         Height          =   1455
         Left            =   -74880
         TabIndex        =   69
         Top             =   1200
         Width           =   3375
         Begin CheckBox chkLstHdr 
            BackColor       =   &H00C0C0C0&
            Caption         =   "Header Information Inhibit"
            Height          =   252
            Left            =   120
            TabIndex        =   70
            Tag             =   "Index List, Header Information Inhibit"
            Top             =   360
            Value           =   1  'Checked
            Width           =   3132
         End
         Begin CheckBox chkLstTxt 
            BackColor       =   &H00C0C0C0&
            Caption         =   "Text Listing Inhibit"
            Height          =   252
            Left            =   120
            TabIndex        =   71
            Tag             =   "Index List, Text Listing Inhibit"
            Top             =   720
            Value           =   1  'Checked
            Width           =   3132
         End
         Begin CheckBox chkLstAud 
            BackColor       =   &H00C0C0C0&
            Caption         =   "Audio Information Inhibit"
            Height          =   252
            Left            =   120
            TabIndex        =   72
            Tag             =   "Index List, Audio Information Inhibit"
            Top             =   1080
            Value           =   1  'Checked
            Width           =   3132
         End
      End
      Begin SSFrame grpRebOpt 
         Caption         =   "Options"
         ForeColor       =   &H00000000&
         Height          =   972
         Left            =   -71400
         TabIndex        =   97
         Tag             =   "Index Rebuild, Frequency"
         Top             =   120
         Width           =   2412
         Begin TextBox edtRebFrq 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   288
            Left            =   1560
            TabIndex        =   90
            Tag             =   "Index Rebuild, Frequency"
            Text            =   "6000"
            Top             =   360
            Width           =   732
         End
         Begin Label lblRebFrq 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Frequency (Hz):"
            Height          =   192
            Left            =   120
            TabIndex        =   98
            Top             =   360
            Width           =   1320
         End
      End
      Begin SSFrame grpRebSrc 
         ForeColor       =   &H00000000&
         Height          =   1575
         Left            =   -74880
         TabIndex        =   85
         Top             =   120
         Width           =   3375
         Begin CheckBox chkRebBak 
            BackColor       =   &H00C0C0C0&
            Caption         =   "Backup"
            Height          =   252
            Left            =   2280
            TabIndex        =   88
            Tag             =   "Index Rebuild, Backup"
            Top             =   915
            Value           =   1  'Checked
            Width           =   972
         End
         Begin TextBox edtRebDst 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   120
            TabIndex        =   89
            Tag             =   "Index Rebuild, Destination Index File"
            Top             =   1200
            Width           =   3135
         End
         Begin TextBox edtRebSrc 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   120
            TabIndex        =   87
            Tag             =   "Index Rebuild, Source Index File"
            Text            =   "*.vap"
            Top             =   480
            Width           =   3135
         End
         Begin Label lblRebDst 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "&Destination Index File:"
            Height          =   195
            Left            =   120
            TabIndex        =   96
            Top             =   960
            Width           =   1845
         End
         Begin Label lblRebSrc 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Source Index File:"
            Height          =   192
            Left            =   120
            TabIndex        =   86
            Top             =   240
            Width           =   1500
         End
      End
      Begin SSFrame grpChpFil 
         ForeColor       =   &H00000000&
         Height          =   975
         Left            =   -74880
         TabIndex        =   52
         Top             =   120
         Width           =   3375
         Begin TextBox edtChpSrc 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   120
            TabIndex        =   54
            Tag             =   "Index Chop, Source Index File"
            Text            =   "*.vap"
            Top             =   480
            Width           =   3135
         End
         Begin Label lblChpSrc 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Source Index File:"
            Height          =   192
            Left            =   120
            TabIndex        =   53
            Top             =   240
            Width           =   1500
         End
      End
      Begin SSFrame grpChpOpt 
         Caption         =   "Options"
         ForeColor       =   &H00000000&
         Height          =   2295
         Left            =   -74880
         TabIndex        =   60
         Top             =   1200
         Width           =   3375
         Begin TextBox edtChpTxt 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   1920
            TabIndex        =   56
            Tag             =   "Index Chop, Text Extension"
            Text            =   ".txt"
            Top             =   720
            Width           =   735
         End
         Begin TextBox edtChpVox 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   1920
            TabIndex        =   55
            Tag             =   "Index Chop, Vox Extension"
            Text            =   ".vox"
            Top             =   360
            Width           =   735
         End
         Begin TextBox edtChpCnt 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   1920
            TabIndex        =   58
            Tag             =   "Index Chop, Number of Indexes"
            Text            =   "all"
            Top             =   1440
            Width           =   735
         End
         Begin TextBox edtChpOff 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   1920
            TabIndex        =   57
            Tag             =   "Index Chop, File Name Offset"
            Text            =   "0"
            Top             =   1080
            Width           =   735
         End
         Begin TextBox edtChpBeg 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   1920
            TabIndex        =   59
            Tag             =   "Index Chop, Begin at Index#"
            Text            =   "1"
            Top             =   1800
            Width           =   735
         End
         Begin Label lblChpOff 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "File Name Offset:"
            Height          =   192
            Left            =   120
            TabIndex        =   63
            Top             =   1080
            Width           =   1440
         End
         Begin Label lblChpCnt 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Number of Indexes:"
            Height          =   192
            Left            =   120
            TabIndex        =   64
            Top             =   1440
            Width           =   1608
         End
         Begin Label lblChpBeg 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Begin at Index#:"
            Height          =   192
            Left            =   120
            TabIndex        =   65
            Top             =   1800
            Width           =   1332
         End
         Begin Label lblChpTxt 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Text Extension:"
            Height          =   192
            Left            =   120
            TabIndex        =   62
            Top             =   720
            Width           =   1272
         End
         Begin Label lblChpVox 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Vox Extension:"
            Height          =   192
            Left            =   120
            TabIndex        =   61
            Top             =   360
            Width           =   1224
         End
      End
      Begin SSFrame grpPakOpt 
         Caption         =   "Options"
         ForeColor       =   &H00000000&
         Height          =   1575
         Left            =   -74880
         TabIndex        =   78
         Top             =   1920
         Width           =   3375
         Begin TextBox edtPakFrq 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   288
            Left            =   1680
            TabIndex        =   80
            Tag             =   "Index Pack, Frequency"
            Text            =   "6000"
            Top             =   360
            Width           =   1572
         End
         Begin ComboBox cboPakSrt 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   288
            Left            =   1680
            Style           =   2  'Dropdown List
            TabIndex        =   82
            Tag             =   "Index Pack, Sort Order"
            Top             =   720
            Width           =   1572
         End
         Begin ComboBox cboPakTxt 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   288
            Left            =   1680
            TabIndex        =   84
            Tag             =   "Index Pack, Text Source"
            Text            =   "none"
            Top             =   1080
            Width           =   1572
         End
         Begin Label lblPakFrq 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Frequency (Hz):"
            Height          =   192
            Left            =   120
            TabIndex        =   79
            Top             =   360
            Width           =   1320
         End
         Begin Label lblPakTxt 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Text Source:"
            Height          =   192
            Left            =   120
            TabIndex        =   83
            Top             =   1080
            Width           =   1056
         End
         Begin Label lblPakSrt 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Sort Order:"
            Height          =   192
            Left            =   120
            TabIndex        =   81
            Top             =   720
            Width           =   912
         End
      End
      Begin SSFrame grpPakFil 
         ForeColor       =   &H00000000&
         Height          =   1695
         Left            =   -74880
         TabIndex        =   73
         Top             =   120
         Width           =   3375
         Begin TextBox edtPakDst 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   120
            TabIndex        =   77
            Tag             =   "Index Pack, Destination Index File"
            Text            =   "index.vap"
            Top             =   1200
            Width           =   3135
         End
         Begin TextBox edtPakSrc 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   120
            TabIndex        =   75
            Tag             =   "Index Pack, Source Vox File"
            Text            =   "*.vox"
            Top             =   480
            Width           =   3135
         End
         Begin Label lblPakSrc 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Source Vox File:"
            Height          =   192
            Left            =   120
            TabIndex        =   74
            Top             =   240
            Width           =   1368
         End
         Begin Label lblPakDst 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Destination Index File:"
            Height          =   192
            Left            =   120
            TabIndex        =   76
            Top             =   960
            Width           =   1848
         End
      End
      Begin SSFrame grpSndOpt 
         Caption         =   "Options"
         ForeColor       =   &H00000000&
         Height          =   1815
         Left            =   -74880
         TabIndex        =   47
         Top             =   1920
         Width           =   3375
         Begin TextBox edtSndOff 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   2520
            TabIndex        =   43
            Tag             =   "Index Chop, File Name Offset"
            Text            =   "0"
            Top             =   1080
            Width           =   735
         End
         Begin TextBox edtSndThr 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   2520
            TabIndex        =   44
            Tag             =   "Sound Chop, Sound Threshold"
            Text            =   "2"
            Top             =   1440
            Width           =   735
         End
         Begin TextBox edtSndExt 
            Alignment       =   2  'Center
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   2520
            TabIndex        =   41
            Tag             =   "Sound Chop, Destination File Extension"
            Text            =   ".vox"
            Top             =   360
            Width           =   735
         End
         Begin TextBox edtSndGrd 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   2520
            TabIndex        =   42
            Tag             =   "Sound Chop, Guard Time"
            Text            =   ".05"
            Top             =   720
            Width           =   735
         End
         Begin Label lblSndOff 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "File Name Offset:"
            Height          =   192
            Left            =   120
            TabIndex        =   50
            Top             =   1080
            Width           =   1440
         End
         Begin Label lblSndThr 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Sound Threshold (%max):"
            Height          =   192
            Left            =   120
            TabIndex        =   51
            Top             =   1440
            Width           =   2136
         End
         Begin Label lblSndExt 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Destination File Extension:"
            Height          =   195
            Left            =   120
            TabIndex        =   48
            Top             =   360
            Width           =   2295
         End
         Begin Label lblSndGrd 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Guard Time (sec):"
            Height          =   192
            Left            =   120
            TabIndex        =   49
            Top             =   720
            Width           =   1500
         End
      End
      Begin SSFrame grpSndSrc 
         ForeColor       =   &H00000000&
         Height          =   1815
         Left            =   -74880
         TabIndex        =   36
         Top             =   120
         Width           =   3375
         Begin TextBox edtSndFrq 
            Alignment       =   2  'Center
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   288
            Left            =   2040
            TabIndex        =   40
            Tag             =   "Sound Chop, Source Frequency"
            Text            =   "default"
            Top             =   1440
            Width           =   1212
         End
         Begin ComboBox cboSndDat 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   300
            Left            =   120
            Style           =   2  'Dropdown List
            TabIndex        =   39
            Tag             =   "Sound Chop, Source Data Type"
            Top             =   1080
            Width           =   3135
         End
         Begin TextBox edtSndSrc 
            FontBold        =   0   'False
            FontItalic      =   0   'False
            FontName        =   "MS Sans Serif"
            FontSize        =   7.8
            FontStrikethru  =   0   'False
            FontUnderline   =   0   'False
            Height          =   285
            Left            =   120
            TabIndex        =   38
            Tag             =   "Sound Chop, Source Audio File"
            Text            =   "*.wav"
            Top             =   480
            Width           =   3135
         End
         Begin Label lblSndFrq 
            Alignment       =   1  'Right Justify
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Frequency (kHz):"
            Height          =   192
            Left            =   120
            TabIndex        =   46
            Top             =   1440
            Width           =   1788
         End
         Begin Label lblSndDat 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "File Data Type:"
            Height          =   192
            Left            =   120
            TabIndex        =   45
            Top             =   840
            Width           =   1296
         End
         Begin Label lblSndSrc 
            AutoSize        =   -1  'True
            BackColor       =   &H00C0C0C0&
            Caption         =   "Source Audio File:"
            Height          =   192
            Left            =   120
            TabIndex        =   37
            Top             =   240
            Width           =   1536
         End
      End
      Begin CommandButton cmdCvtAdv 
         Caption         =   "&Advanced..."
         Height          =   375
         Left            =   4680
         TabIndex        =   24
         Top             =   3480
         Width           =   1332
      End
      Begin Label lblSmpRat 
         AutoSize        =   -1  'True
         BackColor       =   &H00C0C0C0&
         Caption         =   "Sampling Rate"
         Height          =   192
         Index           =   0
         Left            =   -1.5e5
         TabIndex        =   34
         Top             =   1800
         Width           =   1248
      End
   End
   Begin SSFrame grpCmdLin 
      Caption         =   "&Command line"
      ForeColor       =   &H00000000&
      Height          =   1212
      Left            =   120
      TabIndex        =   1
      Top             =   4680
      Width           =   7572
      Begin CommandButton cmdExecute 
         Caption         =   "&Execute"
         Height          =   372
         Left            =   6120
         TabIndex        =   4
         Top             =   240
         Width           =   1332
      End
      Begin CommandButton cmdMakBat 
         Caption         =   "&Make Batch..."
         Height          =   372
         Left            =   6120
         TabIndex        =   6
         Top             =   720
         Width           =   1332
      End
      Begin TextBox edtMakBat 
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   288
         Left            =   120
         TabIndex        =   5
         Tag             =   "Audio ToolBox, Command Line Batch File"
         Text            =   "*.bat"
         Top             =   720
         Width           =   5892
      End
      Begin TextBox edtCmdLin 
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "Courier New"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   288
         Left            =   120
         ScrollBars      =   1  'Horizontal
         TabIndex        =   3
         TabStop         =   0   'False
         Top             =   360
         Width           =   5892
      End
      Begin Label lblCurDir 
         Alignment       =   1  'Right Justify
         BackColor       =   &H00C0C0C0&
         Caption         =   "Current Directory"
         FontBold        =   0   'False
         FontItalic      =   0   'False
         FontName        =   "MS Sans Serif"
         FontSize        =   7.8
         FontStrikethru  =   0   'False
         FontUnderline   =   0   'False
         Height          =   192
         Left            =   1440
         TabIndex        =   110
         Top             =   120
         Width           =   4452
      End
   End
   Begin Menu mnFile 
      Caption         =   "&File"
      Begin Menu mnExit 
         Caption         =   "E&xit"
      End
   End
   Begin Menu mnTool 
      Caption         =   "&Tool"
      Begin Menu mnAudioConversion 
         Caption         =   "&Audio Conversion"
      End
      Begin Menu mnSoundChop 
         Caption         =   "&Sound Chop"
      End
      Begin Menu mnIndexChop 
         Caption         =   "Index &Chop"
      End
      Begin Menu mnIndexList 
         Caption         =   "Index &List"
      End
      Begin Menu mnIndexPack 
         Caption         =   "Index &Pack"
      End
      Begin Menu mnIndexRebuild 
         Caption         =   "Index &Rebuild"
      End
   End
   Begin Menu mnOptions 
      Caption         =   "&Options"
      Begin Menu mnEnvironment 
         Caption         =   "&Environment..."
      End
   End
   Begin Menu mnHelp 
      Caption         =   "&Help"
      Begin Menu mnHelpContents 
         Caption         =   "&Contents"
      End
      Begin Menu mnHSearch 
         Caption         =   "Search for Help On..."
      End
      Begin Menu mnTechSup 
         Caption         =   "Obtaining Technical Support..."
      End
      Begin Menu mnSep1 
         Caption         =   "-"
      End
      Begin Menu mnAbout 
         Caption         =   "&About ToolBox Apprentice..."
      End
   End
End
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'   Voice Information Systems (1-800-234-VISI) Audio ToolBox Digital Processor
'                   (c) Copyright Andrew J. Michalik 1987-1995
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Option Explicit


''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Dim cBrwActCtl        As Control         ' Previously active control
Dim iPrvSndSrcDat     As Integer         ' Previously Sound Chop source PCM index
Dim iPrvCvtSrcDat     As Integer         ' Previously Audio Convert source PCM index
Dim iPrvCvtDstDat     As Integer         ' Previously Audio Convert destination PCM index

Private Sub BldDatIdx (ttDocArr() As TBXDOCTYP, iIdxNum As Integer, sPCMTyp As String, sPCMDes As String)
  ttDocArr(iIdxNum).sPCMDes = sPCMDes
  ttDocArr(iIdxNum).sPCMTyp = sPCMTyp
End Sub

Sub BldDatInp ()
  ' Build data input selection array
  ' Contains all possible types, manufacturer based exceptions are handled elsewhere
  ReDim ttDocTypInp(10)
  
  ' Load PCM selection
  BldDatIdx ttDocTypInp(), 0, "WAV000", "Multimedia Wave"
  BldDatIdx ttDocTypInp(), 1, "DLG004", "Dialogic 4 bit (OKI)   (6 kHz)"
  BldDatIdx ttDocTypInp(), 2, "DLG008", "Dialogic 8 bit (u-Law) (6 kHz)"
  BldDatIdx ttDocTypInp(), 3, "G11F08", "CCITT G.711 Folded   u-Law 8 bit (8 kHz)"
  BldDatIdx ttDocTypInp(), 4, "G11I08", "CCITT G.711 Inverted A-law 8 bit (8 kHz)"
  BldDatIdx ttDocTypInp(), 5, "G21004", "CCITT G.721 4 bit (8 kHz)"
  BldDatIdx ttDocTypInp(), 6, "MPC008", "Multimedia unsigned 8 bit (11.025 kHz)"
  BldDatIdx ttDocTypInp(), 7, "MPC016", "Multimedia linear  16 bit (11.025 kHz)"
  BldDatIdx ttDocTypInp(), 8, "NWV001", "New Voice CVSD 1 bit (32 kHz)"
  BldDatIdx ttDocTypInp(), 9, "PTC001", "Perception Technology CVSD 1 bit (32 kHz)"
  BldDatIdx ttDocTypInp(), 10, "VIS016", "VIS Interchange Format 16 bit (11.025 kHz)"

End Sub

Sub BldDatOut ()
  ' Build data output selection array
  ' Contains all possible types, manufacturer based exceptions are handled elsewhere
  ReDim ttDocTypOut(11)
  
  ' Load PCM selection
  BldDatIdx ttDocTypOut(), 0, "DLG004", "Dialogic 4 bit (OKI)   (6 kHz)"
  BldDatIdx ttDocTypOut(), 1, "DLG008", "Dialogic 8 bit (u-Law) (6 kHz)"
  BldDatIdx ttDocTypOut(), 2, "G11F08", "CCITT G.711 Folded   u-Law 8 bit (8 kHz)"
  BldDatIdx ttDocTypOut(), 3, "G11I08", "CCITT G.711 Inverted A-law 8 bit (8 kHz)"
  BldDatIdx ttDocTypOut(), 4, "G21004", "CCITT G.721 4 bit (8 kHz)"
  BldDatIdx ttDocTypOut(), 5, "MPC008", "Multimedia unsigned 8 bit (11.025 kHz)"
  BldDatIdx ttDocTypOut(), 6, "MPC016", "Multimedia linear  16 bit (11.025 kHz)"
  BldDatIdx ttDocTypOut(), 7, "NWV001", "New Voice CVSD 1 bit (32 kHz)"
  BldDatIdx ttDocTypOut(), 8, "PTC001", "Perception Technology CVSD 1 bit (32 kHz)"
  BldDatIdx ttDocTypOut(), 9, "VIS016", "VIS Interchange Format 16 bit (11.025 kHz)"
  BldDatIdx ttDocTypOut(), 10, "WAV008", "Wave Multimedia Format  8 bit (11.025 kHz)"
  BldDatIdx ttDocTypOut(), 11, "WAV016", "Wave Multimedia Format 16 bit (11.025 kHz)"

End Sub

Sub cboCvtDstDat_Click ()
  CmdAudCvt
End Sub

Sub cboCvtDstDat_GotFocus ()
  iPrvCvtDstDat = cboCvtDstDat.ListIndex
End Sub

Sub cboCvtDstDat_LostFocus ()
  ' Force source frequency back to default if PCM changed
  If (iPrvCvtDstDat <> cboCvtDstDat.ListIndex) Then edtCvtDstFrq = OPTDEFDEF
End Sub

Sub cboCvtSrcDat_Click ()
  CmdAudCvt
End Sub

Sub cboCvtSrcDat_GotFocus ()
  iPrvCvtSrcDat = cboCvtSrcDat.ListIndex
End Sub

Sub cboCvtSrcDat_LostFocus ()
  ' Force source frequency back to default if PCM changed
  If (iPrvCvtSrcDat <> cboCvtSrcDat.ListIndex) Then edtCvtSrcFrq = OPTDEFDEF
End Sub

Sub cboPakSrt_Click ()
  CmdIdxPak
End Sub

Sub cboPakTxt_Change ()
  CmdIdxPak
End Sub

Sub cboPakTxt_Click ()
  CmdIdxPak
End Sub

Sub cboPakTxt_LostFocus ()
  If ("" = Trim(cboPakTxt)) Then cboPakTxt = OPTNONDEF
End Sub

Sub cboSndDat_Click ()
  CmdSndChp
End Sub

Sub cboSndDat_GotFocus ()
  iPrvSndSrcDat = cboSndDat.ListIndex
End Sub

Sub cboSndDat_LostFocus ()
  ' Force source frequency back to default if PCM changed
  If (iPrvSndSrcDat <> cboSndDat.ListIndex) Then edtSndFrq = OPTDEFDEF
End Sub

Sub chkCvtBak_Click ()
  CmdAudCvt
End Sub

Sub chkCvtCrp_Click ()
  CmdAudCvt
End Sub

Sub chkCvtDCE_Click ()
  CmdAudCvt
End Sub

Sub chkCvtNrm_Click ()
  CmdAudCvt
End Sub

Sub chkLstAud_Click ()
  CmdIdxLst
End Sub

Sub chkLstHdr_Click ()
  CmdIdxLst
End Sub

Sub chkLstTxt_Click ()
  CmdIdxLst
End Sub

Sub chkRebBak_Click ()
  CmdIdxReb
End Sub

Sub CmdAudCvt ()
  Dim sCmdLin As String
  
  ' Gray all options
  lblCvtSrcDat.ForeColor = lLblDisClr
  cboCvtSrcDat.ForeColor = lValDisClr
  lblCvtSrcFrq.ForeColor = lLblDisClr
  edtCvtSrcFrq.ForeColor = lValDisClr
  lblCvtDst.ForeColor = lLblDisClr
  edtCvtDst.ForeColor = lValDisClr
  chkCvtBak.ForeColor = lLblDisClr
  lblCvtDstDat.ForeColor = lLblDisClr
  cboCvtDstDat.ForeColor = lValDisClr
  lblCvtDstFrq.ForeColor = lLblDisClr
  edtCvtDstFrq.ForeColor = lValDisClr
  chkCvtCrp.ForeColor = lLblDisClr
  edtCvtCrp.ForeColor = lValDisClr
  chkCvtNrm.ForeColor = lLblDisClr
  edtCvtNrm.ForeColor = lValDisClr
  chkCvtDCE.ForeColor = lLblDisClr
  edtCvtDCE.ForeColor = lValDisClr
  lblCvtVol.ForeColor = lLblDisClr
  edtCvtVol.ForeColor = lValDisClr
  cmdCvtAdv.FontBold = False
  cmdCvtEqu.FontBold = False

  ' Update audio convert command line
  sCmdLin = Trim(frmEnvOpt.edtPrgDir) & "PCMCvt"
  sCmdLin = sCmdLin & " " & Trim(edtCvtSrc)
  
  ' Set destination file
  If ("" <> Trim(edtCvtDst)) Then
    lblCvtDst.ForeColor = lLblEnaClr
    edtCvtDst.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " " & Trim(edtCvtDst)
  End If
  
  ' Disable backup if source <> destination
  If ("" = Trim(edtCvtDst)) Or Not (0 <> StrComp(edtCvtSrc, edtCvtDst, 1)) Then
    chkCvtBak.Enabled = True
  Else
    chkCvtBak.Enabled = False
  End If

  ' Update backup flag
  If (chkCvtBak.Enabled And (1 <> chkCvtBak)) Then
    sCmdLin = sCmdLin & " -b"
    chkCvtBak.ForeColor = lLblEnaClr
  End If

  ' Check source data selection
  If (cboCvtSrcDat.ListIndex > 0) Then
    lblCvtSrcDat.ForeColor = lLblEnaClr
    cboCvtSrcDat.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -di" & ttDocTypInp(cboCvtSrcDat.ItemData(cboCvtSrcDat.ListIndex)).sPCMTyp
  End If
  
  ' Check source frequency override
  If (Val(edtCvtSrcFrq) > 0) Then
    lblCvtSrcFrq.ForeColor = lLblEnaClr
    edtCvtSrcFrq.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -fi" & Val(edtCvtSrcFrq)
  End If

  ' Check destination data selection
  If (cboCvtDstDat.ListIndex > 0) Then
    lblCvtDstDat.ForeColor = lLblEnaClr
    cboCvtDstDat.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -do" & ttDocTypOut(cboCvtDstDat.ItemData(cboCvtDstDat.ListIndex)).sPCMTyp
  End If
  
  ' Check destination frequency override
  If (Val(edtCvtDstFrq) > 0) Then
    lblCvtDstFrq.ForeColor = lLblEnaClr
    edtCvtDstFrq.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -fo" & Val(edtCvtDstFrq)
  End If

  ' Check crop guard time
  If (1 = chkCvtCrp) Then
    sCmdLin = sCmdLin & " -ac"
    chkCvtCrp.ForeColor = lLblEnaClr
    If (Val(edtCvtCrp) <> CRPGRDDEF) Then
      edtCvtCrp.ForeColor = lValEnaClr
      sCmdLin = sCmdLin & Val(edtCvtCrp)
    End If
  End If

  ' Check volume normalize
  If (1 = chkCvtNrm) Then
    sCmdLin = sCmdLin & " -vn"
    chkCvtNrm.ForeColor = lLblEnaClr
    If (Val(edtCvtNrm) <> VOLNRMDEF) Then
      edtCvtNrm.ForeColor = lValEnaClr
      sCmdLin = sCmdLin & Val(edtCvtNrm)
    End If
  End If

  ' Check dynamic cmp/exp
  If (1 = chkCvtDCE) Then
    sCmdLin = sCmdLin & " -vd"
    chkCvtDCE.ForeColor = lLblEnaClr
    If (Val(edtCvtDCE) <> CMPEXPDEF) Then
      edtCvtDCE.ForeColor = lValEnaClr
      sCmdLin = sCmdLin & Val(edtCvtDCE)
    End If
  End If

  ' Check volume adjust
  If (0 <> Val(edtCvtVol)) Then
    lblCvtVol.ForeColor = lLblEnaClr
    edtCvtVol.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -va" & Val(edtCvtVol)
  End If

  ' Check filter settings
  If (AAFALGNRM <> frmCvtFtr.cboAAFAlg) Then
    If (AAFALGOFF = frmCvtFtr.cboAAFAlg) Then sCmdLin = sCmdLin & " -q2off"
    If (AAFALGFST = frmCvtFtr.cboAAFAlg) Then sCmdLin = sCmdLin & " -q2fast"
    If (AAFALGQ6I = frmCvtFtr.cboAAFAlg) Then sCmdLin = sCmdLin & " -q6fast"
    If (AAFALGQ6F = frmCvtFtr.cboAAFAlg) Then sCmdLin = sCmdLin & " -q6"
    If (AAFALGFFT = frmCvtFtr.cboAAFAlg) Then sCmdLin = sCmdLin & " -ql"
  End If
  If (RESALGNRM <> frmCvtFtr.cboResAlg) Then
    If (RESALGFFT = frmCvtFtr.cboResAlg) Then sCmdLin = sCmdLin & " -qr"
  End If
  If (frmCvtFtr.chkDTMFtr) Then sCmdLin = sCmdLin & " -qd"

  ' Check Input Position Offset
  If (0 <> Val(frmCvtAdv.edtBytPos)) Then
    cmdCvtAdv.FontBold = True
    sCmdLin = sCmdLin & " -pi" & Val(frmCvtAdv.edtBytPos)
  End If

  ' Check Input Length
  If (0 <> Val(frmCvtAdv.edtBytLen)) Then
    cmdCvtAdv.FontBold = True
    sCmdLin = sCmdLin & " -li" & Val(frmCvtAdv.edtBytLen)
  End If

  ' Check swap input
  If (0 <> StrComp(OPTNONDEF, frmCvtAdv.cboSwpInp, 1)) Then
    cmdCvtAdv.FontBold = True
    sCmdLin = sCmdLin & " -si" & frmCvtAdv.cboSwpInp
  End If

  ' Check swap output
  If (0 <> StrComp(OPTNONDEF, frmCvtAdv.cboSwpOut, 1)) Then
    cmdCvtAdv.FontBold = True
    sCmdLin = sCmdLin & " -so" & frmCvtAdv.cboSwpOut
  End If

  ' Check equalizer settings
  If ("" <> Trim(frmCvtEqu.txtCvtEqu)) Then
    cmdCvtEqu.FontBold = True
    ' Set Equalizer gain and frequency range; Insure no embedded spaces
    sCmdLin = sCmdLin & " -qe" & "0" & Chr$(TBXARGDEL) & "0" & Chr$(TBXARGDEL) & Trim(Str(Val(frmAudTBx.edtCvtSrcFrq) / 2#))
    sCmdLin = sCmdLin & frmCvtEqu.txtCvtEqu
  End If

  ' Check global configuration file
  If ("" <> Trim(frmCvtAdv.edtCfgGlo)) Then
    cmdCvtAdv.FontBold = True
    sCmdLin = sCmdLin & " -ng" & frmCvtAdv.edtCfgGlo
  End If
  
  ' Check input configuration file
  If ("" <> Trim(frmCvtAdv.edtCfgInp)) Then
    cmdCvtAdv.FontBold = True
    sCmdLin = sCmdLin & " -ni" & frmCvtAdv.edtCfgInp
  End If
  
  ' Check output configuration file
  If ("" <> Trim(frmCvtAdv.edtCfgOut)) Then
    cmdCvtAdv.FontBold = True
    sCmdLin = sCmdLin & " -no" & frmCvtAdv.edtCfgOut
  End If

  ' Check exit mode
  If (0 <> StrComp(EXIMODNRM, frmEnvOpt.cboExiMod, 1)) Then
    sCmdLin = sCmdLin & " -exit" & frmEnvOpt.cboExiMod.ItemData(frmEnvOpt.cboExiMod.ListIndex)
  End If

  ' Update command line
  edtCmdLin = sCmdLin

End Sub

Sub cmdBrowse_Click ()
  Dim sFilNam As String

  ' Query user for file name
  sFilNam = QryFilNam(cBrwActCtl, FILOPNAUD)
  
  If ("" <> sFilNam) Then cBrwActCtl = sFilNam
  lblCurDir = CURDIRTXT & LCase(CurDir$)
  ' Set focus back to control
  cBrwActCtl.SetFocus

End Sub

Sub cmdCvtAdv_Click ()
  Dim sRetArg As String
  sRetArg = FrmModShw(frmAudTBx, frmCvtAdv, "", CTR_MIDMIDNON)
  CmdAudCvt
End Sub

Sub cmdCvtEqu_Click ()
  Dim sRetArg As String
  ' Adjust upper and lower frequency range for Nyquist rate
  sRetArg = FrmModShw(frmAudTBx, frmCvtEqu, "0" & Chr$(TBXARGDEL) & "0" & Chr$(TBXARGDEL) & Str(Val(frmAudTBx.edtCvtSrcFrq) / 2#), CTR_MIDMIDNON)
  CmdAudCvt
End Sub

Sub cmdExecute_click ()
  Dim hDosIns As Integer
  
  Const SI_CTLINSMEM = 0            ' Insufficient memory
  Const SI_CTLFILNOF = 2            ' Program not found
  Const SI_CTLPTHNOF = 3            ' Path not found
  Const SI_CTLLNKTSK = 5            ' Attempt to link to task
  Const SI_CTLSEPSEG = 6            ' Library requires sep segs
  Const SI_CTLUNK008 = 8            ' Insufficient memory to load
  Const SI_CTLWINVER = 10           ' Incorrect Windows version
  Const SI_CTLBADEXE = 11           ' Invalid controller EXE file
  Const SI_CTLOS2APP = 12           ' OS/2 application
  Const SI_CTLDOSAPP = 13           ' Dos 4.0 Application
  Const SI_CTLUNKEXE = 14           ' Unknow EXE type
  Const SI_CTLMODVER = 15           ' Protected mode wrong version
  Const SI_CTLSECINS = 16           ' Attempt to load second inst
  Const SI_CTLEMSERR = 17           ' Large frame EMS error
  Const SI_CTLPROMOD = 18           ' Attempt to load protected mode
  
  ' Execute DOS command line
  hDosIns = WinExec(edtCmdLin.Text, frmEnvOpt.cboExeMod.ItemData(frmEnvOpt.cboExeMod.ListIndex))
  
  ' Check for valid return
  If (hDosIns <= 32) Then
    Select Case hDosIns
      Case SI_CTLINSMEM: MsgBox "Insufficient memory!" & CRLF() & edtCmdLin.Text, MB_ICONEXCLAMATION, "Execute Error"
      Case SI_CTLFILNOF: MsgBox "Program not found!" & CRLF() & edtCmdLin.Text, MB_ICONEXCLAMATION, "Execute Error"
      Case SI_CTLPTHNOF: MsgBox "Path not found!" & CRLF() & edtCmdLin.Text, MB_ICONEXCLAMATION, "Execute Error"
      Case SI_CTLLNKTSK: MsgBox "Attempt to link to task!" & CRLF() & edtCmdLin.Text, MB_ICONEXCLAMATION, "Execute Error"
      Case SI_CTLSEPSEG: MsgBox "Library requires separate segments!" & CRLF() & edtCmdLin.Text, MB_ICONEXCLAMATION, "Execute Error"
      Case SI_CTLUNK008: MsgBox "Insufficient memory to load!" & CRLF() & edtCmdLin.Text, MB_ICONEXCLAMATION, "Execute Error"
      Case SI_CTLWINVER: MsgBox "Incorrect Windows version!" & CRLF() & edtCmdLin.Text, MB_ICONEXCLAMATION, "Execute Error"
      Case SI_CTLBADEXE: MsgBox "Invalid EXE file!" & CRLF() & edtCmdLin.Text, MB_ICONEXCLAMATION, "Execute Error"
      Case SI_CTLOS2APP: MsgBox "OS/2 application!" & CRLF() & edtCmdLin.Text, MB_ICONEXCLAMATION, "Execute Error"
      Case SI_CTLDOSAPP: MsgBox "Dos 4.0 Application!" & CRLF() & edtCmdLin.Text, MB_ICONEXCLAMATION, "Execute Error"
      Case SI_CTLUNKEXE: MsgBox "Unknown EXE type!" & CRLF() & edtCmdLin.Text, MB_ICONEXCLAMATION, "Execute Error"
      Case SI_CTLMODVER: MsgBox "Protected mode wrong version!" & CRLF() & edtCmdLin.Text, MB_ICONEXCLAMATION, "Execute Error"
      Case SI_CTLSECINS: MsgBox "Attempt to load second instance!" & CRLF() & edtCmdLin.Text, MB_ICONEXCLAMATION, "Execute Error"
      Case SI_CTLEMSERR: MsgBox "Large frame EMS error!" & CRLF() & edtCmdLin.Text, MB_ICONEXCLAMATION, "Execute Error"
      Case SI_CTLPROMOD: MsgBox "Attempt to load protected mode!" & CRLF() & edtCmdLin.Text, MB_ICONEXCLAMATION, "Execute Error"
    Case Else
      MsgBox "Unknown Error" & CRLF() & edtCmdLin.Text, MB_ICONEXCLAMATION, "Execute Error"
    End Select
    ' Exit error
    Exit Sub
  End If

  ' Show Hourglass & wait for process to complete
  screen.MousePointer = 11
  Do While GetModuleUsage(hDosIns) > 0
    DoEvents
  Loop
  screen.MousePointer = 0

End Sub

Sub cmdExit_Click ()
    mnExit_Click
End Sub

Sub cmdHelp_Click ()
  SendKeys "{F1}"
End Sub

Sub CmdIdxChp ()
  Dim sCmdLin As String
  
  ' Gray all options
  lblChpVox.ForeColor = lLblDisClr
  edtChpVox.ForeColor = lValDisClr
  lblChpTxt.ForeColor = lLblDisClr
  edtChpTxt.ForeColor = lValDisClr
  lblChpBeg.ForeColor = lLblDisClr
  edtChpBeg.ForeColor = lValDisClr
  lblChpCnt.ForeColor = lLblDisClr
  edtChpCnt.ForeColor = lValDisClr
  lblChpOff.ForeColor = lLblDisClr
  edtChpOff.ForeColor = lValDisClr
  
  ' Update index chop command line
  sCmdLin = Trim(frmEnvOpt.edtPrgDir) & "IdxChp"
  sCmdLin = sCmdLin & " " & Trim(edtChpSrc)
  
  ' Check vox extension
  If (0 <> StrComp(OPTVOXDEF, edtChpVox, 1)) Then
    lblChpVox.ForeColor = lLblEnaClr
    edtChpVox.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " " & edtChpVox
  End If

  ' Check text extension
  If (0 <> StrComp(OPTTXTDEF, edtChpTxt, 1)) Then
    ' The command line requires ".Vox" then ".Txt" - force .Vox if default
    If (0 = StrComp(OPTVOXDEF, edtChpVox, 1)) Then sCmdLin = sCmdLin & " " & edtChpVox
    lblChpTxt.ForeColor = lLblEnaClr
    edtChpTxt.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " " & edtChpTxt
  End If

  ' Check file name offset
  If (Val(edtChpOff) > 0) Then
    lblChpOff.ForeColor = lLblEnaClr
    edtChpOff.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -co" & Val(edtChpOff)
  End If

  ' Check index count
  If (Val(edtChpCnt) > 0) Then
    lblChpCnt.ForeColor = lLblEnaClr
    edtChpCnt.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -n" & Val(edtChpCnt)
  End If

  ' Check beginning index offset
  If (Val(edtChpBeg) <> IDXPOSDEF) Then
    lblChpBeg.ForeColor = lLblEnaClr
    edtChpBeg.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -@" & Val(edtChpBeg)
  End If

  ' Check exit mode
  If (0 <> StrComp(EXIMODNRM, frmEnvOpt.cboExiMod, 1)) Then
    sCmdLin = sCmdLin & " -exit" & frmEnvOpt.cboExiMod.ItemData(frmEnvOpt.cboExiMod.ListIndex)
  End If

  ' Update command line
  edtCmdLin = sCmdLin

End Sub

Sub CmdIdxLst ()
  Dim sCmdLin As String
  
  ' Gray all options
  chkLstHdr.ForeColor = lLblDisClr
  chkLstTxt.ForeColor = lLblDisClr
  chkLstAud.ForeColor = lLblDisClr
  
  ' Update index list command line
  sCmdLin = Trim(frmEnvOpt.edtPrgDir) & "IdxLst"
  sCmdLin = sCmdLin & " " & Trim(edtLstSrc)
  
  ' Update options & darken any non-default
  If (0 = chkLstHdr) Then
    sCmdLin = sCmdLin & " -g"
    chkLstHdr.ForeColor = lLblEnaClr
  End If
  If (0 = chkLstTxt) Then
    sCmdLin = sCmdLin & " -t"
    chkLstTxt.ForeColor = lLblEnaClr
  End If
  If (0 = chkLstAud) Then
    sCmdLin = sCmdLin & " -v"
    chkLstAud.ForeColor = lLblEnaClr
  End If

  ' Check exit mode
  If (0 <> StrComp(EXIMODNRM, frmEnvOpt.cboExiMod, 1)) Then
    sCmdLin = sCmdLin & " -exit" & frmEnvOpt.cboExiMod.ItemData(frmEnvOpt.cboExiMod.ListIndex)
  End If

  ' Update command line
  edtCmdLin = sCmdLin

End Sub

Sub CmdIdxPak ()
  Dim sCmdLin As String
  
  ' Gray all options
  lblPakFrq.ForeColor = lLblDisClr
  edtPakFrq.ForeColor = lValDisClr
  lblPakTxt.ForeColor = lLblDisClr
  cboPakTxt.ForeColor = lValDisClr
  lblPakSrt.ForeColor = lLblDisClr
  cboPakSrt.ForeColor = lValDisClr
       
  ' Update index pack command line
  sCmdLin = Trim(frmEnvOpt.edtPrgDir) & "IdxPak"
  sCmdLin = sCmdLin & " " & Trim(edtPakSrc)
  sCmdLin = sCmdLin & " " & Trim(edtPakDst)
  
  ' Check pack header frequency
  If (Val(edtPakFrq) > 0) And (Val(edtPakFrq) <> IDXFRQDEF) Then
    lblPakFrq.ForeColor = lLblEnaClr
    edtPakFrq.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -f" & Val(edtPakFrq)
  End If

  ' Check text extension
  If (0 <> StrComp(OPTNONDEF, cboPakTxt, 1)) Then
    lblPakTxt.ForeColor = lLblEnaClr
    cboPakTxt.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -t" & cboPakTxt
  End If

  ' Check sort selection
  Select Case cboPakSrt.ListIndex
    Case 0
      ' Default
    Case 1
      lblPakSrt.ForeColor = lLblEnaClr
      cboPakSrt.ForeColor = lValEnaClr
      sCmdLin = sCmdLin & " -s-"
    Case 2
      lblPakSrt.ForeColor = lLblEnaClr
      cboPakSrt.ForeColor = lValEnaClr
      sCmdLin = sCmdLin & " -su"
  End Select
  
  ' Check exit mode
  If (0 <> StrComp(EXIMODNRM, frmEnvOpt.cboExiMod, 1)) Then
    sCmdLin = sCmdLin & " -exit" & frmEnvOpt.cboExiMod.ItemData(frmEnvOpt.cboExiMod.ListIndex)
  End If

  ' Update command line
  edtCmdLin = sCmdLin

End Sub

Sub CmdIdxReb ()
  Dim sCmdLin As String
  
  ' Gray all options
  lblRebDst.ForeColor = lLblDisClr
  edtRebDst.ForeColor = lValDisClr
  chkRebBak.ForeColor = lLblDisClr
  lblRebFrq.ForeColor = lLblDisClr
  edtRebFrq.ForeColor = lValDisClr
  lblRebBeg.ForeColor = lLblDisClr
  edtRebBeg.ForeColor = lValDisClr
  lblRebIns.ForeColor = lLblDisClr
  edtRebIns.ForeColor = lValDisClr
  lblRebDel.ForeColor = lLblDisClr
  edtRebDel.ForeColor = lValDisClr
  lblRebVox.ForeColor = lLblDisClr
  edtRebVox.ForeColor = lValDisClr
  lblRebTxt.ForeColor = lLblDisClr
  edtRebTxt.ForeColor = lValDisClr
  
  ' Update index rebuild command line
  sCmdLin = Trim(frmEnvOpt.edtPrgDir) & "IdxReb"
  sCmdLin = sCmdLin & " " & Trim(edtRebSrc)
  
  ' Set destination file
  If ("" <> Trim(edtRebDst)) Then
    lblRebDst.ForeColor = lLblEnaClr
    edtRebDst.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " " & Trim(edtRebDst)
  End If
  
  ' Disable backup if source <> destination
  If ("" = Trim(edtRebDst)) Or Not (0 <> StrComp(edtRebSrc, edtRebDst, 1)) Then
    chkRebBak.Enabled = True
  Else
    chkRebBak.Enabled = False
  End If

  ' Update backup inhibit flag
  If (chkRebBak.Enabled And (1 <> chkRebBak)) Then
    sCmdLin = sCmdLin & " -b"
    chkRebBak.ForeColor = lLblEnaClr
  End If

  ' Check rebuild header frequency
  If (Val(edtRebFrq) > 0) And (Val(edtRebFrq) <> IDXFRQDEF) Then
    lblRebFrq.ForeColor = lLblEnaClr
    edtRebFrq.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -f" & Val(edtRebFrq)
  End If

  ' Check beginning index offset
  If (Val(edtRebBeg) <> IDXPOSDEF) Then
    lblRebBeg.ForeColor = lLblEnaClr
    edtRebBeg.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -@" & Val(edtRebBeg)
  End If

  ' Check insert index count
  If (Val(edtRebIns) > 0) Then
    lblRebIns.ForeColor = lLblEnaClr
    edtRebIns.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -i" & Val(edtRebIns)
  End If

  ' Check delete index count
  If (Val(edtRebDel) > 0) Then
    lblRebDel.ForeColor = lLblEnaClr
    edtRebDel.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -d" & Val(edtRebDel)
  End If

  ' Set replacement audio file
  If ("" <> Trim(edtRebVox)) Then
    lblRebVox.ForeColor = lLblEnaClr
    edtRebVox.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -rv" & Trim(edtRebVox)
  End If
  
  ' Set replacement text file
  If ("" <> Trim(edtRebTxt)) Then
    lblRebTxt.ForeColor = lLblEnaClr
    edtRebTxt.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -rt" & Trim(edtRebTxt)
  End If

  ' Check exit mode
  If (0 <> StrComp(EXIMODNRM, frmEnvOpt.cboExiMod, 1)) Then
    sCmdLin = sCmdLin & " -exit" & frmEnvOpt.cboExiMod.ItemData(frmEnvOpt.cboExiMod.ListIndex)
  End If

  ' Update command line
  edtCmdLin = sCmdLin

End Sub

Sub cmdMakBat_Click ()
  On Error GoTo cmdMakBatErr_ClickErr
  
  Dim sFilNam As String

  ' Query user for file name
  sFilNam = QryFilNam(edtMakBat, FILOPNBAT)
  lblCurDir = CURDIRTXT & LCase(CurDir$)
  If ("" = sFilNam) Then Exit Sub
  edtMakBat.Text = sFilNam

  ' Copy command line to file
  Open sFilNam For Output As #1
  Print #1, "@echo off"
  Print #1, edtCmdLin
  Close #1
  Exit Sub

cmdMakBatErr_ClickErr:
  On Error GoTo 0
  MsgBox "Error writing to file: " & sFilNam, MB_ICONEXCLAMATION
  Exit Sub

End Sub

Sub CmdSndChp ()
  Dim sCmdLin As String
  
  ' Gray all options
  lblSndDat.ForeColor = lLblDisClr
  cboSndDat.ForeColor = lValDisClr
  lblSndFrq.ForeColor = lLblDisClr
  edtSndFrq.ForeColor = lValDisClr
  lblSndExt.ForeColor = lLblDisClr
  edtSndExt.ForeColor = lValDisClr
  lblSndGrd.ForeColor = lLblDisClr
  edtSndGrd.ForeColor = lValDisClr
  lblSndOff.ForeColor = lLblDisClr
  edtSndOff.ForeColor = lValDisClr
  lblSndThr.ForeColor = lLblDisClr
  edtSndThr.ForeColor = lValDisClr
  
  ' Update index chop command line
  sCmdLin = Trim(frmEnvOpt.edtPrgDir) & "SndChp"
  sCmdLin = sCmdLin & " " & Trim(edtSndSrc)
  
  ' Check destination vox extension
  If (0 <> StrComp(GetFilExt(edtSndSrc), edtSndExt, 1)) Then
    lblSndExt.ForeColor = lLblEnaClr
    edtSndExt.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " " & edtSndExt
  End If

  ' Check source data selection
  If (cboSndDat.ListIndex > 0) Then
    lblSndDat.ForeColor = lLblEnaClr
    cboSndDat.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -di" & ttDocTypInp(cboSndDat.ItemData(cboSndDat.ListIndex)).sPCMTyp
  End If
  
  ' Check frequency override
  If (Val(edtSndFrq) > 0) Then
    lblSndFrq.ForeColor = lLblEnaClr
    edtSndFrq.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -fi" & Val(edtSndFrq)
  End If

  ' Check chop guard time
  If (Val(edtSndGrd) >= 0) And (Val(edtSndGrd) <> CRPGRDDEF) Then
    lblSndGrd.ForeColor = lLblEnaClr
    edtSndGrd.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -ac" & Val(edtSndGrd)
  End If

  ' Check file name offset
  If (Val(edtSndOff) > 0) Then
    lblSndOff.ForeColor = lLblEnaClr
    edtSndOff.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -co" & Val(edtSndOff)
  End If

  ' Check sound threshold
  If (Val(edtSndThr) >= 0) And (Val(edtSndThr) <> SNDTHRDEF) Then
    lblSndThr.ForeColor = lLblEnaClr
    edtSndThr.ForeColor = lValEnaClr
    sCmdLin = sCmdLin & " -st" & Val(edtSndThr)
  End If

  ' Check exit mode
  If (0 <> StrComp(EXIMODNRM, frmEnvOpt.cboExiMod, 1)) Then
    sCmdLin = sCmdLin & " -exit" & frmEnvOpt.cboExiMod.ItemData(frmEnvOpt.cboExiMod.ListIndex)
  End If

  ' Update command line
  edtCmdLin = sCmdLin

End Sub

Sub edtChpBeg_Change ()
  CmdIdxChp
End Sub

Sub edtChpBeg_LostFocus ()
  If (0 = Val(edtChpBeg)) Then edtChpBeg = IDXPOSDEF
End Sub

Sub edtChpCnt_Change ()
  CmdIdxChp
End Sub

Sub edtChpCnt_LostFocus ()
  If (0 = Val(edtChpCnt)) Then edtChpCnt = OPTALLDEF
End Sub

Sub edtChpOff_Change ()
  CmdIdxChp
End Sub

Sub edtChpOff_LostFocus ()
  If (0 = Val(edtChpOff)) Then edtChpOff = 0
End Sub

Sub edtChpSrc_Change ()
  CmdIdxChp
End Sub

Sub edtChpSrc_GotFocus ()
  ' Set currently active control & enable browse button
  Set cBrwActCtl = edtChpSrc
  cmdBrowse.Enabled = True
End Sub

Sub edtChpSrc_LostFocus ()
  ' Force default value if nothing entered
  If ("" = Trim(edtChpSrc)) Then edtChpSrc = "*.vap"
  
  ' Disable browse button (unless browse is getting focus)
  If (cmdBrowse <> ActiveControl) Then cmdBrowse.Enabled = False
End Sub

Sub edtChpTxt_Change ()
  CmdIdxChp
End Sub

Sub edtChpTxt_LostFocus ()
  If ("" = Trim(edtChpTxt)) Then edtChpTxt = OPTTXTDEF
End Sub

Sub edtChpVox_Change ()
  CmdIdxChp
End Sub

Sub edtChpVox_LostFocus ()
  If ("" = Trim(edtChpVox)) Then edtChpVox = OPTVOXDEF
End Sub

Sub edtCmdLin_KeyPress (KeyAscii As Integer)
    If KeyChkRet(KeyAscii) Then Call cmdExecute_click
End Sub

Sub edtCvtCrp_Change ()
  CmdAudCvt
End Sub

Sub edtCvtCrp_LostFocus ()
  If ("" = Trim(edtCvtCrp)) Then edtCvtCrp = CRPGRDDEF
End Sub

Sub edtCvtDCE_Change ()
  CmdAudCvt
End Sub

Sub edtCvtDCE_LostFocus ()
  If ("" = Trim(edtCvtDCE)) Then edtCvtDCE = CMPEXPDEF
End Sub

Sub edtCvtDst_Change ()
  CmdAudCvt
End Sub

Sub edtCvtDst_GotFocus ()
  ' Set currently active control & enable browse button
  Set cBrwActCtl = edtCvtDst
  cmdBrowse.Enabled = True
End Sub

Sub edtCvtDst_LostFocus ()
  ' Force default value if nothing entered
  If ("" = Trim(edtCvtDst)) Then edtCvtDst = ""
  
  ' Disable browse button (unless browse is getting focus)
  If (cmdBrowse <> ActiveControl) Then cmdBrowse.Enabled = False
End Sub

Sub edtCvtDstFrq_Change ()
  CmdAudCvt
End Sub

Sub edtCvtDstFrq_LostFocus ()
  If (0 = Val(edtCvtDstFrq)) Then edtCvtDstFrq = OPTDEFDEF
End Sub

Sub edtCvtNrm_Change ()
  CmdAudCvt
End Sub

Sub edtCvtNrm_LostFocus ()
  If ("" = Trim(edtCvtNrm)) Then edtCvtNrm = VOLNRMDEF
End Sub

Sub edtCvtSrc_Change ()
  CmdAudCvt
End Sub

Sub edtCvtSrc_GotFocus ()
  ' Set currently active control & enable browse button
  Set cBrwActCtl = edtCvtSrc
  cmdBrowse.Enabled = True
End Sub

Sub edtCvtSrc_LostFocus ()
  ' Force default value if nothing entered
  If ("" = Trim(edtCvtSrc)) Then edtCvtSrc = "*.wav"
  
  ' Disable browse button (unless browse is getting focus)
  If (cmdBrowse <> ActiveControl) Then cmdBrowse.Enabled = False
End Sub

Sub edtCvtSrcFrq_Change ()
  CmdAudCvt
End Sub

Sub edtCvtSrcFrq_LostFocus ()
  If (0 = Val(edtCvtSrcFrq)) Then edtCvtSrcFrq = OPTDEFDEF
End Sub

Sub edtCvtVol_Change ()
  CmdAudCvt
End Sub

Sub edtCvtVol_LostFocus ()
  If ("" = Trim(edtCvtVol)) Then edtCvtVol = "+0"
End Sub

Sub edtLstSrc_Change ()
  CmdIdxLst
End Sub

Sub edtLstSrc_GotFocus ()
  ' Set currently active control & enable browse button
  Set cBrwActCtl = edtLstSrc
  cmdBrowse.Enabled = True
End Sub

Sub edtLstSrc_LostFocus ()
  ' Force default value if nothing entered
  If ("" = Trim(edtLstSrc)) Then edtLstSrc = "*.vap"
  
  ' Disable browse button (unless browse is getting focus)
  If (cmdBrowse <> ActiveControl) Then cmdBrowse.Enabled = False
End Sub

Sub edtMakBat_LostFocus ()
  ' Force default value if nothing entered
  If ("" = Trim(edtMakBat)) Then edtMakBat = "*.bat"
End Sub

Sub edtPakDst_Change ()
  CmdIdxPak
End Sub

Sub edtPakDst_GotFocus ()
  ' Set currently active control & enable browse button
  Set cBrwActCtl = edtPakDst
  cmdBrowse.Enabled = True
End Sub

Sub edtPakDst_LostFocus ()
  ' Force default value if nothing entered
  If ("" = Trim(edtPakDst)) Then edtPakDst = "index.vap"
  
  ' Disable browse button (unless browse is getting focus)
  If (cmdBrowse <> ActiveControl) Then cmdBrowse.Enabled = False
End Sub

Sub edtPakFrq_Change ()
  CmdIdxPak
End Sub

Sub edtPakFrq_LostFocus ()
  If (0 = Val(edtPakFrq)) Then edtPakFrq = IDXFRQDEF
End Sub

Sub edtPakSrc_Change ()
  CmdIdxPak
End Sub

Sub edtPakSrc_GotFocus ()
  ' Set currently active control & enable browse button
  Set cBrwActCtl = edtPakSrc
  cmdBrowse.Enabled = True
End Sub

Sub edtPakSrc_LostFocus ()
  ' Force default value if nothing entered
  If ("" = Trim(edtPakSrc)) Then edtPakSrc = "*" & OPTVOXDEF
  
  ' Disable browse button (unless browse is getting focus)
  If (cmdBrowse <> ActiveControl) Then cmdBrowse.Enabled = False
End Sub

Sub edtRebBeg_Change ()
  CmdIdxReb
End Sub

Sub edtRebBeg_LostFocus ()
  If (0 = Val(edtRebBeg)) Then edtRebBeg = IDXPOSDEF
End Sub

Sub edtRebDel_Change ()
  CmdIdxReb
End Sub

Sub edtRebDel_LostFocus ()
  If (0 = Val(edtRebDel)) Then edtRebDel = 0
End Sub

Sub edtRebDst_Change ()
  CmdIdxReb
End Sub

Sub edtRebDst_GotFocus ()
  ' Set currently active control & enable browse button
  Set cBrwActCtl = edtRebDst
  cmdBrowse.Enabled = True
End Sub

Sub edtRebDst_LostFocus ()
  ' Force default value if nothing entered
  If (("" = Trim(edtRebDst)) Or (0 = StrComp(edtRebSrc, edtRebDst, 1))) Then edtRebDst = ""

  ' Disable browse button (unless browse is getting focus)
  If (cmdBrowse <> ActiveControl) Then cmdBrowse.Enabled = False
End Sub

Sub edtRebFrq_Change ()
  CmdIdxReb
End Sub

Sub edtRebFrq_LostFocus ()
  If (0 = Val(edtRebFrq)) Then edtRebFrq = IDXFRQDEF
End Sub

Sub edtRebIns_Change ()
  CmdIdxReb
End Sub

Sub edtRebIns_LostFocus ()
  If (0 = Val(edtRebIns)) Then edtRebIns = 0
End Sub

Sub edtRebSrc_Change ()
  CmdIdxReb
End Sub

Sub edtRebSrc_GotFocus ()
  ' Set currently active control & enable browse button
  Set cBrwActCtl = edtRebSrc
  cmdBrowse.Enabled = True
End Sub

Sub edtRebSrc_LostFocus ()
  ' Force default value if nothing entered
  If ("" = Trim(edtRebSrc)) Then edtRebSrc = "*.vap"
  
  ' Disable browse button (unless browse is getting focus)
  If (cmdBrowse <> ActiveControl) Then cmdBrowse.Enabled = False
End Sub

Sub edtRebTxt_Change ()
  CmdIdxReb
End Sub

Sub edtRebTxt_GotFocus ()
  ' Set currently active control & enable browse button
  Set cBrwActCtl = edtRebTxt
  cmdBrowse.Enabled = True
End Sub

Sub edtRebTxt_LostFocus ()
  ' Force default value if nothing entered
  If ("" = Trim(edtRebTxt)) Then edtRebTxt = ""
  
  ' Disable browse button (unless browse is getting focus)
  If (cmdBrowse <> ActiveControl) Then cmdBrowse.Enabled = False
End Sub

Sub edtRebVox_Change ()
  CmdIdxReb
End Sub

Sub edtRebVox_GotFocus ()
  ' Set currently active control & enable browse button
  Set cBrwActCtl = edtRebVox
  cmdBrowse.Enabled = True
End Sub

Sub edtRebVox_LostFocus ()
  ' Force default value if nothing entered
  If ("" = Trim(edtRebVox)) Then edtRebVox = ""
  
  ' Disable browse button (unless browse is getting focus)
  If (cmdBrowse <> ActiveControl) Then cmdBrowse.Enabled = False
End Sub

Sub edtSndExt_Change ()
  CmdSndChp
End Sub

Sub edtSndExt_LostFocus ()
  If ("" = Trim(edtSndExt)) Then edtSndExt = GetFilExt(edtSndSrc)
End Sub

Sub edtSndFrq_Change ()
  CmdSndChp
End Sub

Sub edtSndFrq_LostFocus ()
  If (0 = Val(edtSndFrq)) Then edtSndFrq = OPTDEFDEF
End Sub

Sub edtSndGrd_Change ()
  CmdSndChp
End Sub

Sub edtSndGrd_LostFocus ()
  If ("" = Trim(edtSndGrd)) Then edtSndGrd = CRPGRDDEF
End Sub

Sub edtSndOff_Change ()
  CmdSndChp
End Sub

Sub edtSndOff_LostFocus ()
  If (0 = Val(edtSndOff)) Then edtSndOff = 0
End Sub

Sub edtSndSrc_Change ()
  CmdSndChp
End Sub

Sub edtSndSrc_GotFocus ()
  ' Set currently active control & enable browse button
  Set cBrwActCtl = edtSndSrc
  cmdBrowse.Enabled = True
End Sub

Sub edtSndSrc_LostFocus ()
  ' Force default value if nothing entered
  If ("" = Trim(edtSndSrc)) Then edtSndSrc = "*.wav"
  
  ' Disable browse button (unless browse is getting focus)
  If (cmdBrowse <> ActiveControl) Then cmdBrowse.Enabled = False
End Sub

Sub edtSndThr_Change ()
  CmdSndChp
End Sub

Sub edtSndThr_LostFocus ()
  If ("" = Trim(edtSndThr)) Then edtSndThr = SNDTHRDEF
End Sub

Sub Form_Load ()
  HelpContextID = HLP_AUDTBX
  On Error GoTo Form_LoadErr
  
  Dim iRetCod As Integer
  Dim ii As Integer
  
  ' Show initial splash screen, permit refresh
  FrmCtrMov frmTBXIni, 0, screen.Width, 0, screen.Height
  frmTBXIni.Show
  

  ' Show hour glass, permit splash screen to appear
  FrmUsrDis Me
  DoEvents
    
  ' Yield to complete re-paints
  DoEvents
  
  ' Save application working directory spec
  sAppWrkDir = CurDir$

  ' Set working directory to directory containing the application.
  ' This will insure that DLL's can be found (not in path)
  ChDrive app.Path
  ChDir app.Path
  
  ' Initialize name of .INI file
  If ("" = sPrfFilNam) Then sPrfFilNam = PrfGetDir() & PRF_PRFNAMDEF

  ' Initialize name of application help file
  app.HelpFile = app.Path & "\AudTBx.Hlp"

  ' Initialize DLL support
  iRetCod = InsDLLIni(0, 0&, "")
  
  ' Verify module versions are correct
  VerModUse

  ' Read last screen position
  Left = PrfGetStr(PRF_SCRPOSSEC, PRF_SCRLFTKEY, Str$(Left))
  Top = PrfGetStr(PRF_SCRPOSSEC, PRF_SCRTOPKEY, Str$(Top))
  SafWndSta Me, PrfGetStr(PRF_SCRPOSSEC, PRF_SCRSTAKEY, Str$(WindowState))

  ' Calculate Minimum x / y measurements
  Dim iMinX
  Dim iMinY
  
  iMinX = screen.TwipsPerPixelX * GetSystemMetrics(SM_CXMin)
  iMinY = screen.TwipsPerPixelY * GetSystemMetrics(SM_CYMin)

  ' Check for valid Left and Top
  If (Left + Width <= iMinX) Or (Left >= screen.Width - iMinX) Then Left = (screen.Width - Width) / 2
  If (Top + Height <= iMinY) Or (Top >= screen.Height - iMinY) Then Top = (screen.Height - Height) / 2

  ' Load data input/output arrays
  BldDatInp
  BldDatOut

  ' Load option defaults color (.INI support only)
  lLblEnaClr = PrfGetStr(PRF_ENVOPTSEC, PRF_LBLENAKEY, Str$(LBLENADEF))
  lLblDisClr = PrfGetStr(PRF_ENVOPTSEC, PRF_LBLDISKEY, Str$(LBLDISDEF))
  lValEnaClr = PrfGetStr(PRF_ENVOPTSEC, PRF_VALENAKEY, Str$(VALENADEF))
  lValDisClr = PrfGetStr(PRF_ENVOPTSEC, PRF_VALDISKEY, Str$(VALDISDEF))

  ' Load Environment Options form
  Load frmEnvOpt
  
  ' Load Audio Conversion Filter & Advanced form
  Load frmCvtFtr
  Load frmCvtAdv
  Load frmCvtEqu
  
  ' Load Audio Conversion filter listbox
  lstCvtFtr.AddItem frmCvtFtr.cboAAFAlg
  lstCvtFtr.AddItem frmCvtFtr.cboResAlg
  If (frmCvtFtr.chkDTMFtr) Then lstCvtFtr.AddItem DTMNOTFTR
  
  ' Load Audio Conversion comboBox; Force input format default to wave
  LodDatCBx ttDocTypInp(), cboCvtSrcDat, PrfGetStr(PRF_MFGVERKEY, PRF_MFGVERSEC, MFGVERDLG), MFGVERMPC
  cboCvtSrcDat.ListIndex = 0
  LodDatCBx ttDocTypOut(), cboCvtDstDat, PrfGetStr(PRF_MFGVERKEY, PRF_MFGVERSEC, MFGVERDLG), PrfGetStr(PRF_MFGVERKEY, PRF_MFGVERSEC, MFGVERDLG)
  cboCvtDstDat.ListIndex = 0

  ' Load Sound Chop comboBox; Force input format default to wave
  ' Note: 2.0, Available for Dialogic formats only
  ' LodDatCBx ttDocTypInp(), cboSndDat, MFGVERDLG, MFGVERMPC
  LodDatCBx ttDocTypInp(), cboSndDat, PrfGetStr(PRF_MFGVERKEY, PRF_MFGVERSEC, MFGVERDLG), MFGVERMPC
  cboSndDat.ListIndex = 0
  
  ' Load Index Pack comboBox
  cboPakSrt.AddItem "Ascending"
  cboPakSrt.AddItem "Descending"
  cboPakSrt.AddItem "Unsorted"
  cboPakSrt.ListIndex = 0
  cboPakTxt.AddItem OPTNONDEF
  cboPakTxt.AddItem "*" & OPTTXTDEF
  cboPakTxt.ListIndex = 0

  ' Update current directory display
  lblCurDir = CURDIRTXT & LCase(CurDir$)
  
  ' Restore Control contents
  For ii = 0 To Controls.Count - 1
    PrfCtlGet Controls(ii)
  Next ii

  ' Unload splash screen and show main form
  Unload frmTBXIni
  FrmUsrEna Me
  Show

  ' Insure that all tabs are visible
  ' Note: This must be done because the folders inherit
  ' the "invisible" state of the parent form
  For ii = 0 To tabAudTBx.Folder - 1
    tabAudTBx.ActiveFolder = ii
    tabAudTBx.Visible = True
  Next ii

  ' Set active tab & set focus to control(0)
  tabAudTBx.ActiveFolder = Min(Val(PrfGetStr(PRF_TABPOSSEC, PRF_ACTFLDKEY, "0")), tabAudTBx.Folders - 1)
  tabAudTbx_TabChanged Val(tabAudTBx.ActiveFolder), Val(tabAudTBx.ActiveFolder)

  ' Show command buttons
  TabSetOrd

  ' Everything ready to go!
  Exit Sub

Form_LoadErr:
    If (2 = MsgBox(APPINIERR, MB_RETRYCANCEL, caption)) Then ErrMsgExi ("")
    Resume Next

End Sub

Sub Form_Unload (Cancel As Integer)
  Dim ii As Integer

  ' Save current window state (if not minimized)
  If (MINIMIZED <> WindowState) Then PrfSetStr PRF_SCRPOSSEC, PRF_SCRSTAKEY, WindowState
  
  ' Save current screen position
  If (NORMAL = WindowState) Then
    PrfSetStr PRF_SCRPOSSEC, PRF_SCRLFTKEY, Left
    PrfSetStr PRF_SCRPOSSEC, PRF_SCRTOPKEY, Top
  End If

  ' Save active tab folder
  PrfSetStr PRF_TABPOSSEC, PRF_ACTFLDKEY, tabAudTBx.ActiveFolder

  ' Save Control contents
  For ii = 0 To Controls.Count - 1
    PrfCtlSet Controls(ii)
  Next ii

  ' Force all forms to close
  End
End Sub

Sub FrmUsrDis (fFrm As Form)
    ' Disable input, set mouse to hourglass
    fFrm.Enabled = False
    screen.MousePointer = HourGlass
End Sub

Sub FrmUsrEna (fFrm As Form)
    ' Re-enable input, set mouse to default
    screen.MousePointer = Default
    fFrm.Enabled = True
End Sub

Function GetFilExt (sFilNam) As String
    Dim iDotPos As Integer
    
    ' Return file extension string
    iDotPos = InStr(sFilNam, ".")
    If (0 <> iDotPos) Then GetFilExt = Mid(sFilNam, iDotPos)
End Function

Sub lblChpSrc_Click ()
  edtChpSrc.SetFocus
  DoEvents
  cmdBrowse_Click
End Sub

Sub lblCvtDst_Click ()
  edtCvtDst.SetFocus
  DoEvents
  cmdBrowse_Click
End Sub

Sub lblCvtSrc_Click ()
  edtCvtSrc.SetFocus
  DoEvents
  cmdBrowse_Click
End Sub

Sub lblLstSrc_Click ()
  edtLstSrc.SetFocus
  DoEvents
  cmdBrowse_Click
End Sub

Sub lblPakDst_Click ()
  edtPakDst.SetFocus
  DoEvents
  cmdBrowse_Click
End Sub

Sub lblPakSrc_Click ()
  edtPakSrc.SetFocus
  DoEvents
  cmdBrowse_Click
End Sub

Sub lblRebDst_Click ()
  edtRebDst.SetFocus
  DoEvents
  cmdBrowse_Click
End Sub

Sub lblRebSrc_Click ()
  edtRebSrc.SetFocus
  DoEvents
  cmdBrowse_Click
End Sub

Sub lblRebTxt_Click ()
  edtRebTxt.SetFocus
  DoEvents
  cmdBrowse_Click
End Sub

Sub lblRebVox_Click ()
  edtRebVox.SetFocus
  DoEvents
  cmdBrowse_Click
End Sub

Sub lblSndSrc_Click ()
  edtSndSrc.SetFocus
  DoEvents
  cmdBrowse_Click
End Sub

Sub LodDatCBx (ttDocTyp() As TBXDOCTYP, cLstBox As Control, sMfgVer As String, sMfgDef As String)
  On Error GoTo LodDatCBxErr
  Dim sPCMDes As String
  Dim ii As Integer
  
  ' Save space for manufacturer's default in first index
  cLstBox.AddItem MFGVERUNK
  cLstBox.ItemData(cLstBox.NewIndex) = 0

  ' Since the tools can be installed for a specific manufacturer,
  ' only load the combobox with available data types
  While (True)
    ' Skip any manufacturer specific exceptions
    If LodDatFtr(ttDocTyp(ii).sPCMTyp, sMfgVer) Then
      ' If no bounds error, retrieve string
      sPCMDes = ttDocTyp(ii).sPCMDes
      ' If no bounds error, load string into first (mfg default) or new position
      If LodDatDef(ttDocTyp(ii).sPCMTyp, sMfgDef) Then
        cLstBox.List(0) = sPCMDes
        cLstBox.ItemData(0) = ii
      Else
        cLstBox.AddItem sPCMDes
        cLstBox.ItemData(cLstBox.NewIndex) = ii
      End If
    End If
    ' If no bounds error, go to next entry
    ii = ii + 1
  Wend
  Exit Sub

LodDatCBxErr:
  On Error GoTo 0
  Exit Sub

End Sub

Private Function LodDatDef (sPCMTyp, sMfgDef As String)
  ' Determine if type is manufacturer default
  LodDatDef = False
  
  If ((MFGVERDLG = sMfgDef) Or (MFGVERRTX = sMfgDef)) And ("DLG004" = sPCMTyp) Then LodDatDef = True
  If (MFGVERG21 = sMfgDef) And ("G21004" = sPCMTyp) Then LodDatDef = True
  If (MFGVERMPC = sMfgDef) And ("WAV000" = sPCMTyp) Then LodDatDef = True
  If (MFGVERNWV = sMfgDef) And ("NWV001" = sPCMTyp) Then LodDatDef = True
  If (MFGVERPTC = sMfgDef) And ("PTC001" = sPCMTyp) Then LodDatDef = True

End Function

Private Function LodDatFtr (sPCMTyp, sMfgVer As String) As Integer
  ' Filter by manufacturer for any non-supported formats
  LodDatFtr = True
  If ((MFGVERDLG = sMfgVer) Or (MFGVERRTX = sMfgVer)) Then
    If ("NWV001" = sPCMTyp) Then LodDatFtr = False
    If ("PTC001" = sPCMTyp) Then LodDatFtr = False
    If ("G21004" = sPCMTyp) Then LodDatFtr = False
  End If
  If (MFGVERG21 = sMfgVer) Then
    If ("NWV001" = sPCMTyp) Then LodDatFtr = False
    If ("PTC001" = sPCMTyp) Then LodDatFtr = False
  End If
    If (MFGVERNWV = sMfgVer) Then
    If ("PTC001" = sPCMTyp) Then LodDatFtr = False
    If ("G21004" = sPCMTyp) Then LodDatFtr = False
  End If
    If (MFGVERPTC = sMfgVer) Then
    If ("NWV001" = sPCMTyp) Then LodDatFtr = False
    If ("G21004" = sPCMTyp) Then LodDatFtr = False
  End If

End Function

Sub lstCvtFtr_Click ()
  Dim sRetArg As String
  sRetArg = FrmModShw(frmAudTBx, frmCvtFtr, "", CTR_MIDMIDNON)
  
  ' Retrieve filter settings
  lstCvtFtr.Clear
  lstCvtFtr.AddItem frmCvtFtr.cboAAFAlg
  lstCvtFtr.AddItem frmCvtFtr.cboResAlg
  If (frmCvtFtr.chkDTMFtr) Then lstCvtFtr.AddItem DTMNOTFTR

  ' Update command line
  CmdAudCvt
End Sub

Sub mnAbout_Click ()
    Dim sRetArg As String
    sRetArg = FrmModShw(frmAudTBx, frmAbtTBx, "", CTR_MIDMIDNON)
End Sub

Sub mnAudioConversion_Click ()
  tabAudTBx.ActiveFolder = TABPCMCVT
End Sub

Sub mnEnvironment_Click ()
  Dim sRetArg As String
  sRetArg = FrmModShw(frmAudTBx, frmEnvOpt, "", CTR_MIDMIDNON)
  ' Update active tab
  tabAudTbx_TabChanged Val(tabAudTBx.ActiveFolder), 0
End Sub

Sub mnExit_Click ()
  Unload Me
  End
End Sub

Sub mnHelpContents_Click ()
  Dim ii%
  ii = WinHelp(hWnd, app.HelpFile, HELP_INDEX, 0)
End Sub

Sub mnHSearch_Click ()
    HelpSearch
End Sub

Sub mnIndexChop_Click ()
  tabAudTBx.ActiveFolder = TABIDXCHP
End Sub

Sub mnIndexList_Click ()
  tabAudTBx.ActiveFolder = TABIDXLST
End Sub

Sub mnIndexPack_Click ()
  tabAudTBx.ActiveFolder = TABIDXPAK
End Sub

Sub mnIndexRebuild_Click ()
  tabAudTBx.ActiveFolder = TABIDXREB
End Sub

Sub mnSoundChop_Click ()
  tabAudTBx.ActiveFolder = TABSNDCHP
End Sub

Sub mnTechSup_Click ()
  Dim ii%
  ii = WinHelp(hWnd, app.HelpFile, HELP_CONTEXT, ByVal CLng(HLP_TCHSUP))
End Sub

Sub tabAudTbx_Click ()
  tabAudTbx_TabChanged Val(tabAudTBx.ActiveFolder), Val(tabAudTBx.ActiveFolder)
End Sub

Sub tabAudTbx_TabChanged (NewFolder As Integer, OldFolder As Integer)
  Select Case NewFolder
    Case TABPCMCVT  ' audio conv
      edtCvtSrc.SetFocus
      CmdAudCvt
  
    Case TABSNDCHP  ' sound chop
      edtSndSrc.SetFocus
      CmdSndChp

    Case TABIDXCHP  ' chop
      edtChpSrc.SetFocus
      CmdIdxChp

    Case TABIDXLST  ' list
      edtLstSrc.SetFocus
      CmdIdxLst

    Case TABIDXPAK  ' pack
      edtPakSrc.SetFocus
      CmdIdxPak

    Case TABIDXREB  ' rebuild
      edtRebSrc.SetFocus
      CmdIdxReb

  End Select

  ' Initialize tab help information
  tabAudTBx.HelpContextID = HLP_TABBAS + HLP_TABINC * NewFolder
  HelpContextID = tabAudTBx.HelpContextID

  ' Show command buttons
  TabSetOrd
End Sub

Sub TabSetOrd ()
  ' Show command buttons
  cmdBrowse.ZOrder
  cmdHelp.ZOrder
  cmdExit.ZOrder
End Sub

Sub VerModUse ()
  ' DLL's used
  VerifyModule app.Path & "\VBRun300.DLL"
  VerifyModule app.Path & "\MHRun400.DLL"
  VerifyModule app.Path & "\CommDlg.DLL"
  VerifyModule app.Path & "\TBxIns.DLL"
  VerifyModule app.Path & "\Ver.DLL"

  ' VBX's used
  VerifyModule app.Path & "\CMDialog.VBX"
  VerifyModule app.Path & "\MHTab.VBX"
  VerifyModule app.Path & "\MHSl200.VBX"
  VerifyModule app.Path & "\ThreeD.VBX"
End Sub

