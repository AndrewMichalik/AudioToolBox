/************************************************************************/
/* Generic Speed Adjust Utility Defs: LibSup.h          V2.00  08/15/95 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define TBXMAXSTR         40            /* Gen purpose string length    */
#define REGKEYLEN          8            /* Registration key length      */
#define SERSEGLEN          4            /* Serial number segment length */
#define DEMFILDEF          8L           /* 8 sec file length limit      */

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
    WORD    usRegKey;                   /* usRegKey                     */
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
DWORD   SpdFilEff (short, short, DWORD, LPSTR, WORD, struct _ITCBLK FAR *, 
        struct _ITCBLK FAR *, SPDBLK FAR *);

/************************************************************************/
/************************************************************************/
WORD             FFTFtrAlo (struct _SPDBLK FAR *, struct _FFTBLK FAR *, 
                 LPVOID, DWORD);
WORD  FAR PASCAL FFTFtrFnc (float FAR * FAR *, DWORD FAR *, DWORD, DWORD, 
                 DWORD, DWORD, DWORD, struct _FFTBLK FAR *);
WORD             FFTFtrRel (struct _FFTBLK FAR *);

/************************************************************************/
/*                      Error Message Support                           */
/************************************************************************/
#define SPDERRMSG if (TBxGlo.usDebFlg & ERR___DEB) MsgDspUsr

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
#define SI_VOCSECNAM "Vocoder"              /* Vocoder section          */
#define SI_VOCFFTORD "FFTOrder"             /* FFT filter size          */
#define SI_VOCWINORD "WindowOrder"          /* FFT window size          */
#define SI_VOCSYNTHR "SynthesisThreshold"   /* Synthesis threshold def  */

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
                                                                        

