/************************************************************************/
/* Summa Four Packing Utility Defs: SU4Pak.h            V1.00  09/10/92 */
/* Copyright (c) 1987-1993 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define TBXMAXSTR     40                /* Gen purpose string length    */
#define REGKEYLEN      8                /* Registration key length      */
#define SERSEGLEN      4                /* Serial number segment length */

#define USEERREND      1                /* User termination exit code   */

/************************************************************************/
/************************************************************************/
typedef struct _TBXGLO {
    char    szAppNam  [TBXMAXSTR];      /* Application name             */
    char    RelKeyArr [REGKEYLEN];      /* Operating/Demo Only Key      */
    char    SeqNumArr [SERSEGLEN];      /* Op/Demo sequence number      */
} TBXGLO;

typedef struct _REPBLK {
    DWORD   ulFilCnt;
    SU4HDR  shSu4Hdr;
} REPBLK;

/************************************************************************/
/************************************************************************/
#define W__SWP(w)   ((((WORD)((w)&0xffff)) >> 8) | (((WORD)((w)&0xffff)) << 8))
#define DW_SWP(dw)  (((DWORD)W__SWP(((DWORD)(dw)) >> 16)) | (((DWORD)W__SWP((DWORD)(dw))) << 16))

/************************************************************************/
/************************************************************************/
BOOL  FAR PASCAL Su4ChkFil (LPCSTR, LPVOID); 
DWORD FAR PASCAL Su4RepFil (short, short, LPCSTR, LPCSTR, LPSTR, WORD, LPVOID); 
BOOL  FAR PASCAL Su4EndFil (DWORD, LPVOID); 
DWORD FAR        IniSu4Hdr (SU4HDR FAR *);

long             Su4CopCvt (short, short, LPSTR, WORD, WORD, WORD, unsigned int FAR *);

