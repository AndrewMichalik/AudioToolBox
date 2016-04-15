/************************************************************************/
/* Generic PCM Conversion Utility Defs: GenCvt.h        V3.00  04/15/96 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define CVTVERNUM       0x0300          /* Cvt support ver number       */

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
  #define FIOENCSWP   0x0800            /* Encode type: b/n/b/w swap    */ 
  #define FIOSWPBIT   0x0002            /* Swap bit    selection        */
  #define FIOSWPNIB   0x0004            /* Swap nibble selection        */
  #define FIOSWPBYT   0x0008            /* Swap byte selection          */
  #define FIOSWPWRD   0x0010            /* Swap word selection          */
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
#define SRCFRQMIN        100L           /* Minimum frequency       (Hz) */
#define SRCFRQMAX      65000L           /* Maximum frequency       (Hz) */

#define CRPFRMDEF  (float)  .50         /* Crop frame length      (sec) */
#define CRPRESDEF  (float)  .01         /* Crop resolution        (sec) */
#define CRPSNDDEF  (float) 2.0          /* Crop threshold        (%max) */
#define CRPATKDEF         20            /* Crop atk ratio        (%frm) */
#define CRPDCYDEF         10            /* Crop dcy ratio        (%frm) */
#define CRPGRDDEF  (float)  .05         /* Crop guard time        (sec) */
#define NRMLVLDEF         80            /* VolNrm target level      (%) */
#define NRMEXCDEF  (float) 0.2          /* VolNrm except thrsh      (%) */
#define NRMMAXDEF          6            /* VolNrm adjust max       (dB) */
#define DCELVLDEF          6            /* DCE   level             (dB) */
#define CMPTHRDEF         80            /* Comp  threshold          (%) */
#define CMPATKDEF         50            /* Comp  attack time       (Hz) */
#define EXPTHRDEF         30            /* Exp   threshold          (%) */
#define EXPATKDEF          5            /* Exp   attack time       (Hz) */
#define CE_DCYDEF          5            /* C/E   decay time        (Hz) */
#define NOITHRDEF          2            /* NGate threshold          (%) */
#define NOIATTDEF          3            /* NGate attenuation       (dB) */
#define AAF_Q2TYP          2            /* AAF IIR low-pass filter type */
#define AAF_Q6TYP          6            /* AAF IIR low-pass filter type */

#define FFTORDDEF          9            /* FFT filter size              */
#define FEQMAXCNT         64            /* Freq equalizer max pnt cnt   */
#define FEQADJDEF          0            /* Freq equalizer gain adj (dB) */
#define FEQBOTDEF        300            /* Freq equalizer bot freq (Hz) */
#define FEQTOPDEF       3800            /* Freq equalizer top freq (Hz) */
#define FEQCNTDEF          7            /* Freq equalizer def pnt cnt   */
#define FEQVOX001         +3            /*  300- 800: +3dB (<300Hz:0dB) */
#define FEQVOX002         +6            /*  800-1300: +6dB              */
#define FEQVOX003         +6            /* 1300-1800: +6dB              */
#define FEQVOX004         +6            /* 1800-2300: +6dB              */
#define FEQVOX005         +6            /* 2300-2800: +6dB              */
#define FEQVOX006         +6            /* 2800-3300: +6dB              */
#define FEQVOX007         +3            /* 3300-3800: +3dB (>3.8Kz:0dB) */

/************************************************************************/
/************************************************************************/
typedef WORD  (FAR PASCAL *CVTPOLPRC) (LPVOID, DWORD, DWORD);
typedef WORD  (FAR cdecl  *CVTERRPRC) (LPCSTR, ...);

/************************************************************************/
/************************************************************************/
typedef struct _CVTBLK {                /* PCM conversion structure     */
    WORD        usSrcFmt;               /* Source file format           */
    WORD        usSrcPCM;               /* Source PCM type              */
    WORD        usSrcChn;               /* Source file chan count       */
    WORD        usSrcBIO;               /* Source bit encoding          */
    DWORD       ulSrcFrq;               /* Source sample frequency      */
    DWORD       ulSrcOff;               /* Source byte offset           */
    DWORD       ulSrcLen;               /* Source byte length           */
    VISMEMHDL   mhSrcCfg;               /* Source cfg memory buff       */       
    WORD        usDstFmt;               /* Destination file format      */
    WORD        usDstPCM;               /* Destination PCM type         */
    WORD        usDstChn;               /* Destination file chan count  */
    WORD        usDstBIO;               /* Destination bit encoding     */
    DWORD       ulDstFrq;               /* Destination sample frequency */
    DWORD       ulDstOff;               /* Reserved                     */
    DWORD       ulDstLen;               /* Destination byte length      */
    VISMEMHDL   mhDstCfg;               /* Destination cfg memory buff  */       
    float       flCrpFrm;               /* FndSndEvt frame size   (sec) */
    float       flCrpRes;               /* FndSndEvt resolution   (sec) */
    float       flCrpSnd;               /* FndSndEvt snd thresh  (%max) */
    WORD        usCrpAtk;               /* FndSndEvt atk ratio   (%frm) */
    WORD        usCrpDcy;               /* FndSndEvt dcy ratio   (%frm) */
    float       flCrpGrd;               /* FndSndEvt guard time   (sec) */
    float       flVolAdj;               /* Volume adjust mult      (dB) */
    WORD        usNrmLvl;               /* Volume norm target level (%) */
    float       flNrmExc;               /* Volume norm except thrsh (%) */
    WORD        usNrmMax;               /* Volume norm max adjust  (dB) */
    float       flDCELvl;               /* DynCmpExp  level        (dB) */
    WORD        usCmpThr;               /* Compander  threshold     (%) */
    WORD        usCmpAtk;               /* Compander  attack time  (Hz) */
    float       flCmpMin;               /* Compander  minimum      (dB) */
    WORD        usExpThr;               /* Expander   threshold     (%) */
    WORD        usExpAtk;               /* Expander   attack time  (Hz) */
    float       flExpMax;               /* Expander   maximum      (dB) */
    WORD        usCE_Dcy;               /* Cmp / Exp  decay time   (Hz) */
    WORD        usNoiThr;               /* Noise Gate threshold     (%) */
    float       flNoiAtt;               /* Noise Gate attenuation  (dB) */
    WORD        usTDLAAF;               /* Time Domain Anti-alias (Ord) */
    BOOL        bfTDLFst;               /* Anti-alias fast math on/off  */
    BOOL        bfFFTDTM;               /* FFT DTMF filter   on/off     */
    BOOL        bfFFTAAF;               /* FFT Anti-aliasing on/off     */
    BOOL        bfFFTFEq;               /* FFT Freq Equalize on/off     */
    BOOL        bfFFTRes;               /* FFT Freq Resample on/off     */
    WORD        usFFTOrd;               /* FFT filter size              */
    float       flFEqGai;               /* Freq equalizer gain adj (dB) */
    DWORD       ulFEqBot;               /* Freq equalizer bot freq (Hz) */
    DWORD       ulFEqTop;               /* Freq equalizer top freq (Hz) */
    WORD        usFEqCnt;               /* Freq equalizer point count   */
    float       flFEqPnt[FEQMAXCNT];    /* Freq equalizer data points   */
    VISMEMHDL   mhGloCfg;               /* Global cfg memory buff       */       
    CVTPOLPRC   lpPolDsp;               /* Conversion poll display proc */
    DWORD       ulRsvPol;               /* Reserved                     */
    CVTERRPRC   lpMsgDsp;               /* Reserved                     */
    DWORD       ulRsvErr;               /* Reserved                     */
    BYTE        ucRsv001[32];           /* Reserved                     */
    WORD        usCmpCod;               /* Conversion completion code   */
    BYTE        ucRsv002[32];           /* Reserved                     */
} CVTBLK;

/************************************************************************/
/************************************************************************/
WORD    FAR PASCAL  CvtPCMVer (void);
WORD    FAR PASCAL  CvtPCMIni (WORD, DWORD, DWORD);
WORD    FAR PASCAL  CvtPCMTrm ();

WORD    FAR PASCAL  CvtCfgIni (CVTBLK FAR *);
WORD    FAR PASCAL  CvtCfgLod (LPSTR, VISMEMHDL FAR *);
WORD    FAR PASCAL  CvtCfgRel (VISMEMHDL FAR *);
DWORD   FAR PASCAL  CvtCapQry (enum _PCMTYP, DWORD, DWORD);
                    
WORD    FAR PASCAL  CvtSetDeb (WORD);                   
LPVOID  FAR PASCAL  CvtSetDsp (LPVOID);                 
                    
DWORD   FAR PASCAL  CvtPCMFil (short, short, LPCSTR, LPCSTR, LPSTR, WORD, LPVOID); 

/************************************************************************/
/*                  Initialization Return Codes                         */
/************************************************************************/
#define SI_CVTNO_ERR    0           /* No error                         */
#define SI_CVTKEYERR    1           /* Key/License error, 8 second lim  */
#define SI_CVTVERERR    2           /* Module version mismatch          */



