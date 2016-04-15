/************************************************************************/
/* Binary File Replacement Defs: BinRep.h               V1.30  11/20/91 */
/* Copyright (c) 1987-1991 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define USEERREND      1                /* User termination exit code   */
#define BADOLDSTR      2                /* Invalid old string spec      */
#define BADNEWSTR      3                /* Invalid new string spec      */
#define MAXARRLEN     64                /* Replacement array length     */

/************************************************************************/
/************************************************************************/
typedef struct _REPBLK {
    BYTE *  pbSrcArr;
    WORD    usSrcSiz;
    BYTE *  pbTrgArr;
    WORD    usTrgSiz;
    DWORD   ulTotCnt;
} REPBLK;

/************************************************************************/
/************************************************************************/
BOOL  FAR PASCAL ArrChkFil (LPCSTR, LPVOID);
DWORD FAR PASCAL ArrRepFil (short, short, LPCSTR, LPCSTR, LPSTR, WORD, LPVOID); 
BOOL  FAR PASCAL ArrEndFil (DWORD, LPVOID);












