/************************************************************************/
/* Index List Utility: IdxLst.c                         V2.10  10/15/95 */
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
BOOL    FAR PASCAL  _loadds LstChkCBk (LPCSTR, LPVOID); 
DWORD   FAR PASCAL  _loadds LstIdxCBk (short, short, LPCSTR, LPCSTR, LPSTR, WORD, LPVOID); 
BOOL    FAR PASCAL  _loadds LstEndCBk (DWORD, LPVOID); 
WORD    far cdecl   _loadds MsgDspCBk (const char *pFmtLst, ...);
void    UseErrEnd   (EXIMOD, LPSTR);
void    DspHlpEnd   (void);

/************************************************************************/
/*                          Help Messages                               */
/************************************************************************/
char  * MsgUseIni =  
"\n\
          Voice Information Systems (1-800-234-VISI) Index List Utility\n\
               v3.00a.1 (C) Copyright Andrew J. Michalik 1987-1996\n\n";

char  * MsgDspUse =  
"\
Usage: IdxLst [-help] IdxFile [-g -t -v]\n";

char  * MsgDspHlp = "\
       IdxFile: Source Index filename.\n\
       -g     : General information list inhibited.\n\
       -t     : Txt information list inhibited.\n\
       -v     : Vox information list inhibited.\n\
Notes: Environment TMP= specifies temp work files directory.\n";

/************************************************************************/
/************************************************************************/
LSIBLK  lbLstBlk;                       /* List Index function block    */

/************************************************************************/
/*                  Progress and Statistics Globals                     */
/************************************************************************/
DWORD   ulFilCnt = 0L;                  /* Total file count             */

/************************************************************************/
/************************************************************************/
void main (int argc, char **argv, char **envp)
{
    char    szSrcFil[FIOMAXNAM] = {'\0'};
    EXIMOD  emExiMod = EXIALLEXI;

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
    /* Initialize function communication block                          */
    /********************************************************************/
    LstIdxIni (&lbLstBlk);

    /********************************************************************/
    /* Get dash parameters (uses support library query functions)       */
    /********************************************************************/
    while (0 < argc--) {
        if (!strncmp (*argv, "-debug", 6));    /* Handled in Ini   */
        else if (!strncmp (*argv, "-exit", 5));/* Handled in Ini   */
        else if (!strncmp (*argv, "-kf", 3));  /* Handled in Ini   */
        else if (!strncmp (*argv, "-ks", 3));  /* Handled in Ini   */
        else if (!strncmp (*argv, "-g", 2)) lbLstBlk.usLstFlg |= GENINHFLG; 
        else if (!strncmp (*argv, "-t", 2)) lbLstBlk.usLstFlg |= TXTINHFLG;
        else if (!strncmp (*argv, "-v", 2)) lbLstBlk.usLstFlg |= VOXINHFLG;
        else UseErrEnd (emExiMod, *argv);
        *argv++;
    }

    /********************************************************************/
    /* Override default message display routine                         */
    /********************************************************************/
    lbLstBlk.lpMsgDsp = MsgDspCBk;

    /********************************************************************/
    /********************************************************************/
    BinFilRep (szSrcFil, "", 0, LstChkCBk, LstIdxCBk, LstEndCBk, NULL, 
        &lbLstBlk);
    printf ("%ld files listed\n", ulFilCnt);

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
BOOL FAR PASCAL _loadds LstChkCBk (LPCSTR szFulNam, LSIBLK FAR *lpLstBlk)
{
    printf ("%Fs\n", (LPSTR) szFulNam);
    ulFilCnt++;

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);
}

DWORD FAR PASCAL _loadds LstIdxCBk (short sSrcHdl, short sDstRsv, LPCSTR szSrcFil, 
                 LPCSTR szDstRsv, LPSTR lpWrkBuf, WORD usBufSiz, LSIBLK FAR *lpLstBlk)
{
    return (LstIdxFil (sSrcHdl, NULL, lpWrkBuf, usBufSiz, lpLstBlk));
}

BOOL FAR PASCAL _loadds LstEndCBk (DWORD ulRepCnt, LSIBLK FAR *lpLstBlk)
{
    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);
}

/************************************************************************/
/************************************************************************/
#include <stdarg.h>                     /* ANSI Std var length args     */
WORD far cdecl  _loadds MsgDspCBk (const char *pFmtLst, ...)
{
    /********************************************************************/
    /* Note: printf() requires _loadds for a QuickWin DLL callback.     */
    /********************************************************************/

    /********************************************************************/
    /********************************************************************/
    va_list vaArgMrk;
    va_start (vaArgMrk, pFmtLst);
  
    /********************************************************************/
    /********************************************************************/
    vprintf (pFmtLst, vaArgMrk);

    /********************************************************************/
    /********************************************************************/
    va_end (vaArgMrk);                  /* Reset variable arguments     */

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);

}
