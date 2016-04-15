/************************************************************************/
/* Index Packing Utility: IdxPak.c                      V2.20  10/15/95 */
/* Copyright (c) 1987-1991 Andrew J. Michalik                           */
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
BOOL    FAR PASCAL  _loadds PakChkCBk (LPCSTR, DWORD);
BOOL    FAR PASCAL  _loadds PakEndCBk (DWORD, DWORD);
WORD    FAR PASCAL  _loadds PakPolPrc (LPVOID, DWORD, DWORD);
void    UseErrEnd   (EXIMOD, LPSTR);
void    DspHlpEnd   (void);

/************************************************************************/
/*                          Help Messages                               */
/************************************************************************/
char  * MsgUseIni =  
"\n\
        Voice Information Systems (1-800-234-VISI) Index Packing Utility\n\
               v3.00a.1 (C) Copyright Andrew J. Michalik 1987-1996\n\n";

char  * MsgDspUse =  
"\
Usage: IdxPak [-help] VoxFile IdxFile [-f -s -t]\n";

char  * MsgDspHlp = "\
       VoxFile: Source filename.\n\
       IdxFile: Destination filename.\n\
       -fNNNN : Set sample frequency to NNNN, default 6000.\n\
       -sX    : Sort files. + up, - down, u unsorted. Default is +.\n\
       -tSPEC : Text file path specification with * wildcard.\n\
Notes: Environment TMP= specifies temp work files directory.\n";

/************************************************************************/
/************************************************************************/
PKIBLK  kbPakBlk;                       /* Pack Index function block    */

/************************************************************************/
/*                  Progress and Statistics Globals                     */
/************************************************************************/
DWORD   ulFilCnt = 0L;                  /* Total file count             */
DWORD   ulTotCnt = 0L;                  /* Total byte transfer count    */
DWORD   ulDotCnt = 0L;                  /* Progress poll dot count      */

/************************************************************************/
/************************************************************************/
void main (int argc, char **argv, char **envp)
{
    char    szSrcFil[FIOMAXNAM] = {'\0'};
    char    szTrgSpc[FIOMAXNAM] = {'\0'};
    char    szTxtSpc[FIOMAXNAM] = {'\0'};
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
    if (argc < 2) UseErrEnd (emExiMod, NULL);

    /********************************************************************/
    /* Get name of source and target files                              */
    /********************************************************************/
    _fstrncpy (szSrcFil, *argv++, FIOMAXNAM);
    _fstrncpy (szTrgSpc, *argv++, FIOMAXNAM);
    argc = argc - 2;                            /* Indicate 2 arg used  */

    /********************************************************************/
    /* Initialize function communication block                          */
    /* Initialize preferred settings                                    */
    /********************************************************************/
    PakIdxIni (&kbPakBlk);
    kbPakBlk.sSrtTyp  =    +1;          /* Sort ascending               */
    kbPakBlk.ulSmpFrq = 6000L;          /* Header sample frequency      */

    /********************************************************************/
    /* Get dash parameters (uses support library query functions)       */
    /********************************************************************/
    while (0 < argc--) {
        if (!strncmp (*argv, "-debug", 6));    /* Handled in Ini   */
        else if (!strncmp (*argv, "-exit", 5));/* Handled in Ini   */
        else if (!strncmp (*argv, "-kf", 3));  /* Handled in Ini   */
        else if (!strncmp (*argv, "-ks", 3));  /* Handled in Ini   */
        else if (!_fstrncmp (*argv, "-f", 2)) kbPakBlk.ulSmpFrq = atol (&(*argv)[2]);
        else if (!_fstrncmp (*argv, "-t", 2)) _fstrncpy (szTxtSpc, &(*argv)[2], FIOMAXNAM);
        else if (!_fstrncmp (*argv, "-s", 2)) 
            kbPakBlk.sSrtTyp = ('-' == (*argv)[2]) ? -1 : (('u' == (*argv)[2]) ? 0 : +1);
        else UseErrEnd (emExiMod, *argv);
        *argv++;
    }

    /********************************************************************/
    /* Override default poll display routine                            */
    /********************************************************************/
    kbPakBlk.lpPolDsp = PakPolPrc;

    /********************************************************************/
    /********************************************************************/
    PakIdxFil (szSrcFil, szTxtSpc, szTrgSpc, PakChkCBk, NULL, PakEndCBk, 0L, 
        &kbPakBlk);
    printf ("%ld file(s), %ld bytes packed\n", ulFilCnt, ulTotCnt);

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
BOOL  FAR PASCAL _loadds PakChkCBk (LPCSTR szFulNam, DWORD ulCBkDat)
{
    static char szFilDrv[_MAX_DRIVE], szFilDir[_MAX_DIR];   
    static char szFilNam[_MAX_FNAME], szFilExt[_MAX_EXT];   

    /********************************************************************/
    /* Separate file name from full path & print                        */
    /********************************************************************/
    _splitpath (szFulNam, szFilDrv, szFilDir, szFilNam, szFilExt);
    strcat (szFilNam, szFilExt);
    printf ("%Fs  ", (LPSTR) szFilNam);
    ulDotCnt = strlen (szFilNam) + 2;
    ulFilCnt++;

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);
}

BOOL  FAR PASCAL _loadds PakEndCBk (DWORD ulRepCnt, DWORD ulCBkDat)
{
    ulTotCnt += ulRepCnt;
    printf ("\n");

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);
}

/************************************************************************/
/*              Cooperative yield to other processes                    */
/************************************************************************/
WORD FAR PASCAL _loadds PakPolPrc (LPVOID lpRsv001, DWORD ulRsv002, DWORD ulRsv003)
{
#if (defined (_QWINVER)) /***********************************************/
    MSG msg;
    /********************************************************************/
    /********************************************************************/
    if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage (&msg);
    }
#endif /*****************************************************************/

    /********************************************************************/
    /* Print "." to indicate continued processing                       */
    /********************************************************************/
    printf (".");
#if (defined (_QWINVER)) /***********************************************/
    if (!(++ulDotCnt % 80)) printf ("\n");
#endif /*****************************************************************/

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);

}

