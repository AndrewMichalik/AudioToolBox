/************************************************************************/
/* Generic Sound Chip Utility Supp Defs: LibSup.h       V2.00  01/10/96 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define TBXMAXSTR         40            /* Gen purpose string length    */
#define REGKEYLEN          8            /* Registration key length      */
#define SERSEGLEN          4            /* Serial number segment length */
#define DEMRECDEF          8            /* 8 section limit              */

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
typedef struct _CMPCBK {                /* Completion callback struct   */
    char *  pcMsgTxt;                   /* Completion message text      */
    DWORD   ulInpCnt;                   /* Input  count                 */
    float   flInpFac;                   /* Input  completion factor     */
    DWORD   ulOutCnt;                   /* Output count                 */
    float   flOutFac;                   /* Output completion factor     */
} CMPCBK;

/************************************************************************/
/************************************************************************/
#define ERR___DEB       0x0002          /* Debug error messages flag    */
#define KEY___DEB       0x0008          /* Debug key flag               */
#define MEM___DEB       0x0020          /* Debug memory messages flag   */
#define CFG___DEB       0x0080          /* Debug config settings flag   */
#define INI___DEB       0x0200          /* Debug init   settings flag   */

/************************************************************************/
/*                      Error Message Support                           */
/************************************************************************/
#define CHPERRMSG if (TBxGlo.usDebFlg & ERR___DEB) MsgDspUsr

/************************************************************************/
/************************************************************************/
#define SI_MODVERMIS "Software module version mismatch: %s DLL\n"
#define SI_EFFDLLSUP "Effects"
#define SI_PCMDLLSUP "PCM Conversion"
#define SI_INSMEMERR "Insufficient main memory.\n"
#define SI_DOSMEMERR "Insufficient DOS global memory.\nSome functions will be inhibited.\n"
#define SI_ACCKEYNON "Accelerator Key not installed!\n"

