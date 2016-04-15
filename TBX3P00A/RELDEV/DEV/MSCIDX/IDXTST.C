/************************************************************************/
/* Indexed File Test: IdxTst.c                                          */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/* You have a royalty-free right to use, modify, reproduce and          */
/* distribute the Sample Files (and/or any modified version) in any     */
/* way you find useful, provided that you agree that VISI has no        */
/* warranty obligations or liability for any Sample Application Files   */
/* which are modified.                                                  */
/*                                                                      */
/************************************************************************/
#if (defined (W31)) /****************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#endif /*****************************************************************/
#if (defined (DOS)) /****************************************************/
#include "..\idxlib\visdef.h"           /* VIS Standard type defs       */
#endif /*****************************************************************/
#include "..\idxlib\genidx.h"           /* ToolBox Indexed File defs    */

#include <stdlib.h>                     /* Standard library defs        */
#include <string.h>                     /* String manipulation funcs    */
#include <stdio.h>                      /* Standard I/O                 */
#include <io.h>                         /* Low level file I/O           */
#include <fcntl.h>                      /* Flags used in open/ sopen    */
#include <sys\stat.h>                   /* File status types            */

/************************************************************************/
/************************************************************************/
#define MAXFNMLEN       96
#define USEERREND        2

/************************************************************************/
/*                      Forward References                              */
/************************************************************************/
void    UseErrEnd (char *);
void    DspHlpEnd (void);

BOOL    FAR PASCAL  _loadds IdxChkCBk (LPCSTR, DWORD);
BOOL    FAR PASCAL  _loadds IdxEndCBk (DWORD, DWORD);
WORD    FAR PASCAL  _loadds IdxPolPrc (LPVOID, DWORD, DWORD);

/************************************************************************/
/*                          Help Messages                               */
/************************************************************************/
char  * MsgUseIni =  
"\n\
 ToolBox Test [-h for Help] v3.00a.1 Voice Information Systems 1-800-234-VISI\n\n";

char  * MsgDspUse =  
"Usage: IdxTst [-help] IdxFile\n";

char  * MsgDspHlp = "\
       IdxFile:  Source filename (Indexed file format).\n\
";

/************************************************************************/
/*                  Indexed File Communication Blocks                   */
/************************************************************************/
IDXBLK  cbIdxBlk;                       /* Function communication block */
CHIBLK  cbChpBlk;                       /* Chop Index function block    */
LSIBLK  lbLstBlk;                       /* List Index function block    */
PKIBLK  kbPakBlk;                       /* Pack Index function block    */
RBIBLK  rbRebBlk;                       /* Rebuild Index function block */

/************************************************************************/
/************************************************************************/
#define TMPWRKBUF    512
BYTE    ucWrkBuf[TMPWRKBUF];            /* Temporary work buffer        */

/************************************************************************/
/************************************************************************/
void main(int argc, char **argv, char **envp)
{
    static char szSrcFil[MAXFNMLEN] = {'\0'};       /* Static for win   */ 
    static char szDstFil[MAXFNMLEN] = {'\0'};
    short       sSrcHdl;
    short       sDstHdl;
    DWORD       ulOutCnt = 0L;

    /********************************************************************/
    /********************************************************************/
    printf (MsgUseIni);

    /********************************************************************/
    /* Check the version number of support modules (DOS Lib / Win DLLs) */
    /********************************************************************/
    if (IDXVERNUM != IdxFilVer()) UseErrEnd ("Wrong Library Version Number!\n");

    /********************************************************************/
    /* Check argument count. Skip program name argument                 */
    /********************************************************************/
    if (--argc && (0 == strncmp (*++argv, "-h", 2))) DspHlpEnd ();
    if (argc < 1) UseErrEnd (NULL);

    /********************************************************************/
    /* Get name of source file                                          */
    /********************************************************************/
    strncpy (szSrcFil, *argv++, MAXFNMLEN);
    argc--;                                     /* Source file arg used */

    /********************************************************************/
    /* Force destination file name                                      */
    /********************************************************************/
    strncpy (szDstFil, "TstOut.Vap", MAXFNMLEN);    

    /********************************************************************/
    /* Initialize and allocate indexed file resources                   */
    /* Note: Use "_DEBUG_" flag to prevent test for accelerator key     */
    /********************************************************************/
    #if (defined (DEB)) /************************************************/
        #define KEYFLG  (DWORD)(LPSTR)"_DEBUG_"        
    #else
        #define KEYFLG  0L        
    #endif /*************************************************************/
    switch (IdxFilIni (0, 0L, KEYFLG)) {
        case SI_IDXNO_ERR: 
            break;
        case SI_IDXKEYERR:
            printf ("Key/License error, 8 segment limit.\n");
            break;
        case SI_IDXVERERR:
            printf ("Support software module version mismatch, continuing...\n");
            break;
        case SI_IDXKEYERR | SI_IDXVERERR:
            printf ("Key/License error, 8 segment limit.\n");
            printf ("Support software module version mismatch, continuing...\n");
            break;
        default:
            UseErrEnd ("Unknown error, exiting...\n");
            exit (-1);
            break;
    }

    /********************************************************************/
    /* Initialize function communication blocks                         */
    /********************************************************************/
    IdxCfgIni (&cbIdxBlk);
    ChpIdxIni (&cbChpBlk);
    LstIdxIni (&lbLstBlk);
    PakIdxIni (&kbPakBlk);
    RebIdxIni (&rbRebBlk);

    /********************************************************************/
    /* Initialize preferred settings                                    */
    /********************************************************************/
    kbPakBlk.sSrtTyp  =    +1;          /* Sort ascending               */
    kbPakBlk.ulSmpFrq = 6000L;          /* Header sample frequency      */

    /********************************************************************/
    /* Override default display routine                                 */
    /********************************************************************/
    cbChpBlk.lpPolDsp = IdxPolPrc;
    kbPakBlk.lpPolDsp = IdxPolPrc;
    rbRebBlk.lpPolDsp = IdxPolPrc;

    /********************************************************************/
    /* Open Indexed input file                                          */
    /********************************************************************/
    if (-1 == (sSrcHdl = _open (szSrcFil, O_BINARY | O_RDONLY, 0))) {
        printf ("Cannot open source file: %s\n", szSrcFil);
        exit (-1);
    }

    /********************************************************************/
    /* List the contents of the indexed file                            */
    /********************************************************************/
    _lseek (sSrcHdl, 0L, SEEK_SET);
    printf ("\nList the contents of the indexed file:\n");
    LstIdxFil (sSrcHdl, NULL, ucWrkBuf, TMPWRKBUF, &lbLstBlk);

    /********************************************************************/
    /* Rebuild the indexed file to a new file                           */
    /********************************************************************/
    _lseek (sSrcHdl, 0L, SEEK_SET);
    printf ("\nRebuild the indexed file to a new file %s ", szDstFil);
    if (-1 == (sDstHdl = _open (szDstFil,
        O_BINARY | O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE))) {
        printf ("Cannot open destination file: %s\n", szDstFil);
        _close (sSrcHdl);
        exit (-1);
    }
    RebIdxFil (sSrcHdl, sDstHdl, NULL, NULL, ucWrkBuf, TMPWRKBUF, &rbRebBlk);
    _close (sDstHdl);
    printf ("\n");

    /********************************************************************/
    /* Chop an indexed file into its component parts                    */
    /********************************************************************/
    _lseek (sSrcHdl, 0L, SEEK_SET);
    printf ("\nChop an indexed file into its component parts:\n");
    ChpIdxFil (sSrcHdl, ".vox", ".txt", IdxChkCBk, NULL, IdxEndCBk, 
        (DWORD) (LPVOID) &ulOutCnt, &cbChpBlk);

    /********************************************************************/
    /* Pack the component parts back into an indexed file               */
    /********************************************************************/
    _close (sSrcHdl);
    printf ("\nPack the component parts back into an indexed file:\n");
    PakIdxFil ("*.vox", "*.txt", szDstFil, IdxChkCBk, NULL, IdxEndCBk, 
        (DWORD) (LPVOID) &ulOutCnt, &kbPakBlk);

    /********************************************************************/
    /* Free indexed file resources                                      */
    /********************************************************************/
    IdxFilTrm ();

    /********************************************************************/
    /********************************************************************/
    printf ("\n%ld output bytes chopped and packed.\n", ulOutCnt);
    exit (0);

}

/************************************************************************/
/************************************************************************/
void    UseErrEnd (char *SError)
{
    if (SError != NULL) printf ("Bad Switch: %s\n", SError);
    printf (MsgDspUse);

#if (defined (_QWINVER)) /***********************************************/
_wsetexit (_WINEXITPERSIST);
#endif /*****************************************************************/
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
BOOL  FAR PASCAL _loadds IdxChkCBk (LPCSTR szFulNam, DWORD ulCBkDat)
{
    static char szFilDrv[_MAX_DRIVE], szFilDir[_MAX_DIR];   
    static char szFilNam[_MAX_FNAME], szFilExt[_MAX_EXT];   

    /********************************************************************/
    /* Separate file name from full path & print                        */
    /********************************************************************/
    _splitpath (szFulNam, szFilDrv, szFilDir, szFilNam, szFilExt);
    strcat (szFilNam, szFilExt);
    printf ("%Fs ", (LPSTR) szFilNam);

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);
}

BOOL  FAR PASCAL _loadds IdxEndCBk (DWORD ulRepCnt, DWORD ulCBkDat)
{
    DWORD FAR * lpOutCnt = (DWORD FAR *) ulCBkDat;
    *lpOutCnt += ulRepCnt;
    printf ("\n");

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);
}

/************************************************************************/
/*              Cooperative yield to other processes                    */
/************************************************************************/
WORD FAR PASCAL _loadds IdxPolPrc (LPVOID lpRsv001, DWORD ulRsv002, DWORD ulRsv003)
{
#if (defined (_QWINVER)) /***********************************************/
    _wyield(); 

    /********************************************************************/
    /* Note: printf() requires _loadds for a QuickWin DLL callback.     */
    /* The RC compiler will warn that Segment 1 (offset 034A) contains  */
	/* a relocation record pointing to the automatic data segment. This */
    /* will cause the program to crash if the instruction being fixed   */
    /* up is executed in a multi-instance application. Remove printf    */
    /* and _loadds to overcome this limitation; then remove the SINGLE  */
    /* attribute from the .DEF file to permit multiple instances.       */
    /********************************************************************/
#endif /*****************************************************************/

    /********************************************************************/
    /* Print "." to indicate continued processing                       */
    /********************************************************************/
    printf (".");

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);

}
