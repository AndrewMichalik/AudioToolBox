/************************************************************************/
/* Generic PCM Conversion defs: GenPCM.h                V2.00  07/15/95 */
/* Copyright (c) 1987-1995 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define PCMVERNUM       0x0212          /* PCM Cvt support ver number   */
#define PCMMAXDES           40          /* Max description text length  */
#define PCMMAXCHN            8          /* Max channel count            */
#define PCMBIODEF            0          /* File Bit I/O encoding def    */

/************************************************************************/
/*                  Callback procedure definitions                      */
/************************************************************************/
typedef enum _PCMPOL {
    PCMPOLBEG,                              /* Begin poll operation     */
    PCMPOLCNT,                              /* Continuation poll op     */
    PCMPOLPOS,                              /* Position poll notify     */
    PCMPOLEMP,                              /* Empty poll operation     */
    PCMPOLEND                               /* End poll operation       */
} PCMPOL;
typedef WORD (FAR PASCAL *PCMPOLPRC)(PCMPOL,DWORD,DWORD);

/************************************************************************/
/*                      Extended error codes                            */
/************************************************************************/
#define PCM_E_UNK           0xffff          /* Unknown I/O error        */
#define PCM_E_ABT           0xfff0          /* I/O aborted              */

/************************************************************************/
/*          Generic PCM Sample: Signed, native A to D width             */
/************************************************************************/
#define GENPCM000   MPCPCM016           /* Generic PCM storage type     */
typedef short 		GENSMP;             
typedef GENSMP FAR *LPGSMP;

/************************************************************************/
/*                      PCM Data Formats                                */
/************************************************************************/
typedef enum _PCMTYP {
    UNKPCM000 =   0,                    /* Unknown conversion type      */
    AVAPCM004 =   4,                    /* AVA 4 bit                    */
    BKTPCM001 =   8,                    /* Brooktrout CVSD              */
    CPXPCM064 =  12,                    /* Complex rea/img, mag/phs x32 */
    DLGPCM004 =  16,                    /* DLG 4 bit                    */
    DLGPCM008 =  20,                    /* DLG uLaw Folded 8 bit        */
    FLTPCM032 =  24,                    /* Floating point 32 bit        */
    G11PCMF08 =  28,                    /* CCITT G.711 U-law            */
    G11PCMI08 =  32,                    /* CCITT G.711 A-law            */
    G21PCM004 =  36,                    /* CCITT G.721                  */
    G22PCM004 =  40,                    /* CCITT G.722                  */
    G23PCM003 =  44,                    /* CCITT G.723 3 bit            */
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

/************************************************************************/
/*                      PCM Query Types                                 */
/************************************************************************/
typedef enum _PCMQRY {
    UNKPCMQRY =   0,                    /* Unknown query                */
    PCMSUPQRY =   4,                    /* PCM support information      */
    PCMDEFQRY =   8,                    /* PCM type default             */
    CHNDEFQRY =  12,                    /* Channel count default        */
    FRQDEFQRY =  16,                    /* Sample frequency default     */
    ATDRESQRY =  20,                    /* A to D resolution            */
    ATDMINQRY =  24,                    /* A to D minimum               */
    ATDMAXQRY =  28,                    /* A to D maximum               */
    GRDSMPQRY =  32,                    /* Guard sample default count   */
    BLKSIZQRY =  36,                    /* Byte boundary value          */
    VIWFTRQRY =  40,                    /* View filter sample count     */
} PCMQRY;    
             
/************************************************************************/
/*                  Oscillation Display structures                      */
/************************************************************************/
typedef struct _MXMBLK {                /* Max / Min display block      */   
    GENSMP      gsMinWav;                   
    GENSMP      gsMaxWav;                   
} MXMBLK;

typedef struct _SYNBLK {                /* Synchronization block        */              
    DWORD       ulBytOff;
    DWORD       ulBlkOff;
} SYNBLK;

/************************************************************************/
/*                  PCM description & linkage structures                */
/************************************************************************/
typedef struct _TCMBLK {                /* Table coded modulation       */
    GENSMP  gsCurWav;
    long    lWavSum;
} TCMBLK;

typedef struct _APCBLK {                /* Adaptive Pulse Code Mod      */
    GENSMP  gsCurWav;            
    union   {
    long    lWavSum;
    struct  {short sLowSum; short sHghSum;};
    };
    short   sNxtSIx;
    short   sResCnt;
} APCBLK;

typedef struct _CVSINI {                /* CVSD initialization block    */
    DWORD   ulSmpFrq;
    WORD    usLowTyp;
    DWORD   ulLowPas;
    WORD    usSigFtr;     
    WORD    usSylAtk;     
    WORD    usSylDcy;     
    WORD    usCoiBit;     
    WORD    usStpMin;
    WORD    usStpMax;
    WORD    usAtDRes;
} CVSINI;

#define CVSFCBMAX   11
typedef enum _CVSFTRTYP {
    CVSNULFTR =   0,                    /* Null filter                  */
    CVSLI2LFA =   0x21,                 /* Low Pass IIR 2nd order fast  */
    CVSLI6FLT =   0x60,                 /* Low Pass IIR 6th order float */
} CVSFTRTYP;    
typedef struct _CVSFCB {                /* CVSD filter coefficient blk  */
    WORD    usLen;                      /* Filter length                */
    union   {
    long    lfCoe[CVSFCBMAX];           /* Long fraction                */
    float   flCoe[CVSFCBMAX];           /* Float                        */
    };
    union   {
    long    lfHis[CVSFCBMAX];           /* Long fraction                */
    float   flHis[CVSFCBMAX];           /* Float                        */
    GENSMP  gsHis[CVSFCBMAX];           /* Generic sample               */
    };
} CVSFCB;
typedef struct _CVSFTR {                /* CVSD filter block            */
    CVSFTRTYP   usTyp;
    WORD        usSec;
    CVSFCB      fbZer;
    CVSFCB      fbPol;
} CVSFTR;
typedef struct _CVSBLK {                /* Continuously variable slope  */
    long    lfCurWav;
    long    lfCurStp;
    long    lfSigFtr;
    long    lfSylAtk;
    long    lfSylDcy;
    long    lfMinStp;
    long    lfMaxStp;
    long    lfClpMax;       
    WORD    usCoiMsk;
    CVSFTR  cfLowPas;
} CVSBLK;

typedef struct g72x_state {
    long    yl;     /* Locked or steady state step size multiplier.     */
    short   yu;     /* Unlocked or non-steady state step size mult.     */
    short   dms;    /* Short term energy estimate.                      */
    short   dml;    /* Long term energy estimate.                       */
    short   ap;     /* Linear weighting coefficient of 'yl' and 'yu'.   */
    short   a[2];   /* Coefficients of pole portion of prediction filt. */
    short   b[6];   /* Coefficients of zero portion of prediction filt. */
    short   pk[2];  /* Signs of prev 2 smp of partially reconstructed signal.   */
    short   dq[6];  /* Previous 6 samples of the quantized difference   */
                    /* signal in internal floating point format.        */
    short   sr[2];  /* Previous 2 samples of the quantized difference   */
                    /* signal in internal floating point format.        */
    char    td;     /* delayed tone detect, new in 1988 version         */
    char    cTAj;   /* Tandem adjust type indicator                     */
} G2XBLK;

#ifndef _INC_MMREG
    /* MultiMedia                                                           */ 
    /* general waveform format structure (information common to all fmts)   */
    #define     MAXCOESET   16
    typedef struct waveformat_extended_tag {
        WORD    wFormatTag;        /* format type                           */
        WORD    nChannels;         /* #channels (i.e. mono, stereo, etc.)   */
        DWORD   nSamplesPerSec;    /* sample rate                           */
        DWORD   nAvgBytesPerSec;   /* for buffer estimation                 */
        WORD    nBlockAlign;       /* block size of data                    */
        WORD    wBitsPerSample;    /* sample size                           */
        WORD    cbSize;            /* #bytes of the size of extra info (after cbSize)   */
    } WAVEFORMATEX;
    typedef WAVEFORMATEX    FAR *LPWAVEFORMATEX;
    typedef struct adpcmcoef_tag {
        short   iCoef1;
        short   iCoef2;
    } ADPCMCOEFSET;
    typedef struct adpcmwaveformat_tag {
        WAVEFORMATEX    wfx;
        WORD            wSamplesPerBlock;
        WORD            wNumCoef;
        ADPCMCOEFSET    aCoef[MAXCOESET];
    } ADPCMWAVEFORMAT;
    typedef ADPCMWAVEFORMAT FAR *LPADPCMWAVEFORMAT;
#endif    /* _INC_MMREG */

typedef struct _MSWBLK {                /* MS Wave control block        */
    ADPCMWAVEFORMAT afWEX;
} MSWBLK;

typedef struct _RTXBLK {                /* Rhetorex proprietary ADPCM   */
    LPVOID  lpR24Cmp;
    LPVOID  lpR24Exp;
    LPVOID  lpR32Cmp;
    LPVOID  lpR32Exp;
} RTXBLK;

typedef struct _ITCINI {
    union   {
    CVSINI  ciCVS;
    };
} ITCINI;
typedef ITCINI FAR * LPITCI;

typedef struct _ITCBLK {
    WORD    usBitOff;                   /* Current sample bit   offset  */
    WORD    usBytOff;                   /* Current sample byte  offset  */
    DWORD   ulBlkOff;                   /* Current sample block offset  */
    union   {
    BYTE    ucPad[248];     /* MSC8 bug gives sizeof(*LPITCB)=59=sizeof */
                            /* G2XBlk, not 12+MSWBLK=54; force to 256   */
    TCMBLK  tbTCM;
    APCBLK  abAPC;
    CVSBLK  cbCVS;
    G2XBLK  gbG2X;
    MSWBLK  mwMSW;
    RTXBLK  rbRtx;
    };
} ITCBLK;
typedef ITCBLK FAR * LPITCB;

/************************************************************************/
/************************************************************************/
WORD    FAR PASCAL  PCMDLLIni (WORD, DWORD, DWORD); 
WORD    FAR PASCAL  PCMSupVer (void);
WORD    PASCAL FAR  PCMSetDeb (WORD);

PCMTYP  FAR PASCAL  PCMTypEnu (PCMTYP);
WORD    FAR PASCAL  PCMChnEnu (PCMTYP, WORD);
DWORD   FAR PASCAL  PCMFrqEnu (PCMTYP, DWORD);
LPSTR   FAR PASCAL  PCMDesQry (PCMTYP, LPSTR, WORD);
DWORD   FAR PASCAL  PCMCapQry (PCMTYP, DWORD, DWORD);
DWORD   FAR PASCAL  PCMByt2Sl (PCMTYP, WORD, DWORD, LPITCB);
DWORD   FAR PASCAL  PCMSmp2Bh (PCMTYP, WORD, DWORD, LPITCB);
DWORD   FAR PASCAL  PCMSmpLow (PCMTYP, WORD, DWORD, LPITCB);

DWORD   FAR PASCAL  PCMSetSil (PCMTYP, LPVOID, DWORD);

LPVOID  FAR PASCAL  PCMAloITC (PCMTYP, LPITCB, LPITCI);
LPVOID  FAR PASCAL  PCMIniITC (PCMTYP, LPITCB);
DWORD   FAR PASCAL  PCMGrdITC (short, DWORD, LPVOID, DWORD, PCMTYP, WORD,
                    WORD, LPITCB, LPVOID, WORD FAR *, PCMPOLPRC, DWORD);
LPVOID  FAR PASCAL  PCMRelITC (PCMTYP, LPITCB);

DWORD FAR PASCAL    PCMRd_G16 (short, DWORD, LPGSMP, DWORD, PCMTYP, WORD,
                    WORD, LPITCB, DWORD FAR *, WORD FAR *, PCMPOLPRC, DWORD);
DWORD FAR PASCAL    PCMWr_PCM (LPGSMP, DWORD, short, DWORD, PCMTYP, WORD,
                    WORD, LPITCB, DWORD FAR *, WORD FAR *, PCMPOLPRC, DWORD);
                   
DWORD   FAR PASCAL  PCMPtoG16 (LPSTR, DWORD, LPGSMP, DWORD, PCMTYP, 
                    WORD, WORD, LPITCB, DWORD FAR *, WORD FAR *);
DWORD   FAR PASCAL  PCMG16toP (LPGSMP, DWORD, LPSTR, DWORD, PCMTYP,
                    WORD, WORD, LPITCB, DWORD FAR *, WORD FAR *);
DWORD   FAR PASCAL  PCML16toG (short FAR *, LPGSMP, DWORD, WORD);    
DWORD   FAR PASCAL  PCMGtoL16 (LPGSMP, short FAR *, DWORD, WORD);    
DWORD   FAR PASCAL  PCMGtoATD (LPGSMP lpgsSrcBuf, short FAR *lpsDstBuf, 
                    DWORD ulSmpCnt, short usDltRes);

DWORD   FAR PASCAL  PCMRd_MxM (short, DWORD, MXMBLK FAR *, DWORD, float, LPVOID, 
                    DWORD, PCMTYP, WORD, WORD, LPITCB, DWORD FAR *, 
                    WORD FAR *, PCMPOLPRC, DWORD);
DWORD   FAR PASCAL  PCMPtoMxM (LPVOID, DWORD, MXMBLK FAR *, DWORD, float,  
                    PCMTYP, WORD, WORD, LPITCB, DWORD FAR *, 
                    WORD FAR *);
