/************************************************************************/
/* Generic PCM Conversion Utility Defs: GenEff.h        V2.00  02/10/93 */
/* Copyright (c) 1987-1991 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define SRCFRQMIN       1000L           /* Minimum frequency (Hz)       */
#define SRCFRQMAX      65000L           /* Maximum frequency (Hz)       */

#define FORALLVER       TRUE            /* Function avail on all ver    */
#define FFTORDDEF          9            /* FFT filter size              */
#define WINORDDEF          9            /* FFT window size              */
#define SPDMULDEF  (float) 1            /* Speed multiplier default     */
#define PCHMULDEF  (float) 1            /* Pitch multiplier default     */
#define SYNTHRDEF  (float)  .0001       /* Synthesis threshold default  */
                                        /* Future: -60 dB of high sig   */
#define LOGBASTWO(x)    (log(x)/log(2)) /* Log base 2                   */
#define USEERREND          1            /* User termination exit code   */

/************************************************************************/
/************************************************************************/
#define ERR___DEB       0x0002          /* Debug error messages flag    */
#define MEM___DEB       0x0020          /* Debug memory messages flag   */
#define CFG___DEB       0x0080          /* Debug config settings flag   */
#define INI___DEB       0x0200          /* Debug init   settings flag   */

/************************************************************************/
/************************************************************************/
typedef struct _CFGBLK {
    WORD        usRepFlg;               /* File replacement flags       */
    char        szSrcCfg[FIOMAXNAM];    /* Input  config file name      */       
    char        szDstCfg[FIOMAXNAM];    /* Output config file name      */       
    char        szGloCfg[FIOMAXNAM];    /* Global config file name      */       
} CFGBLK;

typedef struct _FRQBLK {
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
    DWORD       ulDstOff;               /* Destination byte offset      */
    DWORD       ulDstLen;               /* Destination byte length      */
    VISMEMHDL   mhDstCfg;               /* Destination cfg memory buff  */       
    float       flVolAdj;               /* Volume adjust multiplier     */
    WORD        usFFTOrd;               /* FFT filter size              */
    WORD        usWinOrd;               /* FFT window size              */
    float       flSpdMul;               /* Vocoder speed multiplier     */
    float       flPchMul;               /* Vocoder pitch multiplier     */
    float       flSynThr;               /* Vocoder synthesis threshold  */
    BOOL        bfFrcOBS;               /* Force Osc Bank Syn flag      */
    VISMEMHDL   mhGloCfg;               /* Global cfg memory buff       */       
    DWORD       ulTotCnt;               /* Total bytes processed count  */
} FRQBLK;

typedef struct _CMPCBK {                /* Completion callback struct   */
    char *  pcMsgTxt;                   /* Completion message text      */
    DWORD   ulInpCnt;                   /* Input  count                 */
    float   flInpFac;                   /* Input  completion factor     */
    DWORD   ulOutCnt;                   /* Output count                 */
    float   flOutFac;                   /* Output completion factor     */
} CMPCBK;

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL EffFrqIni (LPSTR, LPSTR FAR *);
WORD FAR PASCAL EffFrqTrm ();
WORD FAR PASCAL EffSetDeb (WORD);
WORD FAR PASCAL CvtSetDsp (LPVOID);

/************************************************************************/
/************************************************************************/
char *  GetDshPrm (WORD, char **, CFGBLK *, FRQBLK *);
int     GetCfgFil (char *, VISMEMHDL *);
int     RelCfgFil (VISMEMHDL *);





