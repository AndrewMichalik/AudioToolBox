/************************************************************************/
/* File I/O Support Definitions: FIOSup.h               V2.00  07/15/94 */
/* Copyright (c) 1987-1992 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
/************************************************************************/
/*                  Array/String Length Constants                       */
/************************************************************************/
#define FIOMAXSTR      40               /* Gen purpose string length    */
#define FIOMAXMSG      81               /* Message Box string length    */
#define REGKEYLEN       8               /* Registration key length      */
#define SERSEGLEN       4               /* Serial number segment length */
#define FIOLOCBAS     256               /* File DLL local  base msg     */
#define FIOGLOBLK    1024L              /* File I/O memory blk size     */
#define FIOGLOMAX      62L              /* File I/O max memory blk cnt  */
#define FIOGLOMIN       2L              /* File I/O min memory blk cnt  */

/************************************************************************/
/*              Declarations for Windows KERNEL Functions               */
/************************************************************************/
#if !(defined (WM_NULL))                                /* No Win       */ 
    typedef WORD        HWND;
    typedef long        LONG;
    typedef WORD        HANDLE;
    typedef WORD        HBRUSH;
    typedef short       LPOFSTRUCT;
#elif !(defined (WF_ENHANCED))                          /* Win 2.x      */ 
#elif !(defined (WF_PAGING))                            /* Win 2 & 3.0  */ 
#elif (defined (WF_PAGING))                             /* Win 3.1 only */
#endif

/************************************************************************/
/*              Global Data Structures Definitions                      */
/************************************************************************/
typedef struct _FIOGLO {
    char    szLibNam  [FIOMAXSTR];      /* Library name                 */
    char    RelKeyArr [REGKEYLEN];      /* Operating/Demo Only Key      */
    char    SeqNumArr [SERSEGLEN];      /* Op/Demo sequence number      */
    DWORD   ulDBxClr;                   /* Dlg box background RGB value */
    HANDLE  hLibIns;                    /* Global instance handle       */
    HBRUSH  hDBxBsh;                    /* Dlg box background brush     */
} FIOGLO;

/************************************************************************/
/*      These procedures are exported and called by Windows.            */
/************************************************************************/
BOOL    FAR PASCAL  MsgAskFmtPrc (HWND, unsigned, WORD, LONG);
BOOL    FAR PASCAL  SelFilIdxPrc (HWND, unsigned, WORD, LONG);

/************************************************************************/
/*                          Resource Control                            */
/************************************************************************/
WORD    FIOResIni   (HANDLE);
WORD    FIOResTrm   (HANDLE);

/************************************************************************/
/*                  OFS File Support Operations                         */
/************************************************************************/
short   FAR PASCAL OFSFilOpn (LPOFSTRUCT, WORD, WORD);

/************************************************************************/
/*                  Configuration Profile Strings                       */
/************************************************************************/
#define SI_DBXBKGCLR       16 + FIOLOCBAS   /* Dialog box bkg color     */

/************************************************************************/
/************************************************************************/
#define SI_FILFMTUNK       32 + FIOLOCBAS   /* Unknown file format      */
#define SI_FILFMTHDR       33 + FIOLOCBAS   /* Headered file format     */
#define SI_FILFMTPL2       34 + FIOLOCBAS   /* PowerLine file format    */
#define SI_FILFMTPUR       35 + FIOLOCBAS   /* Pure file format         */
#define SI_FILFMTSU4       36 + FIOLOCBAS   /* Summa Four file format   */
#define SI_FILFMTVBS       37 + FIOLOCBAS   /* VBase file format        */
#define SI_FILFMTWAV       38 + FIOLOCBAS   /* MM Wave file format      */

#define SI_FILEXTUNK       48 + FIOLOCBAS   /* Unknown file extension   */
#define SI_FILEXTHDR       49 + FIOLOCBAS   /* Headered file format     */
#define SI_FILEXTPL2       50 + FIOLOCBAS   /* PowerLine file extension */
#define SI_FILEXTPUR       51 + FIOLOCBAS   /* Pure file extension      */
#define SI_FILEXTSU4       52 + FIOLOCBAS   /* Pure file extension      */
#define SI_FILEXTVBS       53 + FIOLOCBAS   /* VBase file extension     */
#define SI_FILEXTWAV       54 + FIOLOCBAS   /* MM Wave file extension   */

#define SI_IDXINSMEM       64 + FIOLOCBAS   /* Insufficient index mem   */
#define SI_IDXSELNEW       65 + FIOLOCBAS   /* Select index:            */
#define SI_IDXCREBFR       66 + FIOLOCBAS   /* Create index before:     */
#define SI_IDXSELDEL       67 + FIOLOCBAS   /* Delete index:            */
#define SI_IDXCNTREQ       68 + FIOLOCBAS   /* Enter Index Count:       */
#define SI_IDXEOFTXT       69 + FIOLOCBAS   /* ** File End **           */

/************************************************************************/
/*                  File I/O Library reported Errors                    */
/************************************************************************/
#define SI_FIOUNKERR       80 + FIOLOCBAS   /* Unknown file I/O error   */
#define SI_UNKACCERR       81 + FIOLOCBAS   /* Unknown file access err  */
#define SI_FILEXIOVR       82 + FIOLOCBAS   /* File Exists, Overwrite?  */
#define SI_INVFILFMT       83 + FIOLOCBAS   /* Invalid File Format      */
#define SI_INVPCMFMT       84 + FIOLOCBAS   /* Invalid File PCM Format  */
#define SI_BADOVRWRT       85 + FIOLOCBAS   /* Cannot Overwrite File    */
#define SI_BADOPNSRC       86 + FIOLOCBAS   /* Cannot Open Source File  */
#define SI_BADOPNDST       87 + FIOLOCBAS   /* Cannot Open Output File  */
#define SI_BADUPDWRK       88 + FIOLOCBAS   /* Cannot Update Work File  */
#define SI_FILCOPERR       89 + FIOLOCBAS   /* File Copy Error          */
#define SI_FILSHFERR       90 + FIOLOCBAS   /* File Shift Error         */
#define SI_FILLCKORO       91 + FIOLOCBAS   /* File Locked or Read-only */
#define SI_INVFILHDL       92 + FIOLOCBAS   /* Invalid File Handle      */
#define SI_INSDSKSPC       93 + FIOLOCBAS   /* Insufficient Disk Space  */
#define SI_USRCANREQ       94 + FIOLOCBAS   /* User cancelled request   */
#define SI_FIOLIBNAM    "File Input/Output" /* Library name             */

/************************************************************************/
/************************************************************************/
#define DI_REQSTR       0x10            /* Msg Ask request ID           */
#define DI_RSPSTR       0x12            /* Msg Ask response ID          */

#define DI_IDXINS       0x10
#define DI_IDXHDR       0x12
#define DI_IDXLST       0x14
#define DI_IDXNUM       0x16
#define DI_IDXOIN       0x18
#define DI_IDXOFF       0x1a
#define DI_IDXTIN       0x1c
#define DI_IDXTXT       0x1e

