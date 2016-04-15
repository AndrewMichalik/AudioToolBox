/************************************************************************/
/* Index Rebuild Utility: IdxReb.c                      V2.10  10/15/95 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#if (defined (W31)) /****************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#include "..\os_dev\winmem.h"           /* Generic memory supp defs     */
#include "..\os_dev\winmsg.h"           /* User message support defs    */
#endif /*****************************************************************/
#if (defined (DOS)) /****************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "..\os_dev\dosmem.h"           /* Generic memory support defs  */
#include "..\os_dev\dosmsg.h"           /* User message support defs    */
#endif /*****************************************************************/
#include "..\fiodev\filutl.h"           /* File utilities definitions   */
#include "idxlib\genidx.h"              /* Indexed file defs            */
#include "idxsup.h"                     /* Indexed File supp            */

#include <string.h>                     /* String manipulation funcs    */
#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <stdio.h>                      /* Standard I/O                 */
#include <io.h>                         /* Low level file I/O           */
  
/************************************************************************/
/*                          External References                         */
/************************************************************************/
extern  char  * MsgUseIni;
extern  char  * MsgDspUse;
extern  char  * MsgDspHlp;

/************************************************************************/
/*                          Forward References                          */
/************************************************************************/
BOOL    FAR PASCAL  _loadds RebChkCBk (LPCSTR, LPVOID); 
DWORD   FAR PASCAL  _loadds RebIdxCBk (short, short, LPCSTR, LPCSTR, LPSTR, WORD, LPVOID); 
BOOL    FAR PASCAL  _loadds RebEndCBk (DWORD, LPVOID); 
WORD    FAR PASCAL  _loadds RebPolPrc (LPVOID, DWORD, DWORD);
void    UseErrEnd   (EXIMOD, LPSTR);
void    DspHlpEnd   (void);

/************************************************************************/
/*                          Help Messages                               */
/************************************************************************/
char  * MsgUseIni =  
"\n\
        Voice Information Systems (1-800-234-VISI) Index Rebuild Utility\n\
              v3.00a.1 (C) Copyright Andrew J. Michalik 1987-1996\n\n";

char  * MsgDspUse =  
"\
Usage: IdxReb [-help] OldFile [NewFile] [-b -d -f -i -rt -rv -@]\n";

char  * MsgDspHlp = "\
       OldFile: Source filename.\n\
       NewFile: Destination filename, if different from OldFile.\n\
       -b     : Backup file creation inhibited.\n\
       -dNNNN : Delete NNNN indexes (including @ position), default 0.\n\
       -fNNNN : Set sample frequency to NNNN.\n\
       -iNNNN : Insert NNNN indexes (before @ position), default 0.\n\
       -rtFILE: Replace Txt (@ position), default none.\n\
       -rvFILE: Replace Vox (@ position), default none.\n\
       -@NNNN : @ index position; default = 1, end = 9999.\n\
Notes: Environment TMP= specifies temp work files directory.\n";

/************************************************************************/
/************************************************************************/
RBIBLK  rbRebBlk;                       /* Rebuild Index function block */

/************************************************************************/
/*                  Progress and Statistics Globals                     */
/************************************************************************/
DWORD   ulFilCnt = 0L;                  /* Total file count             */
DWORD   ulTotCnt = 0L;                  /* Total byte transfer count    */
DWORD   ulDotCnt = 0L;                  /* QuickWin progress dot count  */

/************************************************************************/
/************************************************************************/
void main (int argc, char **argv, char **envp)
{
    char    szSrcFil[FIOMAXNAM] = {'\0'};
    char    szTrgSpc[FIOMAXNAM] = {'\0'};
    EXIMOD  emExiMod = EXIALLEXI;
    WORD    usRepFlg = 0;

    /********************************************************************/
    /********************************************************************/
    printf (MsgUseIni);

    /********************************************************************/
    /* Initialize support library and get system dash parameters.       */
    /* Note: Retrieves *argv working directory and system debug         */
    /*       parameters prior to regular argument parsing.              */
    /********************************************************************/
    if (SysPrmIni (argc, argv, &emExiMod)) UseErrEnd (emExiMod, NULL);

    /********************************************************************/
    /* Check argument count. Skip program name argument                 */
    /********************************************************************/
    if (--argc && (0 == _fstrncmp (*++argv, "-h", 2))) DspHlpEnd ();
    if (argc < 1) UseErrEnd (emExiMod, NULL);

    /********************************************************************/
    /* Get name of source file                                          */
    /********************************************************************/
    _fstrncpy (szSrcFil, *argv++, FIOMAXNAM);
    argc--;                                     /* Source file arg used */

    /********************************************************************/
    /* Get name of new file (if specified)                              */
    /********************************************************************/
    if ((0 != argc) && (NULL == _fstrchr (*argv, '-'))) {
        _fstrncpy (szTrgSpc, *argv++, FIOMAXNAM);
        argc--;
    }
    else *szTrgSpc = '\0';

    /********************************************************************/
    /* Initialize function communication block                          */
    /********************************************************************/
    RebIdxIni (&rbRebBlk);

    /********************************************************************/
    /* Get dash parameters (uses support library query functions)       */
    /********************************************************************/
    while (0 < argc--) {
        if (!strncmp (*argv, "-debug", 6));    /* Handled in Ini   */
        else if (!strncmp (*argv, "-exit", 5));/* Handled in Ini   */
        else if (!strncmp (*argv, "-kf", 3));  /* Handled in Ini   */
        else if (!strncmp (*argv, "-ks", 3));  /* Handled in Ini   */
        else if (!_fstrncmp (*argv, "-b", 2)) usRepFlg |= BFRBKIFLG; 
        else if (!_fstrncmp (*argv, "-d", 2)) rbRebBlk.sShfCnt = - abs (atoi (&(*argv)[2])); 
        else if (!_fstrncmp (*argv, "-f", 2)) rbRebBlk.ulSmpFrq = atol (&(*argv)[2]); 
        else if (!_fstrncmp (*argv, "-i", 2)) rbRebBlk.sShfCnt = abs (atoi (&(*argv)[2])); 
        else if (!_fstrncmp (*argv, "-@", 2)) rbRebBlk.usAt_Pos = atoi (&(*argv)[2]); 
        else if (!_fstrncmp (*argv, "-rv", 3)) _fstrncpy (rbRebBlk.ucRepVox, &(*argv)[3], FIOMAXNAM);
        else if (!_fstrncmp (*argv, "-rt", 3)) _fstrncpy (rbRebBlk.ucRepTxt, &(*argv)[3], FIOMAXNAM);
        else UseErrEnd (emExiMod, *argv);
        *argv++;
    }

    /********************************************************************/
    /* Override default poll display routine                            */
    /********************************************************************/
#if (defined (_QWINVER)) /***********************************************/
    rbRebBlk.lpPolDsp = RebPolPrc;
#endif /*****************************************************************/

    /********************************************************************/
    /********************************************************************/
    BinFilRep (szSrcFil, szTrgSpc, usRepFlg, RebChkCBk, RebIdxCBk, RebEndCBk,
        NULL, &rbRebBlk);
    printf ("%ld bytes in %ld file(s)\n", ulTotCnt, ulFilCnt);

    /********************************************************************/
    /* Free system resources                                            */
    /********************************************************************/
    SysPrmTrm ();

    /********************************************************************/
    /* Use exit mode setting to determine whether to exit or pause      */
    /********************************************************************/
#if (defined (_QWINVER)) /***********************************************/
    _wsetexit (_WINEXITNOPERSIST);
#endif /*****************************************************************/
    switch (emExiMod) {
        case EXIALLEXI:
        case EXIERRPAU:
            break;
        case EXIWRNPAU:
            if (ulFilCnt) break;
        case EXIALLPAU:
            #if (defined (_QWINVER)) /***********************************/
                _wsetexit (_WINEXITPERSIST);
            #else
                MsgAskUsr ("\nPress <Enter> to continue... ");                
            #endif /*****************************************************/
            break;
    }
    exit (0);

}

/************************************************************************/
/*                      User Error Messages                             */
/************************************************************************/
void    UseErrEnd (EXIMOD emExiMod, LPSTR szErrTxt)
{
    if (NULL != szErrTxt) printf ("Bad Switch: %Fs\n", (LPSTR) szErrTxt);
    printf (MsgDspUse);

    /********************************************************************/
    /* Use exit mode setting to determine whether to exit or pause      */
    /********************************************************************/
#if (defined (_QWINVER)) /***********************************************/
    _wsetexit (_WINEXITNOPERSIST);
#endif /*****************************************************************/
    switch (emExiMod) {
        case EXIALLEXI:
            break;
        case EXIERRPAU:
        case EXIWRNPAU:
        case EXIALLPAU:
            #if (defined (_QWINVER)) /***********************************/
                _wsetexit (_WINEXITPERSIST);
            #else
                MsgAskUsr ("\nPress <Enter> to continue... ");                
            #endif /*****************************************************/
            break;
    }
    exit (USEERREND);
}

void    DspHlpEnd (void)
{
    printf (MsgDspUse);
    printf (MsgDspHlp);

#if (defined (_QWINVER)) /***********************************************/
_wsetexit (_WINEXITPERSIST);
#endif /*****************************************************************/
    exit (0);

}

/************************************************************************/
/*                      File I/O Callback Procedures                    */
/************************************************************************/
BOOL  FAR PASCAL _loadds RebChkCBk (LPCSTR szFulNam, RBIBLK FAR *lpRebBlk)
{
    printf ("%Fs, ", (LPSTR) szFulNam);
    ulDotCnt = strlen (szFulNam) + 2;
    ulFilCnt++;

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);
}

DWORD FAR PASCAL _loadds RebIdxCBk (short sSrcHdl, short sDstHdl, LPCSTR szSrcFil, 
                 LPCSTR szDstFil, LPSTR lpWrkBuf, WORD usBufSiz, RBIBLK FAR *lpRebBlk)
{
    return (RebIdxFil (sSrcHdl, sDstHdl, NULL, NULL, lpWrkBuf, usBufSiz, lpRebBlk));
}

BOOL  FAR PASCAL _loadds RebEndCBk (DWORD ulRepCnt, RBIBLK FAR *lpRebBlk)
{
    ulTotCnt += ulRepCnt;
    if (ulRepCnt) printf ("%ld bytes transferred\n", ulRepCnt);
    else printf ("\r                                        \r");

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);
}

/************************************************************************/
/*              Cooperative yield to other processes                    */
/************************************************************************/
#if (defined (_QWINVER)) /***********************************************/
WORD FAR PASCAL _loadds RebPolPrc (LPVOID lpRsv001, DWORD ulRsv002, DWORD ulRsv003)
{
    MSG msg;

    /********************************************************************/
    /********************************************************************/
    if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage (&msg);
    }

    /********************************************************************/
    /* Print "." to indicate continued processing                       */
    /* Note: Must use _loadds for printf from a QuickWin DLL callback   */
    /********************************************************************/
    printf (".");
    if (!(++ulDotCnt % 80)) printf ("\n");

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);

}
#endif /*****************************************************************/

