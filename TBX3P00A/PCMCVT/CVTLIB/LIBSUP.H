/************************************************************************/
/* Generic PCM Conversion Utility Supp Defs: LibSup.h   V2.00  08/15/95 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define TBXMAXSTR         40            /* Gen purpose string length    */
#define REGKEYLEN          8            /* Registration key length      */
#define SERSEGLEN          4            /* Serial number segment length */
#define DEMFILDEF          8L           /* 8 sec file length limit      */

#define CRPDCFDEF         20L           /* Crop DC high pass filt  (Hz) */

#define AAFPASFAC  (float) 0.3          /* IIR low-pass pass bnd factor */
#define AAFSTPATT         40            /* IIR low-pass stop bnd atten  */
#define Q2_SECCNT          2            /* IIR low-pass section count   */
#define Q2_SECORD          1            /* IIR low-pass filter length   */
#define Q6_SECCNT          3            /* IIR low-pass section count   */
#define Q6_SECORD          2            /* IIR low-pass filter length   */

/************************************************************************/
/*              Declarations for Windows KERNEL Functions               */
/************************************************************************/
#if !(defined (WM_NULL))                                /* No Win       */ 
    typedef WORD        HANDLE;
#endif

/************************************************************************/
/************************************************************************/
typedef struct _TBXGLO {
    char *  szAppNam;                   /* Application name             */
    char *  szLibVer;                   /* Library version              */
    char    RelKeyArr [REGKEYLEN];      /* Operating/Demo Only Key      */
    char    SeqNumArr [SERSEGLEN];      /* Op/Demo sequence number      */
    char    ChkSumArr [SERSEGLEN];      /* Security check sum           */
    WORD    usRegKey;                   /* Registration key             */
    WORD    usDebFlg;                   /* Debug on/off flags           */
    HANDLE  hLibIns;                    /* Global instance handle       */
    HANDLE  hLicDlg;                    /* License dialog window handle */
} TBXGLO;

/************************************************************************/
/************************************************************************/
#define ERR___DEB       0x0002          /* Debug error messages flag    */
#define KEY___DEB       0x0008          /* Debug key flag               */
#define MEM___DEB       0x0020          /* Debug memory messages flag   */
#define CFG___DEB       0x0080          /* Debug config settings flag   */
#define INI___DEB       0x0200          /* Debug init   settings flag   */

/************************************************************************/
/************************************************************************/
DWORD   CvtFilEff (short, short, DWORD, LPSTR, WORD, struct _ITCBLK FAR *, 
        struct _ITCBLK FAR *, CVTBLK FAR *);

/************************************************************************/
/************************************************************************/
WORD             FFTFtrAlo (struct _CVTBLK FAR *, struct _FFTBLK FAR *, 
                 LPVOID, DWORD);
WORD  FAR PASCAL FFTFtrFnc (float FAR * FAR *, DWORD FAR *, DWORD, DWORD, 
                 DWORD, DWORD, DWORD, struct _FFTBLK FAR *);
WORD             FFTFtrRel (struct _FFTBLK FAR *);

/************************************************************************/
/*                      Error Message Support                           */
/************************************************************************/
#define CVTERRMSG if (TBxGlo.usDebFlg & ERR___DEB) MsgDspUsr

/************************************************************************/
/************************************************************************/
#define SI_MODVERMIS "Software module version mismatch: %s DLL\n"
#define SI_EFFDLLSUP "Effects"
#define SI_PCMDLLSUP "PCM Conversion"
#define SI_INSMEMERR "Insufficient main memory.\n"
#define SI_DOSMEMERR "Insufficient DOS global memory.\nSome functions will be inhibited.\n"
#define SI_ACCKEYNON "Accelerator Key not installed!\n"
#define SI_EFFEOSERR "Insufficient buffer memory, terminating...\n"

/************************************************************************/
/************************************************************************/
#define SI_CVSSECNAM "CVSD"                 /* CVSD Config section      */
#define SI_CVSLOWTYP "LowPassFilterType"    /* Low pass filter type     */
#define SI_CVSLOWPAS "LowPassFilter"        /* Low pass filter     (Hz) */
#define SI_CVSSIGFTR "SignalFilter"         /* Signal filter       (Hz) */
#define SI_CVSSYLATK "SyllabicAttack"       /* Syllabic attack     (Hz) */
#define SI_CVSSYLDCY "SyllabicDecay"        /* Syllabic attack     (Hz) */
#define SI_CVSCOIBIT "CoincidenceBit"       /* Coincidence bit count    */
#define SI_CVSSTPMIN "StepsizeMinimum"      /* Step size maximum  (AtD) */
#define SI_CVSSTPMAX "StepsizeMaximum"      /* Step size minimum  (AtD) */
                                                                        
/************************************************************************/
/************************************************************************/
#define SI_GLOSECNAM "Global"               /* Global section           */
#define SI_GLOFFTORD "FFTOrder"             /* FFT filter size          */

#define SI_VOLSECNAM "Volume"               /* Volume section           */
#define SI_VOLNRMEXC "NormalizeException"   /* VolNrm except thrsh (%)  */
#define SI_VOLNRMLVL "NormalizeLevel"       /* VolNrm target level (%)  */
#define SI_VOLNRMMAX "NormalizeMaximum"     /* VolNrm adjust max  (dB)  */

#define SI_DCESECNAM "Compressor/Expander"  /* Dynamic Cmp/Exp section  */
#define SI_DCECMPTHR "CompressorThreshold"  /* Comp  threshold     (%)  */ 
#define SI_DCECMPATK "CompressorAttack"     /* Comp  attack time   (Hz) */ 
#define SI_DCECMPMIN "CompressorMinimum"    /* Comp  minimum       (dB) */
#define SI_DCEEXPTHR "ExpanderThreshold"    /* Exp   threshold     (%)  */
#define SI_DCEEXPATK "ExpanderAttack"       /* Exp   attack time   (Hz) */
#define SI_DCEEXPMAX "ExpanderMaximum"      /* Exp   maximum       (dB) */
#define SI_DCECE_DCY "Decay"                /* C/E   decay time    (Hz) */
#define SI_DCENOITHR "NoiseGateThreshold"   /* NGate threshold     (%)  */
#define SI_DCENOIATT "NoiseGateAttenuation" /* NGate attenuation   (dB) */

#define SI_CRPSECNAM "AutoCrop"             /* Auto Crop section        */
#define SI_CRPEVTFRM "FrameSize"            /* Crop frame size    (sec) */
#define SI_CRPEVTRES "Resolution"           /* Crop resolution    (sec) */
#define SI_CRPEVTSND "SoundThreshold"       /* Crop threshold    (%max) */
#define SI_CRPEVTATK "AttackRatio"          /* Crop atk ratio   (1/frm) */
#define SI_CRPEVTDCY "DecayRatio"           /* Crop dcy ratio   (1/frm) */
#define SI_CRPEVTGRD "GuardTime"            /* Crop guard time    (sec) */

#define SI_FEQSECNAM "FrequencyEqualizer"   /* Freq Equalizer section   */
#define SI_FEQGAIADJ "GainAdjust"           /* Filter gain adjust       */
#define SI_FEQBOTFRQ "BottomFrequency"      /* Filter bottom frequency  */
#define SI_FEQTOPFRQ "TopFrequency"         /* Filter top    frequency  */
#define SI_FEQBNDKEY "BandKey"              /* Filter band key name     */

