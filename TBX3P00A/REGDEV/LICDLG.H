/************************************************************************/
/* License Dialog Defs: LicDlg.h                        V2.00  04/15/96 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define         LICSTRLEN   80          /* Maximum string length        */
typedef struct _LICDLGBLK {
    char        szTtlTxt[LICSTRLEN+1];  /* Title                        */
    char        szLicTxt[LICSTRLEN+1];  /* License text                 */
    char        szPrdNam[LICSTRLEN+1];  /* Product name                 */
    char        szCopRgt[LICSTRLEN+1];  /* Copyright                    */
    char        szConAdr[LICSTRLEN+1];  /* Contact address              */
} LICDLGBLK;

/************************************************************************/
/************************************************************************/
int LicDlgDsp (LPSTR, LPSTR, LPSTR, LICDLGBLK FAR *);
