/************************************************************************/
/* Sound Chopping Utility Defs: SndChp.h                V3.00  04/15/96 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define CHPVERNUM       0x0300          /* Chp support ver number       */

/************************************************************************/
/*      Define general purpose VISI data types for SDK users            */
/************************************************************************/
#if !(defined (FIOVERNUM))                              /* No FIO defs  */ 
  typedef enum _FILFMT {
    UNKFILFMT =   0,                    /* File format = Unknown        */
    ALLHDRFMT =   4,                    /* File format = Headered       */
    ALLPURFMT =   8,                    /* File format = Pure           */
    AIFHDRFMT =  12,                    /* File format = AIF            */
    BKTHDRFMT =  16,                    /* File format = Brooktrout     */
    PL2HDRFMT =  20,                    /* File format = PowerLine      */
    SU4HDRFMT =  24,                    /* File format = Summa Four     */
    VBSIDXFMT =  28,                    /* File format = Indexed VBase  */
    VISHDRFMT =  32,                    /* File format = VIS Header     */
    WAVHDRFMT =  36,                    /* File format = MM Wave        */
    MAXFILFMT                           /* Maximum # entries            */
  } FILFMT;                  
#endif
#if !(defined (MEMVERNUM))                              /* No mem defs  */ 
    typedef unsigned long   VISMEMHDL;  /* VIS general purpose mem hdl  */
#endif
#if !(defined (PCMVERNUM))                              /* No PCM defs  */ 
  typedef enum _PCMTYP {
    UNKPCM000 =   0,                    /* Unknown conversion type      */
    AVAPCM004 =   4,                    /* AVA 4 bit                    */
    BKTPCM001 =   8,                    /* Brooktrout CVSD              */
    CPXPCM064 =  12,                    /* Complex rea/img, mag/phs x32 */
    DLGPCM004 =  16,                    /* DLG 4 bit                    */
    DLGPCM008 =  20,                    /* DLG uLaw Folded 8 bit        */
    FLTPCM032 =  24,                    /* Floating point 32 bit        */
    G11PCMF08 =  28,                    /* C-ITU (CCITT) G.711 U-law    */
    G11PCMI08 =  32,                    /* C-ITU (CCITT) G.711 A-law    */
    G21PCM004 =  36,                    /* C-ITU (CCITT) G.721          */
    G22PCM004 =  40,                    /* C-ITU (CCITT) G.722          */
    G23PCM003 =  44,                    /* C-ITU (CCITT) G.723 3 bit    */
    HARPCM001 =  48,                    /* Harris CVSD 1 bit            */
    MPCPCM008 =  52,                    /* MPC Unsigned 8 bit           */
    MPCPCM016 =  56,                    /* MPC Linear  16 bit           */
    MSAPCM004 =  60,                    /* MS Wave ADPCM 4 bit          */
    NWVPCM001 =  64,                    /* New Voice CVSD 1 bit         */
    PTCPCM001 =  68,                    /* Perception Tech CVSD 1 bit   */
    RTXPCM003 =  72,                    /* Rhetorex 3 bit               */
    RTXPCM004 =  76,                    /* Rhetorex 4 bit               */
    TTIPCM008 =  80,                    /* TTI 8 bit                    */
    MAXPCM000                           /* Maximum #PCM entries         */
  } PCMTYP;
#endif

/************************************************************************/
/************************************************************************/
#define SRCFRQMIN        100L           /* Minimum frequency (Hz)       */
#define SRCFRQMAX      65000L           /* Maximum frequency (Hz)       */

#define CHPFRMDEF  (float) 1.0          /* Chop frame length      (sec) */
#define CHPDCFDEF         20L           /* Crop DC high pass filt  (Hz) */
#define CHPRESDEF  (float)  .01         /* Chop resolution        (sec) */
#define CHPSNDDEF  (float) 2.0          /* Chop threshold        (%max) */
#define CHPATKDEF         20            /* Chop atk ratio       (1/frm) */
#define CHPDCYDEF         10            /* Chop dcy ratio       (1/frm) */
#define CHPGRDDEF  (float)  .05         /* Crop guard time        (sec) */

/************************************************************************/
/************************************************************************/
typedef WORD  (FAR PASCAL *CHPPOLPRC) (LPVOID, DWORD, DWORD);
typedef WORD  (FAR cdecl  *CHPERRPRC) (LPCSTR, ...);

/************************************************************************/
/************************************************************************/
typedef struct _CHPBLK {                /* Sound Chop structure         */
    WORD        usSrcFmt;               /* Source file format           */
    WORD        usSrcPCM;               /* Source PCM type              */
    WORD        usSrcChn;               /* Source file chan count       */
    WORD        usSrcBIO;               /* Source bit encoding          */
    DWORD       ulSrcFrq;               /* Source sample frequency      */
    DWORD       ulSrcOff;               /* Source byte offset           */
    DWORD       ulSrcLen;               /* Source byte length           */
    VISMEMHDL   mhSrcCfg;               /* Source cfg memory buff       */       
    WORD        usDstFmt;               /* Destination file format      */
    float       flChpFrm;               /* FndSndEvt frame size   (sec) */
    float       flChpRes;               /* FndSndEvt resolution   (sec) */
    float       flChpSnd;               /* FndSndEvt snd thresh  (%max) */
    WORD        usChpAtk;               /* FndSndEvt atk ratio   (%frm) */
    WORD        usChpDcy;               /* FndSndEvt dcy ratio   (%frm) */
    float       flChpGrd;               /* FndSndEvt guard time   (sec) */
    VISMEMHDL   mhGloCfg;               /* Global cfg memory buff       */       
    CHPPOLPRC   lpPolDsp;               /* Conversion poll display proc */
    DWORD       ulRsvPol;               /* Reserved                     */
    CHPERRPRC   lpMsgDsp;               /* Reserved                     */
    DWORD       ulRsvErr;               /* Reserved                     */
    BYTE        ucRsv001[32];           /* Reserved                     */
    WORD        usCmpCod;               /* Reserved                     */
    BYTE        ucRsv002[32];           /* Reserved                     */
} CHPBLK;

/************************************************************************/
/************************************************************************/
typedef BOOL  (FAR PASCAL *CHPOPNCBK) (short FAR *, DWORD);
typedef DWORD (FAR PASCAL *CHPCOPCBK) (short, DWORD, short, LPSTR, WORD, DWORD);
typedef BOOL  (FAR PASCAL *CHPCLSCBK) (short, DWORD);

/************************************************************************/
/************************************************************************/
WORD    FAR PASCAL  ChpSndVer (void);
WORD    FAR PASCAL  ChpSndIni (WORD, DWORD, DWORD);
WORD    FAR PASCAL  ChpSndTrm ();

WORD    FAR PASCAL  ChpCfgIni (CHPBLK FAR *);
WORD    FAR PASCAL  ChpCfgLod (LPSTR, VISMEMHDL FAR *);
WORD    FAR PASCAL  ChpCfgRel (VISMEMHDL FAR *);
DWORD   FAR PASCAL  ChpCapQry (enum _PCMTYP, DWORD, DWORD);
                    
WORD    FAR PASCAL  ChpSetDeb (WORD);                   
LPVOID  FAR PASCAL  ChpSetDsp (LPVOID);                 
                    
DWORD   FAR PASCAL  ChpSndFil (short, LPSTR, CHPOPNCBK, CHPCOPCBK, CHPCLSCBK, DWORD, CHPBLK FAR *);

/************************************************************************/
/*                  Initialization Return Codes                         */
/************************************************************************/
#define SI_CHPNO_ERR    0           /* No error                         */
#define SI_CHPKEYERR    1           /* Key/License error, 8 section lim */
#define SI_CHPVERERR    2           /* Module version mismatch          */
