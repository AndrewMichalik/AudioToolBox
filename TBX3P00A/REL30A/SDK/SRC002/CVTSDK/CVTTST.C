/************************************************************************/
/* PCM Convertor Test: CvtTst.c                                         */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
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
#include "..\cvtlib\visdef.h"           /* VIS Standard type defs       */
#endif /*****************************************************************/
#include "..\cvtlib\gencvt.h"           /* ToolBox PCM conversion defs  */

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
WORD    FAR PASCAL _loadds CvtPolPrc (LPVOID, DWORD, DWORD);

/************************************************************************/
/*                          Help Messages                               */
/************************************************************************/
char  * MsgUseIni =  
"\n\
 ToolBox Test [-h for Help] v3.00a.1 Voice Information Systems 1-800-234-VISI\n\n";

char  * MsgDspUse =  
"Usage: CvtTst [-help] OldFile NewFile [-d]\n";

char  * MsgDspHlp = "\
       OldFile:  Source filename (Multimedia Wave format).\n\
       NewFile:  Destination filename, if different from OldFile.\n\
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
/*                  Conversion Communication Blocks                     */
/************************************************************************/
CVTBLK  cbCvtBlk;                       /* Function communication block */

/************************************************************************/
/************************************************************************/
void main(int argc, char **argv, char **envp)
{
    static char szSrcFil[MAXFNMLEN] = {'\0'};       /* Static for win   */ 
    static char szDstFil[MAXFNMLEN] = {'\0'};
    short       sSrcHdl;
    short       sDstHdl;
    DWORD       ulOutCnt;

    /********************************************************************/
    /********************************************************************/
    printf (MsgUseIni);

    /********************************************************************/
    /* Check the version number of support modules (DOS Lib / Win DLLs) */
    /********************************************************************/
    if (CVTVERNUM != CvtPCMVer()) UseErrEnd ("Wrong Library Version Number!\n");

    /********************************************************************/
    /* Check argument count. Skip program name argument                 */
    /********************************************************************/
    if (--argc && (0 == strncmp (*++argv, "-h", 2))) DspHlpEnd ();
    if (argc < 2) UseErrEnd (NULL);

    /********************************************************************/
    /* Get name of source & destination file                            */
    /********************************************************************/
    strncpy (szSrcFil, *argv++, MAXFNMLEN);
    argc--;                                     /* Source file arg used */
    strncpy (szDstFil, *argv++, MAXFNMLEN);
    argc--;                                     /* Dest   file arg used */

    /********************************************************************/
    /* Initialize and allocate conversion resources                     */
    /* Note: Use "_DEBUG_" flag to prevent test for accelerator key     */
    /********************************************************************/
    #if (defined (DEB)) /************************************************/
        #define KEYFLG  (DWORD)(LPSTR)"_DEBUG_"        
    #else
        #define KEYFLG  0L        
    #endif /*************************************************************/
    switch (CvtPCMIni (0, 0L, KEYFLG)) {
        case SI_CVTNO_ERR: 
            break;
        case SI_CVTKEYERR:
            printf ("Key/License error, 8 second limit.\n");
            break;
        case SI_CVTVERERR:
            printf ("Support software module version mismatch, continuing...\n");
            break;
        case SI_CVTKEYERR | SI_CVTVERERR:
            printf ("Key/License error, 8 second limit.\n");
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
    CvtCfgIni (&cbCvtBlk);

    /********************************************************************/
    /*          Default .Wav to .Vox conversion block parameters        */
    /********************************************************************/
    cbCvtBlk.usSrcFmt = WAVHDRFMT;      /* Source file format           */
    cbCvtBlk.usSrcPCM = MPCPCM016;      /* Source PCM type              */
    cbCvtBlk.usSrcChn = 1;              /* Source file chan count       */
    cbCvtBlk.usSrcBIO = 0;              /* Source bit encoding          */
    cbCvtBlk.ulSrcFrq = 11025L;         /* Source sample frequency      */
    cbCvtBlk.ulSrcOff = 0L;             /* Source byte offset           */
    cbCvtBlk.ulSrcLen = 0xffffffffL;    /* Source byte length           */
    cbCvtBlk.mhSrcCfg = (VISMEMHDL) 0;  /* Source cfg memory buff       */       
    cbCvtBlk.usDstFmt = ALLPURFMT;      /* Destination file format      */
    cbCvtBlk.usDstPCM = DLGPCM004;      /* Destination PCM type         */
    cbCvtBlk.usDstChn = 1;              /* Destination file chan count  */
    cbCvtBlk.usDstBIO = 0;              /* Destination bit encoding     */
    cbCvtBlk.ulDstFrq = 6000L;          /* Destination sample frequency */
    cbCvtBlk.ulDstOff = 0L;             /* Destination byte offset      */
    cbCvtBlk.ulDstLen = 0xffffffffL;    /* Destination byte length      */
    cbCvtBlk.mhDstCfg = (VISMEMHDL) 0;  /* Destination cfg memory buff  */ 
    cbCvtBlk.flCrpFrm = 0;              /* Auto Crop frame length (sec) */
    cbCvtBlk.flCrpRes = 0;              /* Auto Crop resolution   (sec) */
    cbCvtBlk.flCrpSnd = 0;              /* Auto Crop threshold   (%max) */
    cbCvtBlk.usCrpAtk = 0;              /* Auto Crop atk ratio   (%frm) */
    cbCvtBlk.usCrpDcy = 0;              /* Auto Crop dcy ratio   (%frm) */
    cbCvtBlk.flCrpGrd = CRPGRDDEF;      /* Auto Crop guard time   (sec) */
    cbCvtBlk.flVolAdj = 0;              /* Volume adjust mult      (dB) */
    cbCvtBlk.usNrmLvl = 0;              /* Volume norm target level (%) */
    cbCvtBlk.flNrmExc = 0;              /* Volume norm except thrsh (%) */
    cbCvtBlk.usNrmMax = 0;              /* Volume norm adjust max  (dB) */
    cbCvtBlk.flDCELvl = 0;              /* DynCmpExp  level        (dB) */
    cbCvtBlk.usCmpThr = 0;              /* Compressor threshold     (%) */
    cbCvtBlk.usCmpAtk = 0;              /* Compressor attack time  (Hz) */
    cbCvtBlk.flCmpMin = 0;              /* Compressor minimum      (dB) */
    cbCvtBlk.usExpThr = 0;              /* Expander   threshold     (%) */
    cbCvtBlk.usExpAtk = 0;              /* Expander   attack time  (Hz) */
    cbCvtBlk.flExpMax = 0;              /* Expander   maximum      (dB) */
    cbCvtBlk.usCE_Dcy = 0;              /* Cmp/Exp    decay time   (Hz) */
    cbCvtBlk.usNoiThr = 0;              /* Noise Gate threshold     (%) */
    cbCvtBlk.flNoiAtt = 0;              /* Noise Gate attenuation  (dB) */
    cbCvtBlk.usTDLAAF = 2;              /* Time Domain Anti-alias (Ord) */
    cbCvtBlk.bfTDLFst = FALSE;          /* Anti-alias fast math on/off  */
    cbCvtBlk.bfFFTDTM = FALSE;          /* FFT DTMF filter   on/off     */
    cbCvtBlk.bfFFTAAF = FALSE;          /* FFT Anti-aliasing on/off     */
    cbCvtBlk.bfFFTFEq = FALSE;          /* FFT Freq Equalize on/off     */
    cbCvtBlk.bfFFTRes = FALSE;          /* FFT Freq Resample on/off     */
    cbCvtBlk.usFFTOrd = FFTORDDEF;      /* FFT filter size              */
    cbCvtBlk.flFEqGai = 0;              /* Freq equalizer gain adj (dB) */
    cbCvtBlk.ulFEqBot = FEQBOTDEF;      /* Freq equalizer bot freq (Hz) */
    cbCvtBlk.ulFEqTop = FEQTOPDEF;      /* Freq equalizer top freq (Hz) */
    cbCvtBlk.usFEqCnt = FEQCNTDEF;      /* Freq equalizer point count   */
    cbCvtBlk.flFEqPnt[0] = FEQVOX001;   /*  300- 800: +3dB (<300Hz:0dB) */
    cbCvtBlk.flFEqPnt[1] = FEQVOX002;   /*  800-1300: +6dB              */
    cbCvtBlk.flFEqPnt[2] = FEQVOX003;   /* 1300-1800: +6dB              */
    cbCvtBlk.flFEqPnt[3] = FEQVOX004;   /* 1800-2300: +6dB              */
    cbCvtBlk.flFEqPnt[4] = FEQVOX005;   /* 2300-2800: +6dB              */
    cbCvtBlk.flFEqPnt[5] = FEQVOX006;   /* 2800-3300: +6dB              */
    cbCvtBlk.flFEqPnt[6] = FEQVOX007;   /* 3300-3800: +3dB (>3.8Kz:0dB) */
    cbCvtBlk.mhGloCfg = (VISMEMHDL) 0;  /* Global cfg memory buff       */       
    cbCvtBlk.lpPolDsp = NULL;           /* Conversion poll display proc */
    cbCvtBlk.ulRsvPol = 0L;             /* Reserved                     */
    cbCvtBlk.lpMsgDsp = NULL;           /* Reserved                     */
    cbCvtBlk.ulRsvErr = 0L;             /* Reserved                     */

//    /********************************************************************/
//    /*          Default .Vox to .Wav conversion block parameters        */
//    /********************************************************************/
//    cbCvtBlk.usSrcFmt = ALLPURFMT;      /* Source file format           */
//    cbCvtBlk.usSrcPCM = DLGPCM004;      /* Source PCM type              */
//    cbCvtBlk.usSrcChn = 1;              /* Source file chan count       */
//    cbCvtBlk.usSrcBIO = 0;              /* Source bit encoding          */
//    cbCvtBlk.ulSrcFrq = 6000L;          /* Source sample frequency      */
//    cbCvtBlk.ulSrcOff = 0L;             /* Source byte offset           */
//    cbCvtBlk.ulSrcLen = 0xffffffffL;    /* Source byte length           */
//    cbCvtBlk.mhSrcCfg = (VISMEMHDL) 0;  /* Source cfg memory buff       */ 
//    cbCvtBlk.usDstFmt = WAVHDRFMT;      /* Destination file format      */
//    cbCvtBlk.usDstPCM = MPCPCM016;      /* Destination PCM type         */
//    cbCvtBlk.usDstChn = 1;              /* Destination file chan count  */
//    cbCvtBlk.usDstBIO = 0;              /* Destination bit encoding     */
//    cbCvtBlk.ulDstFrq = 11025L;         /* Destination sample frequency */
//    cbCvtBlk.ulDstOff = 0L;             /* Destination byte offset      */
//    cbCvtBlk.ulDstLen = 0xffffffffL;    /* Destination byte length      */
//    cbCvtBlk.mhDstCfg = (VISMEMHDL) 0;  /* Destination cfg memory buff  */       
    
    /********************************************************************/
    /* Get dash parameters                                              */
    /********************************************************************/
    TBxGetDig ("DLG24K", &cbCvtBlk.usDstPCM, &cbCvtBlk.ulDstFrq);
    while (argc-- > 0) {
        if (!strncmp (*argv, "-d", 2)) TBxGetDig (&(*argv)[2], &cbCvtBlk.usDstPCM, &cbCvtBlk.ulDstFrq); 
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
    /* Open Vox output file                                             */
    /********************************************************************/
    if (-1 == (sDstHdl = _open (szDstFil,
        O_BINARY | O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE))) {
        printf ("Cannot open destination file: %s\n", szDstFil);
        _close (sSrcHdl);
        exit (-1);
    }

    /********************************************************************/
    /* Override default display routine                                 */
    /********************************************************************/
    cbCvtBlk.lpPolDsp = CvtPolPrc;

    /********************************************************************/
    /* Convert the PCM file                                             */
    /********************************************************************/
    ulOutCnt = CvtPCMFil (sSrcHdl, sDstHdl, NULL, NULL, NULL, 0, &cbCvtBlk);

    /********************************************************************/
    /********************************************************************/
    _close (sSrcHdl);
    _close (sDstHdl);

    /********************************************************************/
    /* Free conversion resources                                        */
    /********************************************************************/
    CvtPCMTrm ();

    /********************************************************************/
    /********************************************************************/
    printf ("\n%ld output bytes converted.\n", ulOutCnt);
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
/*              Cooperative yield to other processes                    */
/************************************************************************/
WORD FAR PASCAL _loadds CvtPolPrc (LPVOID lpRsv001, DWORD ulRsv002, DWORD ulRsv003)
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