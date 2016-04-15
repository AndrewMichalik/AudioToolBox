/************************************************************************/
/* Generic Speed Adjust Utility Defs: GenEff.h          V2.00  08/15/95 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define SPDVERNUM       0x0300          /* Spd support ver number       */

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
typedef WORD  (FAR PASCAL *SPDPOLPRC) (LPVOID, DWORD, DWORD);
typedef WORD  (FAR cdecl  *SPDERRPRC) (LPCSTR, ...);

/************************************************************************/
/************************************************************************/
typedef struct _SPDBLK {
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
    SPDPOLPRC   lpPolDsp;               /* Conversion poll display proc */
    DWORD       ulRsvPol;               /* Reserved                     */
    SPDERRPRC   lpMsgDsp;               /* Reserved                     */
    DWORD       ulRsvErr;               /* Reserved                     */
    BYTE        ucRsv001[32];           /* Reserved                     */
    WORD        usCmpCod;               /* Reserved                     */
    BYTE        ucRsv002[32];           /* Reserved                     */
} SPDBLK;

/************************************************************************/
/************************************************************************/
WORD    FAR PASCAL  SpdAdjVer (void);
WORD    FAR PASCAL  SpdAdjIni (WORD, DWORD, DWORD);
WORD    FAR PASCAL  SpdAdjTrm ();

WORD    FAR PASCAL  SpdCfgIni (SPDBLK FAR *);
WORD    FAR PASCAL  SpdCfgLod (LPSTR, VISMEMHDL FAR *);
WORD    FAR PASCAL  SpdCfgRel (VISMEMHDL FAR *);
DWORD   FAR PASCAL  SpdCapQry (enum _PCMTYP, DWORD, DWORD);
                    
WORD    FAR PASCAL  SpdSetDeb (WORD);                   
LPVOID  FAR PASCAL  SpdSetDsp (LPVOID);                 
                    
DWORD   FAR PASCAL  SpdAdjFil (short, short, LPCSTR, LPCSTR, LPSTR, WORD, LPVOID); 

/************************************************************************/
/*                  Initialization Return Codes                         */
/************************************************************************/
#define SI_SPDNO_ERR    0           /* No error                         */
#define SI_SPDKEYERR    1           /* Acc key not installed, 8 sec lim */
#define SI_SPDVERERR    2           /* Module version mismatch          */

