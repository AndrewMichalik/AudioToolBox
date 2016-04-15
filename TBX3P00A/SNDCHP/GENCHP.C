/************************************************************************/
/* Sound Chopping Utility: SndChp.c                     V2.00  08/15/95 */
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
#include "chplib\genchp.h"              /* Sound Chop defs              */
#include "chpsup.h"                     /* Sound Chop supp              */

#include <string.h>                     /* String manipulation funcs    */
#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <stdio.h>                      /* Standard I/O                 */
#include <sys\stat.h>                   /* File status types            */
#include <fcntl.h>                      /* Flags used in open/ sopen    */
#include <io.h>                         /* Low level file I/O           */
  
/************************************************************************/
/*                  Global & External References                        */
/************************************************************************/
extern  char  * MsgUseIni;
extern  char  * MsgDspUse;
extern  char  * MsgDspHlp;
extern  CFGBLK  cbCfgBlk;               /* Configuration file block     */
extern  CHPBLK  cbChpBlk;               /* Function communication block */

/************************************************************************/
/*                          Forward References                          */
/************************************************************************/
BOOL    FAR PASCAL _loadds ChpOpnCBk (short FAR *, DWORD);
BOOL    FAR PASCAL _loadds ChpClsCBk (short, DWORD);
WORD    FAR PASCAL _loadds ChpPolPrc (LPVOID, DWORD, DWORD);
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
    char    szFilDrv[_MAX_DRIVE], szFilDir[_MAX_DIR];
    char    szFilNam[_MAX_FNAME], szFilExt[_MAX_EXT];
    char    szSrcSpc[FIOMAXNAM] = {'\0'};
    char    szDstSpc[FIOMAXNAM] = {'\0'};
    char    szSrcFil[_MAX_PATH];
    EXIMOD  emExiMod = EXIALLEXI;
    LPSTR   lpWrkPtr;      
    LSTSPC  lsLstSpc;
    DWORD   ulOutCnt;
    WORD    usFilCnt;
    short   sSrcHdl;

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
    /* Use source input file extension as default output file extension */
    /********************************************************************/
    _splitpath (szSrcSpc, szFilDrv, szFilDir, szFilNam, szFilExt);
    _fstrncpy (szDstSpc, szFilExt, FIOMAXNAM);

    /********************************************************************/
    /* Get destination extension string (if specified)                  */
    /********************************************************************/
    if ((0 != argc) && (NULL == _fstrchr (*argv, '-'))) {
        _fstrncpy (szDstSpc, *argv++, FIOMAXNAM);
        argc--;
    }

    /********************************************************************/
    /* Get dash parameters                                              */
    /********************************************************************/
    if (lpWrkPtr = DshPrmLod (argc, argv, &cbCfgBlk, &cbChpBlk)) 
        UseErrEnd (emExiMod, lpWrkPtr);

    /********************************************************************/
    /* Override default poll display routine                            */
    /********************************************************************/
#if (defined (_QWINVER)) /***********************************************/
    cbChpBlk.lpPolDsp = ChpPolPrc;
#endif /*****************************************************************/

    /********************************************************************/
    /* Convert file specification, check if any files available         */
    /********************************************************************/
    usFilCnt = GetFstFil (szSrcSpc, 0, szSrcFil, &lsLstSpc, NULL);
    EndFilLst (&lsLstSpc);
    if (!usFilCnt) UseErrEnd (emExiMod, szSrcSpc);

    /********************************************************************/
    /********************************************************************/
    if (-1 == (sSrcHdl = FilHdlOpn (szSrcFil, O_BINARY | O_RDONLY))) {
        printf ("Cannot Open Source File: %s\n", szSrcFil);
        exit (USEERREND);
    }

    /********************************************************************/
    /********************************************************************/
    printf ("%s:\n", szSrcFil);
    ulOutCnt = ChpSndFil (sSrcHdl, NULL, ChpOpnCBk, NULL, ChpClsCBk, 
        (DWORD) szDstSpc, &cbChpBlk);
    printf ("%ld file(s), %ld bytes extracted\n", ulFilCnt, ulOutCnt);

    /********************************************************************/
    /* Release dash parameters                                          */
    /********************************************************************/
    DshPrmRel (&cbChpBlk);

    /********************************************************************/
    /* Free chop resources                                              */
    /********************************************************************/
    FilHdlCls (sSrcHdl);
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
    printf (MsgDspHlp);

#if (defined (_QWINVER)) /***********************************************/
_wsetexit (_WINEXITPERSIST);
#endif /*****************************************************************/
    exit (0);
}

/************************************************************************/
/*                      File I/O Callback Procedures                    */
/************************************************************************/
BOOL FAR PASCAL _loadds ChpOpnCBk (short FAR *lpsDstHdl, DWORD ulCBkDat)
{
    LPSTR   lpDstSpc = (LPSTR) ulCBkDat;
    char    szDstPth[_MAX_DRIVE + _MAX_DIR]; 
    char    szDstNam[_MAX_FNAME + _MAX_EXT];
    char    szDstFil[_MAX_PATH];

    /********************************************************************/
    /* Use natural numbers for output file name count                   */
    /********************************************************************/
    cbCfgBlk.lNamOff++;

    /********************************************************************/
    /* Parse destination output file name                               */
    /********************************************************************/
    GetFulPth (lpDstSpc, szDstPth, szDstNam);
    sprintf (szDstFil, "%s%04ld%s", szDstPth, cbCfgBlk.lNamOff, szDstNam);
    printf ("%04ld%Fs\n", cbCfgBlk.lNamOff, (LPSTR) szDstNam);

    /********************************************************************/
    /* Initialize QuickWin display wrap-count                           */
    /********************************************************************/
    ulDotCnt = 4L + strlen (szDstNam);

    /********************************************************************/
    /* Open and truncate file to zero length                            */
    /********************************************************************/
    if (-1 == (*lpsDstHdl = FilHdlCre (szDstFil,
      O_BINARY | O_CREAT | O_TRUNC | O_WRONLY, S_IREAD | S_IWRITE))) {
        printf ("Cannot Open Target File: %Fs, skipping...\n", (LPSTR) szDstFil);
        return (TRUE);
    }

    /********************************************************************/
    /* Increment file count                                             */
    /********************************************************************/
    ulFilCnt++;

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);
}

//DWORD FAR PASCAL _loadds ChpCopCBk (short sSrcHdl, DWORD ulBytCnt, short sDstHdl, 
//        LPSTR lpWrkBuf, WORD usBufSiz, DWORD ulCBkDat)
//{
//    return (FilCop (sSrcHdl, sDstHdl, lpWrkBuf, usBufSiz, ulBytCnt, usEncFmt,
//        unsigned int far *lpErrCod, fpPolPrc, ulPolDat));
//}

BOOL FAR PASCAL _loadds ChpClsCBk (short sDstHdl, DWORD ulCBkDat)
{
    /********************************************************************/
    /********************************************************************/
    FilHdlCls (sDstHdl);

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);
}

/************************************************************************/
/*              Cooperative yield to other processes                    */
/************************************************************************/
#if (defined (_QWINVER)) /***********************************************/
WORD FAR PASCAL _loadds ChpPolPrc (LPVOID lpRsv001, DWORD ulRsv002, DWORD ulRsv003)
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