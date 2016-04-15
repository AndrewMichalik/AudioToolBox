/************************************************************************/
/* Generic Sound Effects Definintions: GenEff.h         V2.00  03/15/95 */
/* Copyright (c) 1987-1995 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define EFFVERNUM       0x0212          /* Effects support ver number   */

#define dbPI        3.141592653589793   /* Pi                           */
#define db2PI       6.28318530717959    /* 2 * Pi                       */

/************************************************************************/
/************************************************************************/
#define EFFFFTFWD   +1                  /* FFT forward transform flag   */
#define EFFFFTINV   -1                  /* FFT reverse transform flag   */

/************************************************************************/
/************************************************************************/
typedef long        LNGFRA;                 /* long "fractional" value  */
#define LNGFRANRM   10                      /* LNGFRA fract bit count   */
#define LNGFRAONE   (1L << LNGFRANRM)       /* Normalized unity value   */   
#define LNGFRAMUL(x,y) ((((x)>>3)*((y)>>3))>>4) /* Mul w/o und/ovr flow */
#define LNGFRAMAC(x,y) (((x)>>3)*((y)>>3))  /* Mul/Acc w/o und/ovr flow     */
#define LNGFRAACC(z)   ((z)>>4)             /* Mul/Acc w/o und/ovr flow  */

/************************************************************************/
/*                      PCM Data Formats                                */
/*          Generic Sample: Signed, native A to D width                 */
/************************************************************************/
#if (!defined (PCMVERNUM))
    typedef short  GENSMP;                  /* Generic PCM storage type */
    typedef GENSMP FAR *LPGSMP;
#endif

/************************************************************************/
/*                  Callback procedure definitions                      */
/************************************************************************/
typedef enum _EFFPOL {
    EFFPOLBEG,                              /* Begin poll operation     */
    EFFPOLCNT,                              /* Continuation poll op     */
    EFFPOLPOS,                              /* Position poll notify     */
    EFFPOLEMP,                              /* Empty poll operation     */
    EFFPOLEND                               /* End poll operation       */
} EFFPOL;
typedef WORD (FAR PASCAL *EFFPOLPRC)(EFFPOL,DWORD,DWORD);

/************************************************************************/
/*                      Vocoder selection codes                         */
/************************************************************************/
typedef enum _VOCTYP {
    VOCMSTANA = 0,     
    VOCMSTSYN,
} VOCTYP;

/************************************************************************/
/*                  Effects Buffer Stream return codes                  */
/************************************************************************/
typedef enum _EOSCOD {
    FIOEOS_OK = 0,     
    FIOEOSEOF,
    FIOEOSCAN,                          /* User canceled operation      */
    FIOEOSERR
} EOSCOD;

/************************************************************************/
/*                  Time Domain Filter Design Support                   */
/************************************************************************/
typedef enum _WINTYP {
    EFFWINNON = 0,                      /* No window                    */
    EFFWINREC = 1,                      /* Rectangular                  */
    EFFWINTAP = 2,                      /* Tapered rectangular          */
    EFFWINTRI = 3,                      /* Triangular                   */
    EFFWINHAN = 4,                      /* Hanning                      */
    EFFWINHAM = 5,                      /* Hamming                      */
    EFFWINBLK = 6,                      /* Balckman                     */
} WINTYP;

/************************************************************************/
/*                      Function Data types                             */
/************************************************************************/
typedef struct _CPXFLT {                /* Complex float                */
        float flRea;
        float flImg;
} CPXFLT;   
typedef CPXFLT FAR *LPCPXF;

typedef struct _MGAFLT {                /* Magnitude / Angle float      */
        float flMag;
        float flAng;
} MGAFLT;   
typedef MGAFLT FAR *LPMGAF;

/************************************************************************/
/*                      Function Data Structures                        */
/************************************************************************/
typedef struct _EBSBLK {                /* Effects Buffer Stream block  */
    VISMEMHDL       mhBufHdl;           /* PCM buffer (interleaved)     */
    enum _PCMTYP    usPCMTyp;
    WORD            usAtDRes;
    WORD            usChnCnt;
    DWORD           ulSmpFrq;
    DWORD           ulSmpCnt;           /* Sample count (per channel)   */
    BOOL            usEOSCod;
    DWORD           ulRsv001;
    BYTE            ucPad[16];
} EBSBLK;
typedef EBSBLK FAR *LPEBSB;

typedef enum _PASTYP {
    EFFPASNON = 0,                      /* No pass type                 */
    EFFPASLOW,                          /* Low pass                     */
    EFFPASHGH,                          /* High pass                    */
    EFFPASBND,                          /* Band pass                    */
    EFFPASSTP,                          /* Band stop                    */
} PASTYP;
typedef enum _FTRTYP {
    EFFFTRNON = 0,                      /* No filter                    */
    EFFIIRCH1 = 1,                      /* Chebyshev Type I             */
    EFFIIRCH2 = 2,                      /* Chebyshev Type II            */
    EFFIIRBUT = 3,                      /* Butterworth                  */
    EFFFIRXXX = 4,                      /* FIR: All types               */
} FTRTYP;
#define TDFBLKMAX 11  
typedef struct _TDFBLK {                /* Time domain filter coe/his   */
    WORD    usLen;                      /* Filter length                */
    union   {
    long    lfCoe[TDFBLKMAX];           /* Long fraction                */
    float   flCoe[TDFBLKMAX];           /* Float                        */
    };
    union   {
    long    lfHis[TDFBLKMAX];           /* Long fraction                */
    float   flHis[TDFBLKMAX];           /* Float                        */
    GENSMP  gsHis[TDFBLKMAX];           /* Generic sample               */
    };
} TDFBLK;
typedef struct _TDOFTR {                /* Time domain filter           */
    FTRTYP  usTyp;                      /* Filter type                  */
    WORD    usSec;                      /* Section count                */
    BOOL    bfFst;                      /* Fast (long fraction) flag    */
    TDFBLK  tbZer;                      /* Zeros                        */
    TDFBLK  tbPol;                      /* Poles                        */
} TDOFTR;
typedef TDOFTR FAR * LPTDOF;

typedef struct _OVRBLK {                /* FFT Overlap-save block       */
    VISMEMHDL   mhWrkHdl;               /* FFT function work buf handle */
    DWORD       ulSrcOff;
    BYTE        ucPad[16];
} OVRBLK;
typedef OVRBLK FAR * LPOVRB;
typedef struct _FFTBLK {                /* FFT filter block             */
    DWORD       ulFFTPts;
    VISMEMHDL   mhWinHdl;               /* Window function handle       */
    VISMEMHDL   mhXfrHdl;               /* Transfer function handle     */
    float       flResRat;
    EFFPOLPRC   fpPolPrc;
    DWORD       ulPolDat;
    LPVOID      lpUsrDat;
    OVRBLK      obOvrBlk;
    BYTE        ucPad[16];
} FFTBLK;
typedef FFTBLK FAR * LPFFTB;
typedef WORD (FAR PASCAL *EFFFFTCBK) (float FAR * FAR *, DWORD FAR *, DWORD, 
        DWORD, DWORD, DWORD, DWORD, LPVOID);

typedef struct _FADBLK {                /* Fade In/Out block            */
    WORD        usFadAlg;
    float       flFadBas;
    float       flFadRat;
    DWORD       ulSmpPos;
    BYTE        ucPad[16];
} FADBLK;
typedef FADBLK FAR * LPFADB;

typedef struct _ECHBLK {                /* Echo (decayed) block         */
    WORD        usEchAlg;
    DWORD       ulDlySmp;
    float       flDec_dB;
    EBSBLK      ebDlyEBS;
    BYTE        ucPad[16];
} ECHBLK;
typedef ECHBLK FAR * LPECHB;

typedef struct _RESBLK {                /* Frequency re-sample block    */
    DWORD       ulPhsPos;
    GENSMP      gsCurWav; 
    BYTE        ucPad[16];
} RESBLK;
typedef RESBLK FAR * LPRESB;

typedef struct _DCEBLK {                /* Dynamic Compress / Expand    */
    DWORD       ulAtDMax;
    DWORD       ulCmpThr;
    float       flCmpAtk;
    float       flCmpMin;
    float       flCmpLvl;
    DWORD       ulExpThr;
    float       flExpAtk;
    float       flExpMax;
    float       flExpLvl;
    float       flCE_Dcy;
    DWORD       ulNoiThr;
    float       flNoiMul;
    DWORD       ulEnvNrm;
    BYTE        ucPad[16];
} DCEBLK;
typedef DCEBLK FAR * LPDCEB;

typedef enum _FSEEVT {
    EFFFSENON = 0,                      /* No sound event               */
    EFFFSEINI,                          /* Initialization event         */
    EFFFSEPOL,                          /* Poll event                   */
    EFFFSESND,                          /* Change to sound event        */
    EFFFSESIL,                          /* Change to silence event      */
    EFFFSETRM,                          /* Termination event            */
} FSEEVT;
typedef struct _FSEBLK {                /* Find Sound Event block       */
    WORD        usPCMTyp;
    WORD        usChnCnt;
    WORD        usBIOEnc;
    DWORD       ulSmpFrq; 
    VISMEMHDL   mhFrmHdl;
    DWORD       ulFrmLen;
    DWORD       ulBinPos;
    DWORD       ulBinOff;
    DWORD       ulResCnt;
    DWORD       ulAtkCnt;               /* Silence to sound count       */
    DWORD       ulDcyCnt;               /* Sound to silence count       */
    DWORD       ulAtkGrd;               /* Sil to snd guard samples     */
    DWORD       ulDcyGrd;               /* Snd to sil guard samples     */
    DWORD       ulSndThr;               /* Sound threshold abs level    */
    BOOL        bfPrvSnd;               /* Previously at sound level?   */ 
    TDOFTR      tfHghPas;               /* High pass DC filter block    */
    BYTE        ucPad[16];
} FSEBLK;
typedef struct _FSEREC {                /* Find Sound Event record      */
    DWORD       ulBegPos;               /* First active sound position  */
    DWORD       ulEndPos;               /* Last  active sound position  */
    DWORD       ulUsrDat;               /* User specified data          */
    BYTE        ucPad[4];
} FSEREC;
typedef FSEREC FAR * LPFSER;
typedef FSEBLK FAR * LPFSEB;
typedef WORD (FAR PASCAL *EFFFSECBK)(FSEEVT,DWORD,DWORD);

/************************************************************************/
/************************************************************************/
typedef struct _MSTBLK {                /* Modified Short-term Transfer */
    float       flDifFac;               // Eliminate in future, double conv!           
    VISMEMHDL   mhWinBuf;
    VISMEMHDL   mhPhsBuf;
    VISMEMHDL   mhWrkBuf;
    BYTE        ucPad[16];
} MSTBLK;
typedef MSTBLK FAR * LPMSTB;

typedef struct _OBSBLK {                /* Oscillator Bank Synthesis    */
    float       flTabInc;               // Eliminate in future, double conv!           
    VISMEMHDL   mhPrvBuf;
    VISMEMHDL   mhCosBuf;
    VISMEMHDL   mhIdxBuf;
    VISMEMHDL   mhWrkBuf;
    BYTE        ucPad[16];
} OBSBLK;
typedef OBSBLK FAR * LPOBSB;

typedef struct _VOCBLK {                /* General purpose Vocoder blk  */
    long        lInpTim;
    long        lOutTim;
    DWORD       ulInpNum;
    DWORD       ulOutDen;    
    float       flBinFrq;               // Eliminate in future, double conv!     
    float       flPchMul;
    float       flSynThr; 
    DWORD       ulFFTPts;
    DWORD       ulWinPts;
    DWORD       ulDlyPts;
    MSTBLK      mbInpMST;
    MSTBLK      mbOutMST;
    OBSBLK      obOutOBS;
    EFFFFTCBK   lpVocFnc;
    LPVOID      lpUsrDat;
    OVRBLK      obOvrBlk;
    BYTE        ucPad[16];
} VOCBLK;
typedef VOCBLK FAR * LPVOCB;

/************************************************************************/
/*                      Extended error codes                            */
/************************************************************************/
#define EFF_E_UNK           0xffff          /* Unknown I/O error        */
#define EFF_E_ABT           0xfff0          /* I/O aborted              */

/************************************************************************/
/*              Effects Resource Support Routines                       */       
/************************************************************************/
WORD    FAR PASCAL  EffSupVer (void);

/************************************************************************/
/*                  EBS block PCM I/O callback routines                 */
/************************************************************************/
typedef DWORD (FAR PASCAL *EBSFRDPRC) (short, DWORD, GENSMP FAR *, DWORD, 
        enum _PCMTYP, WORD, WORD, struct _ITCBLK FAR *, DWORD FAR *, 
        WORD FAR *, EFFPOLPRC, DWORD);
typedef DWORD (FAR PASCAL *EBSMRDPRC) (LPVOID, DWORD, GENSMP FAR *, DWORD, 
        enum _PCMTYP, WORD, WORD, struct _ITCBLK FAR *, DWORD FAR *, 
        WORD FAR *);
typedef DWORD (FAR PASCAL *EBSFWRPRC) (GENSMP FAR *, DWORD, short, DWORD, 
        enum _PCMTYP, WORD, WORD, struct _ITCBLK FAR *, DWORD FAR *, 
        WORD FAR *, EFFPOLPRC, DWORD);
typedef DWORD (FAR PASCAL *EBSMWRPRC) (GENSMP FAR *, DWORD, LPVOID, DWORD, 
        enum _PCMTYP, WORD, WORD, struct _ITCBLK FAR *, DWORD FAR *, 
        WORD FAR *);

/************************************************************************/
/*                      File scanning routines                          */
/************************************************************************/
WORD    FAR PASCAL  EffNrmLvl (short, DWORD, float, float, float, float, 
                    float, enum _PCMTYP, WORD, WORD, DWORD, float FAR *, 
                    struct _ITCBLK FAR *, EFFPOLPRC, DWORD, EBSFRDPRC);
WORD    FAR PASCAL  EffFSEAlo (float, float, float, float, float, float, 
                    float, DWORD, enum _PCMTYP, WORD, WORD, DWORD, LPFSEB); 
WORD    FAR PASCAL  EffFSEEnu (short, DWORD, LPFSEB, struct _ITCBLK FAR *, 
                    EFFFSECBK, DWORD, EBSFRDPRC);
WORD    FAR PASCAL  EffFSERel (LPFSEB);
DWORD   FAR PASCAL  EffFSEChp (LPEBSB, LPEBSB, WORD, LPFSER);

/************************************************************************/
/* EBS Functions:   if output not blocked, return 0L to n               */
/*                  if output blocked, free input and return -1L        */
/*                                     output marked as EOS             */
/************************************************************************/
DWORD   FAR PASCAL  EffChXtoM (LPEBSB, LPEBSB, WORD);
DWORD   FAR PASCAL  EffVolAdj (LPEBSB, LPEBSB, float);
DWORD   FAR PASCAL  EffMixMul (LPEBSB FAR *, LPEBSB, WORD, float FAR *);
DWORD   FAR PASCAL  EffNoiRed (LPEBSB, LPEBSB, DWORD, float);
DWORD   FAR PASCAL  EffL16toG (LPEBSB, LPEBSB, WORD);
DWORD   FAR PASCAL  EffGtoL16 (LPEBSB, LPEBSB, WORD);
DWORD   FAR PASCAL  EffVolAtD (LPEBSB, LPEBSB, WORD);
DWORD   FAR PASCAL  EffFrqRes (LPEBSB, LPEBSB, DWORD, LPRESB);
DWORD   FAR PASCAL  EffFFTFtr (LPEBSB, LPEBSB, LPFFTB, EFFFFTCBK, LPVOID);
DWORD   FAR PASCAL  EffFFTVoc (LPEBSB, LPEBSB, LPVOCB, EFFPOLPRC, DWORD);

/************************************************************************/
/*                          Volume Effects                              */
/************************************************************************/
WORD    FAR PASCAL  EffFadAlo (WORD, float, float, DWORD, LPFADB);
DWORD   FAR PASCAL  EffFad_IO (LPEBSB, LPEBSB, LPFADB);
WORD    FAR PASCAL  EffFadRel (LPFADB);

WORD    FAR PASCAL  EffEchAlo (WORD, DWORD, float, LPECHB);
DWORD   FAR PASCAL  EffEchDec (LPEBSB, LPEBSB, LPECHB);
WORD    FAR PASCAL  EffEchRel (LPECHB);

/************************************************************************/
/*                  EBS block File and Memory I/O routines              */
/************************************************************************/
DWORD   FAR PASCAL  EffPtoEBS (short, DWORD FAR *, LPEBSB, DWORD, enum _PCMTYP,
                    WORD, WORD, DWORD, struct _ITCBLK FAR *, EBSFRDPRC);
DWORD   FAR PASCAL  EffMtoEBS (LPVOID, DWORD FAR *, LPEBSB, DWORD, enum _PCMTYP,
                    WORD, WORD, DWORD, struct _ITCBLK FAR *, EBSMRDPRC);
DWORD   FAR PASCAL  EffEBStoE (LPEBSB, LPEBSB, DWORD);
DWORD   FAR PASCAL  EffEBStoP (LPEBSB, DWORD, short, DWORD, enum _PCMTYP, 
                    WORD, struct _ITCBLK FAR *, EBSFWRPRC);
DWORD   FAR PASCAL  EffEBStoM (LPEBSB, DWORD, LPVOID, DWORD, enum _PCMTYP, 
                    WORD, struct _ITCBLK FAR *, EBSMWRPRC);

/************************************************************************/
/*                          Callback Effects                            */
/************************************************************************/
WORD    FAR PASCAL  EffFFTAlo (WORD, float, LPFFTB, EFFPOLPRC, DWORD );
WORD    FAR PASCAL  EffFFTRel (LPFFTB);
WORD    FAR PASCAL  EffEquAlo (DWORD, DWORD, DWORD, WORD, float FAR *, float, LPFFTB);
WORD    FAR PASCAL  EffEquRel (LPFFTB);

WORD    FAR PASCAL  EffFIRAlo (PASTYP, WORD, DWORD, DWORD, float, float, 
                    DWORD, BOOL, LPTDOF);
DWORD   FAR PASCAL  EffFIRFtr (LPEBSB, LPEBSB, LPTDOF);
WORD    FAR PASCAL  EffFIRRel (LPTDOF);
WORD    FAR PASCAL  EffIIRAlo (FTRTYP, PASTYP, WORD, WORD, DWORD, DWORD, 
                    float, float, DWORD, BOOL, LPTDOF);
DWORD   FAR PASCAL  EffIIRFtr (LPEBSB, LPEBSB, LPTDOF);
WORD    FAR PASCAL  EffIIRRel (LPTDOF);

WORD    FAR PASCAL  EffDCEAlo (WORD, WORD, DWORD, float, DWORD, float,
                    float, DWORD, float, DWORD, float, float, LPDCEB);
DWORD   FAR PASCAL  EffVolDCE (LPEBSB, LPEBSB, LPDCEB);
WORD    FAR PASCAL  EffDCERel (LPDCEB);

/************************************************************************/
/*                          Low Level Effects                           */
/************************************************************************/
DWORD   FAR PASCAL  EffChnSSz (DWORD, WORD, WORD);
DWORD   FAR PASCAL  EffChnDSz (DWORD, WORD, WORD);
DWORD   FAR PASCAL  EffChXtoX (GENSMP FAR *, DWORD, WORD, WORD, WORD);

LPRESB  FAR PASCAL  EffResSet (LPRESB);
DWORD   FAR PASCAL  EffResSSz (DWORD, DWORD, DWORD, LPRESB);
DWORD   FAR PASCAL  EffResDSz (DWORD, DWORD, DWORD, LPRESB);
DWORD   FAR PASCAL  EffIntDec (GENSMP FAR *, GENSMP FAR *, DWORD, DWORD, DWORD, 
                    LPRESB, WORD FAR *, EFFPOLPRC, DWORD);
         
WORD    FAR PASCAL  EffVocAlo (EFFFFTCBK, LPVOCB, LPVOID, WORD, WORD, float, float, float, DWORD);
WORD    FAR PASCAL  MSTVocMST (float FAR * FAR *, DWORD FAR *, DWORD, DWORD, DWORD, DWORD, DWORD, VOCBLK FAR *);
WORD    FAR PASCAL  MSTVocOBS (float FAR * FAR *, DWORD FAR *, DWORD, DWORD, DWORD, DWORD, DWORD, VOCBLK FAR *);
DWORD   FAR PASCAL  MSTVocAna (float FAR *, DWORD, long, DWORD, DWORD, float, LPMSTB);
DWORD   FAR PASCAL  MSTVocSyn (float FAR *, DWORD, long, DWORD, DWORD, float, LPMSTB);
DWORD   FAR PASCAL  OBSVocSyn (float FAR *, DWORD, long, DWORD, DWORD, float, float, LPOBSB);
WORD    FAR PASCAL  EffVocRel (LPVOCB);

DWORD   FAR PASCAL  EffFFTOvr (LPEBSB, LPEBSB, DWORD, DWORD, DWORD, 
                    LPOVRB, EFFFFTCBK, LPVOID, EFFPOLPRC, DWORD);
void    FAR PASCAL  EffFFTRea (float FAR *, WORD,  short);
WORD    FAR PASCAL  EffWinRsp (enum _WINTYP, CPXFLT FAR *, DWORD); 
DWORD   FAR PASCAL  EffWinGen (WINTYP, float FAR *, DWORD);

