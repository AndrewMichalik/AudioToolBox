/************************************************************************/
/* Generic PCM Conversion Utility: GenCvt.c             V2.00  03/01/93 */
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
#include "cvtlib\gencvt.h"              /* PCM conversion defs          */
#include "cvtsup.h"                     /* PCM conversion supp          */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */
#include <stdio.h>                      /* Standard I/O                 */
#include <io.h>                         /* Low level file I/O           */
  
/************************************************************************/
/*                  Global & External References                        */
/************************************************************************/
extern  char  * MsgUseIni;
extern  char  * MsgDspUse;
extern  char  * MsgHlp001;
extern  char  * MsgHlp002;
extern  CFGBLK  cbCfgBlk;               /* Configuration file block     */
extern  CVTBLK  cbCvtBlk;               /* Function communication block */

/************************************************************************/
/*                      Forward References                              */
/************************************************************************/
BOOL    FAR PASCAL  _loadds CvtChkCBk (LPCSTR, LPVOID); 
DWORD   FAR PASCAL  _loadds CvtPCMCBk (short, short, LPCSTR, LPCSTR, LPSTR, WORD, LPVOID); 
BOOL    FAR PASCAL  _loadds CvtEndCBk (DWORD, LPVOID); 
WORD    FAR PASCAL  _loadds CvtPolPrc (LPVOID, DWORD, DWORD);
void    UseErrEnd   (EXIMOD, LPSTR);
void    DspHlpEnd   (void);

/************************************************************************/
/*                  Progress and Statistics Globals                     */
/************************************************************************/
DWORD   ulFilCnt = 0L;                  /* Total file count             */
DWORD   ulDotCnt = 0L;                  /* QuickWin progress dot count  */

/************************************************************************/
/************************************************************************/
void main (int argc, char **argv, char **envp)
{
    char    szSrcSpc[FIOMAXNAM] = {'\0'};
    char    szDstSpc[FIOMAXNAM] = {'\0'};
    EXIMOD  emExiMod = EXIALLEXI;
    LPSTR   lpWrkPtr;      

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
    /* Get source file specification                                    */
    /********************************************************************/
    _fstrncpy (szSrcSpc, *argv++, FIOMAXNAM);
    argc--;                                     /* Source file arg used */

    /********************************************************************/
    /* Get name of new file name (if specified)                         */
    /********************************************************************/
    if ((0 != argc) && (NULL == _fstrchr (*argv, '-'))) {
        _fstrncpy (szDstSpc, *argv++, FIOMAXNAM);
        argc--;
    }

    /********************************************************************/
    /* Get dash parameters (uses support library query functions)       */
    /********************************************************************/
    if (lpWrkPtr = DshPrmLod (argc, argv, &cbCfgBlk, &cbCvtBlk)) 
        UseErrEnd (emExiMod, lpWrkPtr);

    /********************************************************************/
    /* Override default poll display routine                            */
    /********************************************************************/
#if (defined (_QWINVER)) /***********************************************/
    cbCvtBlk.lpPolDsp = CvtPolPrc;
#endif /*****************************************************************/

    /********************************************************************/
    /********************************************************************/
    BinFilRep (szSrcSpc, szDstSpc, cbCfgBlk.usRepFlg, CvtChkCBk, CvtPCMCBk, 
        CvtEndCBk, NULL, &cbCvtBlk);
    printf ("%ld file(s) converted\n", ulFilCnt);

    /********************************************************************/
    /* Release dash parameters                                          */
    /********************************************************************/
    DshPrmRel (&cbCvtBlk);

    /********************************************************************/
    /* Free conversion resources                                        */
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
    /********************************************************************/
    /********************************************************************/
    printf ("\n");
    printf (MsgDspUse);
    printf (MsgHlp001);
    if ('n' == MsgAskUsr ("\nPress <No> to exit, <Enter> to continue... ")) exit (0);
    printf ("\n\n");
    printf (MsgHlp002);

#if (defined (_QWINVER)) /***********************************************/
_wsetexit (_WINEXITPERSIST);
#endif /*****************************************************************/
    exit (0);
}

/************************************************************************/
/*                      File I/O Callback Procedures                    */
/************************************************************************/
BOOL  FAR PASCAL _loadds CvtChkCBk (LPCSTR szFulNam, CVTBLK FAR *lpCvtBlk)
{
    printf ("%Fs  ", (LPSTR) szFulNam);
    ulDotCnt = strlen (szFulNam) + 2;
    return (TRUE);
}

DWORD FAR PASCAL _loadds CvtPCMCBk (short sSrcHdl, short sDstHdl, LPCSTR szSrcFil, 
                 LPCSTR szDstFil, LPSTR lpWrkBuf, WORD usBufSiz, CVTBLK FAR *lpCvtBlk)
{
    return (CvtPCMFil (sSrcHdl, sDstHdl, szSrcFil, szDstFil, lpWrkBuf, usBufSiz, lpCvtBlk));
}

BOOL  FAR PASCAL _loadds CvtEndCBk (DWORD ulRepCnt, CVTBLK FAR *lpCvtBlk)
{
    if (ulRepCnt) ulFilCnt++;
    printf ("\n");
    return (TRUE);
}

/************************************************************************/
/*              Cooperative yield to other processes                    */
/************************************************************************/
#if (defined (_QWINVER)) /***********************************************/
WORD FAR PASCAL _loadds CvtPolPrc (LPVOID lpRsv001, DWORD ulRsv002, DWORD ulRsv003)
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