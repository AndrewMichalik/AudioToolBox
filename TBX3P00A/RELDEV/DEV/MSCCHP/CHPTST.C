/************************************************************************/
/* Sound Chop Test: ChpTst.c                                            */
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
#include "..\chplib\visdef.h"           /* VIS Standard type defs       */
#endif /*****************************************************************/
#include "..\chplib\genchp.h"           /* ToolBox Sound Chop defs      */

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
WORD    TBxGetDig (char far *, WORD far *, DWORD far *);
void    UseErrEnd (char *);
void    DspHlpEnd (void);
BOOL    FAR PASCAL _loadds ChpOpnCBk (short FAR *, DWORD);
BOOL    FAR PASCAL _loadds ChpClsCBk (short, DWORD);
WORD    FAR PASCAL _loadds ChpPolPrc (LPVOID, DWORD, DWORD);

/************************************************************************/
/*                          Help Messages                               */
/************************************************************************/
char  * MsgUseIni =  
"\n\
 ToolBox Test [-h for Help] v3.00a.1 Voice Information Systems 1-800-234-VISI\n\n";

char  * MsgDspUse =  
"Usage: ChpTst [-help] OldFile [-d]\n";

char  * MsgDspHlp = "\
       OldFile:  Source filename (Multimedia Wave format).\n\
       -dXXXXXX: Digitized data format, default Dialogic 4 bit.\n\
                 DLG24K = Dialogic   4 bit @ 6kHz (default).\n\
                 DLG32K = Dialogic   4 bit @ 8kHz.\n\
                 DLG48K = Dialogic   8 bit @ 6kHz.\n\
                 DLG64K = Dialogic   8 bit @ 8kHz.\n\
                 G2132K = G.721      4 bit @ 8kHz. (G.721 version only)\n\
                 NVV24K = NewVoice   1 bit @ 6kHz. (NewVoice version only)\n\
                 NVV32K = NewVoice   1 bit @ 8kHz. (NewVoice version only)\n\
                 PTC32K = Perception 1 bit @ 8kHz. (Perception version only)\n\
";

/************************************************************************/
/*                  Sound Chop Communication Blocks                     */
/************************************************************************/
CHPBLK  cbChpBlk;                       /* Function communication block */

/************************************************************************/
/************************************************************************/
void main(int argc, char **argv, char **envp)
{
    static char szSrcFil[MAXFNMLEN] = {'\0'};       /* Static for win   */ 
    static char szDstFil[MAXFNMLEN] = {'\0'};
    short       sSrcHdl;
    DWORD       ulOutCnt;

    /********************************************************************/
    /********************************************************************/
    printf (MsgUseIni);

    /********************************************************************/
    /* Check the version number of support modules (DOS Lib / Win DLLs) */
    /********************************************************************/
    if (CHPVERNUM != ChpSndVer()) UseErrEnd ("Wrong Library Version Number!\n");

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
    /* Initialize and allocate sound chop resources                     */
    /* Note: Use "_DEBUG_" flag to prevent test for accelerator key     */
    /********************************************************************/
    #if (defined (DEB)) /************************************************/
        #define KEYFLG  (DWORD)(LPSTR)"_DEBUG_"        
    #else
        #define KEYFLG  0L        
    #endif /*************************************************************/
    switch (ChpSndIni (0, 0L, KEYFLG)) {
        case SI_CHPNO_ERR: 
            break;
        case SI_CHPKEYERR:
            printf ("Key/License error, 8 section limit.\n");
            break;
        case SI_CHPVERERR:
            printf ("Support software module version mismatch, continuing...\n");
            break;
        case SI_CHPKEYERR | SI_CHPVERERR:
            printf ("Key/License error, 8 section limit.\n");
            printf ("Support software module version mismatch, continuing...\n");
            break;
        default:
            UseErrEnd ("Unknown error, exiting...\n");
            exit (-1);
            break;
    }

    /********************************************************************/
    /* Initialize function communication block                          */
    /********************************************************************/
    ChpCfgIni (&cbChpBlk);

    /********************************************************************/
    /* Get default parameters                                           */
    /********************************************************************/
    cbChpBlk.usSrcFmt = WAVHDRFMT;      /* Source file format           */
    cbChpBlk.usSrcPCM = MPCPCM016;      /* Source PCM type              */
    cbChpBlk.usSrcChn = 1;              /* Source file chan count       */
    cbChpBlk.usSrcBIO = 0;              /* Source bit encoding          */
    cbChpBlk.ulSrcFrq = 11025L;         /* Source sample frequency      */
    cbChpBlk.ulSrcOff = 0L;             /* Source byte offset           */
    cbChpBlk.ulSrcLen = 0xffffffffL;    /* Source byte length           */
    cbChpBlk.mhSrcCfg = (VISMEMHDL) 0;  /* Source cfg memory buff       */       
    cbChpBlk.usDstFmt = WAVHDRFMT;      /* Destination file format      */
    cbChpBlk.flChpFrm = CHPFRMDEF;      /* FndSndEvt frame size   (sec) */
    cbChpBlk.flChpRes = CHPRESDEF;      /* FndSndEvt resolution   (sec) */
    cbChpBlk.flChpSnd = CHPSNDDEF;      /* FndSndEvt snd thresh  (%max) */
    cbChpBlk.usChpAtk = CHPATKDEF;      /* FndSndEvt atk ratio   (%frm) */
    cbChpBlk.usChpDcy = CHPDCYDEF;      /* FndSndEvt dcy ratio   (%frm) */
    cbChpBlk.flChpGrd = CHPGRDDEF;      /* FndSndEvt guard time   (sec) */
    cbChpBlk.mhGloCfg = (VISMEMHDL) 0;  /* Global cfg memory buff       */       
    cbChpBlk.lpPolDsp = NULL;           /* Chop poll display proc       */
    cbChpBlk.ulRsvPol = 0L;             /* Reserved                     */
    cbChpBlk.lpMsgDsp = NULL;           /* Reserved                     */
    cbChpBlk.ulRsvErr = 0L;             /* Reserved                     */

    /********************************************************************/
    /* Get dash parameters; assume output is Multimedia Wave            */
    /********************************************************************/
    strcpy (szDstFil, "0.wav");
    while (argc-- > 0) {
        if ((!strncmp (*argv, "-d", 2)) && (0 == TBxGetDig (&(*argv)[2], 
            &cbChpBlk.usSrcPCM, &cbChpBlk.ulSrcFrq))) {
                /********************************************************/
                /* Force output to vox format                           */
                /********************************************************/
                cbChpBlk.usSrcFmt = cbChpBlk.usDstFmt = ALLPURFMT; 
                strcpy (szDstFil, "0.vox");
            }
        else UseErrEnd (*argv);
        *argv++;
    }

    /********************************************************************/
    /* Open Wav input file                                              */
    /********************************************************************/
    if (-1 == (sSrcHdl = _open (szSrcFil, O_BINARY | O_RDONLY, 0))) {
        printf ("Cannot open source file: %s\n", szSrcFil);
        exit (-1);
    }

    /********************************************************************/
    /* Override default display routine                                 */
    /********************************************************************/
    cbChpBlk.lpPolDsp = ChpPolPrc;

    /********************************************************************/
    /* Chop the PCM file                                                */
    /********************************************************************/
    ulOutCnt = ChpSndFil (sSrcHdl, NULL, ChpOpnCBk, NULL, ChpClsCBk, 
        (DWORD) (LPSTR) szDstFil, &cbChpBlk);

    /********************************************************************/
    /********************************************************************/
    _close (sSrcHdl);

    /********************************************************************/
    /* Free Sound Chop resources                                        */
    /********************************************************************/
    ChpSndTrm ();

    /********************************************************************/
    /********************************************************************/
    printf ("\n%ld output bytes extracted.\n", ulOutCnt);
    exit (0);

}

/************************************************************************/
/************************************************************************/
WORD    TBxGetDig (char far *szDigTyp, WORD far *pusPCMTyp, DWORD far *pulSmpFrq)
{

    if (!_fstricmp (szDigTyp, "DLG24K")) {
        *pusPCMTyp = DLGPCM004;
        *pulSmpFrq = 6000L;
        return (0);
    }
    if (!_fstricmp (szDigTyp, "DLG32K")) {
        *pusPCMTyp = DLGPCM004;
        *pulSmpFrq = 8000L;
        return (0);
    }
    if (!_fstricmp (szDigTyp, "DLG48K")) {
        *pusPCMTyp = DLGPCM008;
        *pulSmpFrq = 6000L;
        return (0);
    }
    if (!_fstricmp (szDigTyp, "DLG64K")) {
        *pusPCMTyp = DLGPCM008;
        *pulSmpFrq = 8000L;
        return (0);
    }

#if (defined (G21)) /****************************************************/
    if (!_fstricmp (szDigTyp, "G2132K")) {
        *pusPCMTyp = G21PCM004;
        *pulSmpFrq = 8000L;
        return (0);
    }
#endif /*****************************************************************/

#if (defined (NWV)) /****************************************************/
    if (!_fstricmp (szDigTyp, "NWV24K")) {
        *pusPCMTyp = NWVPCM001;
        *pulSmpFrq = 24000L;
        return (0);
    }
    if (!_fstricmp (szDigTyp, "NWV32K")) {
        *pusPCMTyp = NWVPCM001;
        *pulSmpFrq = 32000L;
        return (0);
    }
#endif /*****************************************************************/
#if (defined (PTC)) /****************************************************/
    if (!_fstricmp (szDigTyp, "PTC32K")) {
        *pusPCMTyp = PTCPCM001;
        *pulSmpFrq = 32000L;
        return (0);
    }
#endif /*****************************************************************/

    /********************************************************************/
    /********************************************************************/
    printf ("Bad Digitizer selection: %s, using default...\n", szDigTyp);
    return ((WORD) -1);

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
BOOL FAR PASCAL _loadds ChpOpnCBk (short FAR *lpsDstHdl, DWORD ulCBkDat)
{
    LPSTR   lpDstSpc = (LPSTR) ulCBkDat;

    /********************************************************************/
    /* Increment file name to create sequential files                   */
    /********************************************************************/
    (*lpDstSpc)++;
    printf ("\n%Fs", (LPSTR) lpDstSpc);

    /********************************************************************/
    /* Open and truncate file to zero length                            */
    /********************************************************************/
    if (-1 == (*lpsDstHdl = _open (lpDstSpc,
      O_BINARY | O_CREAT | O_TRUNC | O_WRONLY, S_IREAD | S_IWRITE))) {
        printf ("Cannot Open Target File: %Fs, skipping...\n", (LPSTR) lpDstSpc);
        return (TRUE);
    }

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);
}

BOOL FAR PASCAL _loadds ChpClsCBk (short sDstHdl, DWORD ulCBkDat)
{
    /********************************************************************/
    /********************************************************************/
    _close (sDstHdl);

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