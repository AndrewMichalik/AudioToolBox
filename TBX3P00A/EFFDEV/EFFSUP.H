/************************************************************************/
/* Sound Effects Support Definitions: EffSup.h          V2.00  03/15/95 */
/* Copyright (c) 1987-1995 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
/************************************************************************/
/*                  Array/String Length Constants                       */
/************************************************************************/
#define EFFMAXSTR      40               /* Gen purpose string length    */
#define EFFMAXMSG      81               /* Message Box string length    */
#define REGKEYLEN       8               /* Registration key length      */
#define SERSEGLEN       4               /* Serial number segment length */
#define EFFLOCBAS     256               /* Effects DLL local  base msg  */
#define BYTMAXFIO  0xfc00L              /* Maximum file I/O byte count  */
#define EBSCNTDEF    8192L              /* Default Eff Buf Strm smp cnt */
#define EBSCNTMIN    1024L              /* Minimum Eff Buf Strm smpcnt  */
#define EBSPCMDEF       0               /* Default EBS PCM indicator    */
#define FRQTOFCON(f,s) (1-exp((-db2PI*f)/s))    /* Frq to sample period */
#define DB_TO_VLT(x)   pow(10,((double)(x))/20) /* dB to voltage        */
#define VLTTO_DB_(x)   (20*log10(x))            /* Voltage to dB        */
#define ATDTO_MXM(x)   ((pow(2,x)/2)-1)         /* A to D to max/min    */
#define CLPATDGEN(x,m) (max(min(x,m),-m))       /* A to D max/min clip  */
#define LOGBASTWO(x)    (log(x)/log(2))     	/* Log base 2           */

/************************************************************************/
/*              Declarations for Windows KERNEL Functions               */
/************************************************************************/
#if !(defined (WM_NULL))                                /* No Win       */ 
    typedef WORD        HANDLE;
    typedef WORD        HWND;
#elif !(defined (WF_ENHANCED))                          /* Win 2.x      */ 
#elif !(defined (WF_PAGING))                            /* Win 2 & 3.0  */ 
#elif (defined (WF_PAGING))                             /* Win 3.1 only */
#endif

/************************************************************************/
/*              Global Data Structures Definitions                      */
/************************************************************************/
typedef struct _EFFGLO {
    char    szLibNam  [EFFMAXSTR];      /* Library name                 */
    char    RelKeyArr [REGKEYLEN];      /* Operating/Demo Only Key      */
    char    SeqNumArr [SERSEGLEN];      /* Op/Demo sequence number      */
    HANDLE  hLibIns;                    /* Global instance handle       */
} EFFGLO;

/************************************************************************/
/*                          Resource Control                            */
/************************************************************************/
WORD    EffResIni   (HANDLE);
WORD    EffResTrm   (HANDLE);

/************************************************************************/
/*                      Effects Buffer Stream Support                   */
/************************************************************************/
DWORD   FAR PASCAL  EBSAloReq (LPEBSB, enum _PCMTYP, WORD, WORD, DWORD, DWORD);
DWORD   FAR PASCAL  EBSAloMem (LPEBSB, enum _PCMTYP, WORD, WORD, DWORD, DWORD);
EOSCOD  FAR PASCAL  EBSUpdRel (LPEBSB, LPVOID, DWORD);

DWORD   FAR PASCAL  EBSSmp2Bh (enum _PCMTYP, WORD, DWORD);
DWORD   FAR PASCAL  EBSByt2Sl (enum _PCMTYP, WORD, DWORD);
DWORD   FAR PASCAL  EBSSmpLow (enum _PCMTYP, WORD, DWORD);
DWORD   FAR PASCAL  EBSAtDRes (enum _PCMTYP);

/************************************************************************/
/*                      Phase Vocoder Support                           */
/************************************************************************/
WORD               VocMSTAlo (LPMSTB, DWORD, DWORD, DWORD, DWORD, DWORD, VOCTYP);
WORD               VocMSTRel (LPMSTB);
WORD               VocOBSAlo (LPOBSB, DWORD, DWORD, DWORD, DWORD, DWORD);
WORD               VocOBSRel (LPOBSB);

/************************************************************************/
/*                  Time Domain Filter Design Support                   */
/************************************************************************/
WORD    EffIIROpt (FTRTYP, PASTYP, WORD, WORD, DWORD, float, float, DWORD, BOOL, LPTDOF);
WORD    EffFIRNOp (WORD, float, WINTYP, BOOL, LPTDOF);
double  EffFIRWin (WINTYP, WORD, WORD);

/************************************************************************/
/*                          FFT Support                                 */
/************************************************************************/
void    FFTCpx  (float FAR *data, int nn, int isign);

/************************************************************************/
/*                      Local Usage Text Strings                        */
/************************************************************************/
#define SI_LOCNAMSTR        1 + EFFLOCBAS   /* Name string              */
#define SI_LOCINSMEM        2 + EFFLOCBAS   /* Insufficient Memory      */
#define SI_LOCNOLMEM        3 + EFFLOCBAS   /* Cannot Lock Memory       */
#define SI_LOCNULMEM        4 + EFFLOCBAS   /* Cannot Unlock Memory     */

#define SI_EFFLIBNAM        "Effects"       /* Library name             */

