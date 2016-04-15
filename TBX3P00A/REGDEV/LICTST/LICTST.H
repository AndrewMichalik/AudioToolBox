/************************************************************************/
/************************************************************************/
#define APIENTRY FAR PASCAL
LRESULT APIENTRY MainWndProc(HWND, UINT, UINT, LONG);
#define IDM_DIALOG1     101

/************************************************************************/
/************************************************************************/
#define REGKEYLEN          8            /* Registration key length      */
#define SERSEGLEN          4            /* Serial number segment length */
typedef struct _TBXGLO {
    char *  szAppNam;                   /* Application name             */
    char *  szLibVer;                   /* Library version              */
    char    RelKeyArr [REGKEYLEN];      /* Operating/Demo Only Key      */
    char    SeqNumArr [SERSEGLEN];      /* Op/Demo sequence number      */
    char    ChkSumArr [SERSEGLEN];      /* Security check sum           */
    WORD    usRegKey;                   /* usRegKey                     */
    WORD    usDebFlg;                   /* Debug on/off flags           */
    HANDLE  hLibIns;                    /* Global instance handle       */
} TBXGLO;

/************************************************************************/
/************************************************************************/
#define ERR___DEB       0x0002          /* Debug error messages flag    */
#define KEY___DEB       0x0008          /* Debug key flag               */
#define MEM___DEB       0x0020          /* Debug memory messages flag   */
#define CFG___DEB       0x0080          /* Debug config settings flag   */
#define INI___DEB       0x0200          /* Debug init   settings flag   */

#define REGDEMKEY   0                   /* Demo version key detected    */
#define REGEVLKEY   1                   /* Eval version key detected    */
#define REGFULKEY   2                   /* Full version key detected    */
#define REGMASKEY   3                   /* Mstr version key detected    */
