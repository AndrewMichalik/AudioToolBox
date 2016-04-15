/************************************************************************/
/* Index Chopping Utility: IdxChp.c                     V2.10  10/15/95 */
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
#include <fcntl.h>                      /* Flags used in open/ sopen    */
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
BOOL    FAR PASCAL  _loadds ChpChkCBk (LPCSTR, DWORD);
BOOL    FAR PASCAL  _loadds ChpEndCBk (DWORD, DWORD);
WORD    FAR PASCAL  _loadds ChpPolPrc (LPVOID, DWORD, DWORD);
void    UseErrEnd   (EXIMOD, LPSTR);
void    DspHlpEnd   (void);

/************************************************************************/
/*                          Help Messages                               */
/************************************************************************/
char  * MsgUseIni =  
"\n\
        Voice Information Systems (1-800-234-VISI) Index Chopping Utility\n\
               v3.00a.1 (C) Copyright Andrew J. Michalik 1987-1996\n\n";

char  * MsgDspUse =  
"\
Usage: IdxChp [-help] IdxFile [VoxExt TxtExt -co -n -@]\n";

char  * MsgDspHlp = "\
       IdxFile: Source filename.\n\
       VoxExt:  Destination audio file extension (Default is .vox).\n\
       TxtExt:  Destination text  file extension (Default is .txt).\n\
       -coXXX:  Destination file name counter offset, default is 0.\n\
       -nXXXX:  Number of indexes to chop, default is all.\n\
       -@XXXX:  Chop @ index position; default = 1, end = 9999.\n\
Notes: Environment TMP= specifies temp work files directory.\n";

/************************************************************************/
/************************************************************************/
CHIBLK  cbChpBlk;                       /* Chop Index function block    */

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
    char    szSrcSpc[FIOMAXNAM] = {'\0'};
    char    szVoxSpc[FIOMAXNAM] = {".vox"};
    char    szTxtSpc[FIOMAXNAM] = {".txt"};
    char    szSrcFil[_MAX_PATH];
    EXIMOD  emExiMod = EXIALLEXI;
    LSTSPC  lsLstSpc;
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
    /* Get Vox extension string (if specified)                          */
    /********************************************************************/
    if ((0 != argc) && (NULL == _fstrchr (*argv, '-'))) {
        _fstrncpy (szVoxSpc, *argv++, FIOMAXNAM);
        argc--;
    }

    /********************************************************************/
    /* Get Txt file string (if specified)                               */
    /********************************************************************/
    if ((0 != argc) && (NULL == _fstrchr (*argv, '-'))) {
        _fstrncpy (szTxtSpc, *argv++, FIOMAXNAM);
        argc--;
    }

    /********************************************************************/
    /* Initialize function communication block                          */
    /********************************************************************/
    ChpIdxIni (&cbChpBlk);

    /********************************************************************/
    /* Get dash parameters (uses support library query functions)       */
    /********************************************************************/
//How do I specify new text extension without specifying vox extension?
    while (0 < argc--) {
        if (!strncmp (*argv, "-debug", 6));    /* Handled in Ini   */
        else if (!strncmp (*argv, "-exit", 5));/* Handled in Ini   */
        else if (!strncmp (*argv, "-kf", 3));  /* Handled in Ini   */
        else if (!strncmp (*argv, "-ks", 3));  /* Handled in Ini   */
        else if (!_fstrncmp (*argv, "-co", 3)) cbChpBlk.lNamOff = atol (&(*argv)[3]); 
        else if (!_fstrncmp (*argv, "-@", 2)) cbChpBlk.ulIdxPos = labs (atol (&(*argv)[2]));
        else if (!_fstrncmp (*argv, "-n", 2)) cbChpBlk.ulIdxCnt = labs (atol (&(*argv)[2])); 
        else UseErrEnd (emExiMod, *argv);
        *argv++;
    }

    /********************************************************************/
    /* Override default poll display routine                            */
    /********************************************************************/
    cbChpBlk.lpPolDsp = ChpPolPrc;

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
    ChpIdxFil (sSrcHdl, szVoxSpc, szTxtSpc, ChpChkCBk, NULL, ChpEndCBk, 0L, &cbChpBlk);
    printf ("%ld files, %ld bytes extracted\n", ulFilCnt, ulTotCnt);

    /********************************************************************/
    /********************************************************************/
    FilHdlCls (sSrcHdl);

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
BOOL  FAR PASCAL _loadds ChpChkCBk (LPCSTR szFulNam, DWORD ulCBkDat)
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

BOOL  FAR PASCAL _loadds ChpEndCBk (DWORD ulRepCnt, DWORD ulCBkDat)
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
WORD FAR PASCAL _loadds ChpPolPrc (LPVOID lpRsv001, DWORD ulRsv002, DWORD ulRsv003)
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