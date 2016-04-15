/************************************************************************/
/* Speed Adjust Param Config: SpdCfg.c                  V2.00  08/15/95 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#if (defined (W31)) /****************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#include "..\os_dev\winmem.h"           /* Generic memory supp defs     */
#endif /*****************************************************************/
#if (defined (DOS)) /****************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "..\os_dev\dosmem.h"           /* Generic memory support defs  */
#endif /*****************************************************************/
#include "..\fiodev\genfio.h"           /* Generic File I/O definitions */
#include "..\fiodev\filutl.h"           /* File utilities definitions   */
#include "..\pcmdev\genpcm.h"           /* PCM/APCM conv routine defs   */
#include "..\pcmdev\pcmsup.h"           /* PCM/APCM xlate lib defs      */
#include "..\os_dev\cfgsup.h"           /* Configuration support        */
#include "spdlib\genspd.h"              /* Speed Adjust defs            */
#include "spdsup.h"                     /* Speed Adjust support         */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */
#include <stdio.h>                      /* Standard I/O                 */
#include <math.h>                       /* Math library defs            */

/************************************************************************/
/*                          Forward References                          */
/************************************************************************/
int     GetFmtPCM (char *, FILFMT *, PCMTYP *);
int     GetSwpFlg (char *, WORD *);
int     GetSmpFrq (char *, DWORD *);
DWORD   GetFrqOvr (PCMTYP, DWORD, DWORD);

/************************************************************************/
/*                          Help Messages                               */
/************************************************************************/
char  * MsgUseIni =  
"\n\
        Voice Information Systems (1-800-234-VISI)  Speed Adjust Utility\n\
        v3.00x.0 (C) Copyright A.J. Michalik 1987-1996 U.S. Pat. Pending\n\n";

char  * MsgDspUse =  
"Usage: SpdAdj [-help] OldFile [NewFile] [-b -di -fi -ng -ps -sm -va]\n";

char  * MsgDspHlp = "\
       OldFile: Source filename.\n\
       NewFile: Destination filename, if different from OldFile.\n\
       -b:      Backup file creation inhibited.\n\
       -diXXXX: Data Input, default Wave Multimedia Format.\n\
                DLG004 = Dialogic 4 bit (OKI)   (6 kHz).\n\
                DLG008 = Dialogic 8 bit (u-Law) (6 kHz).\n\
                G11F08 = C-ITU (CCITT) G.711 (Folded)   u-Law 8 bit (8 kHz).\n\
                G11I08 = C-ITU (CCITT) G.711 (Inverted) a-Law 8 bit (8 kHz).\n"
#if (defined (G21)) /****************************************************/
"\
                G21004 = C-ITU (CCITT) G.721 4 bit (8 kHz).\n"
#endif /*****************************************************************/
#if (defined (HAR)) /****************************************************/
"\
                HAR001 = Harris Semiconductor 1 bit (32 kHz).\n"
#endif /*****************************************************************/
#if (defined (NWV)) /****************************************************/
"\
                NWV001 = New Voice 1 bit (32 kHz).\n"
#endif /*****************************************************************/
#if (defined (PTC)) /****************************************************/
"\
                PTC001 = Perception  1 bit (32 kHz).\n"
#endif /*****************************************************************/
#if (defined (RTX)) /****************************************************/
#endif /*****************************************************************/
"\
                VIS016 = VIS Interchange Format 16 bit (11.025 kHz).\n\
                WAV008 = Wave Multimedia Format  8 bit (11.025 kHz).\n\
                WAV016 = Wave Multimedia Format 16 bit (11.025 kHz).\n\
       -fiX.x : Frequency of Input  override (X.x kHz).\n\
       -lfXXXX: Length of FFT input buffer, default 512 points.\n\
       -lwXXXX: Length of FFT data window, default 512 points.\n\
       -smX.x:  Speed time multiplier, default is 1.\n\
       -va:     Volume Adjust, default is 0 dB.\n\
Notes: Environment TMP= specifies temp work files directory.\n";

/************************************************************************/
/*                  Speed Adjust Communication Blocks                   */
/************************************************************************/
CFGBLK  cbCfgBlk = {                    /* Configuration file block     */
    0,                                  /* File replacement flags       */
    0,                                  /* Input  config file name      */       
    0,                                  /* Global config file name      */       
};
SPDBLK  sbSpdBlk;                       /* Function communication block */

/************************************************************************/
/*                  System Support Module Initialization                */
/************************************************************************/
WORD    SysPrmIni (WORD usArgCnt, char **ppcArgArr, EXIMOD *pemExiMod)
{
    char    szKeyFil[FIOMAXNAM] = {'\0'};
    char    szKeyStr[FIOMAXNAM] = {'\0'};
    char    szCurDir[FIOMAXNAM] = {'\0'};
    char    szAppDir[FIOMAXNAM] = {'\0'};
    WORD    usRetCod = 0;
    WORD    usi;

    /********************************************************************/
    /* Get current working directory (with trailing slash).             */
    /* Get application executable directory; remove program file name.  */
    /********************************************************************/
    xgetdcwd (szCurDir, FIOMAXNAM - 1);
    usi = _fstrlen (_fstrncpy (szAppDir, *ppcArgArr, FIOMAXNAM));
    while (usi--) if ('\\' == szAppDir[usi]) {
        szAppDir[usi+1] = '\0';        
        break;
    }    

    /********************************************************************/
    /* Set working directory to one containing the application; this    */
    /* insures that DLL's are found (if not in path).                   */
    /********************************************************************/
    xsetdrive (szAppDir[0], NULL);
    xchdir (szAppDir);

    /********************************************************************/
    /* Check the version numbers of support modules (Win DLL's)         */
    /********************************************************************/
    if (SPDVERNUM != SpdAdjVer()) {
        printf ("Main software module version mismatch, exiting...\n");
        usRetCod = (WORD) -1;
    }

    /********************************************************************/
    /* Check for existence of interim license file                      */
    /********************************************************************/
    #define TBXLICFIL   "tbxlic.txt"
    if (!FilGetSta (TBXLICFIL, NULL)) strncpy (szKeyFil, TBXLICFIL, FIOMAXNAM);

    /********************************************************************/
    /* Scan for debug and licensing params; initialize debug status.    */
    /********************************************************************/
    while (usArgCnt--) {
        if (!strncmp (*ppcArgArr, "-debug", 6)) SpdSetDeb ((WORD) strtol (&(*ppcArgArr)[6], NULL, 16));
        else if (!strncmp (*ppcArgArr, "-exit", 5)) *pemExiMod = (EXIMOD) strtol (&(*ppcArgArr)[5], NULL, 16);
        else if (!strncmp (*ppcArgArr, "-kf", 3)) strncpy (szKeyFil, &(*ppcArgArr)[3], FIOMAXNAM); 
        else if (!strncmp (*ppcArgArr, "-ks", 3)) strncpy (szKeyStr, &(*ppcArgArr)[3], FIOMAXNAM); 
        *ppcArgArr++;
    }

    /********************************************************************/
    /* Initialize and allocate conversion resources                     */
    /* Note: Support services must be initialized for query functions   */
    /********************************************************************/
    if (!usRetCod) 
    switch (SpdAdjIni (0, (DWORD)(LPSTR) szKeyFil, (DWORD)(LPSTR) szKeyStr)) {
        case SI_SPDNO_ERR: 
            break;
        case SI_SPDKEYERR:
            printf ("Key/License error, 8 second limit.\n");
            break;
        case SI_SPDVERERR:
            printf ("Support software module version mismatch, continuing...\n");
            break;
        case SI_SPDKEYERR | SI_SPDVERERR:
            printf ("Key/License error, 8 second limit.\n");
            printf ("Support software module version mismatch, continuing...\n");
            break;
        default:
            printf ("Unknown error, exiting...\n");
            usRetCod = (WORD) -1;
            break;
    }

    /********************************************************************/
    /* Re-set working directory to original directory.                  */
    /********************************************************************/
    xsetdrive (szCurDir[0], NULL);
    xchdir (szCurDir);

    /********************************************************************/
    /* Return zero if processing should continue, non-zero to abort     */
    /********************************************************************/
    return (usRetCod);

}

WORD    SysPrmTrm ()
{
    /********************************************************************/
    /* Free speed adjust resources                                      */
    /********************************************************************/
    SpdAdjTrm ();
    return (0);
}

/************************************************************************/
/*                  Command Line Dash Parameter Support                 */
/************************************************************************/
char *  DshPrmLod (WORD usArgCnt, char **ppcArgArr, CFGBLK *pcbCfgBlk, 
        SPDBLK *lpSpdBlk)
{
    DWORD   ulSrcFrqOvr = 0L;              /* Sample frequency override    */    

    /********************************************************************/
    /* Initialize function communication block                          */
    /********************************************************************/
    SpdCfgIni (lpSpdBlk);

    /********************************************************************/
    /* Get default parameters                                           */
    /********************************************************************/
    lpSpdBlk->usSrcFmt = WAVHDRFMT;     /* Source file format           */
    lpSpdBlk->usSrcPCM = MPCPCM016;     /* Source PCM type              */
    lpSpdBlk->usSrcChn = 1;             /* Source file chan count       */
    lpSpdBlk->usSrcBIO = PCMBIODEF;     /* Source bit encoding          */
    lpSpdBlk->ulSrcFrq = 11025L;        /* Source sample frequency      */
    lpSpdBlk->ulSrcOff = 0L;            /* Source byte offset           */
    lpSpdBlk->ulSrcLen = 0xffffffffL;   /* Source byte length           */
    lpSpdBlk->mhSrcCfg = (VISMEMHDL) 0; /* Source cfg memory buff       */       
    lpSpdBlk->usDstFmt = WAVHDRFMT;     /* Destination file format      */
    lpSpdBlk->usDstPCM = MPCPCM016,     /* Destination PCM type         */
    lpSpdBlk->usDstChn = 1,             /* Destination file chan count  */
    lpSpdBlk->usDstBIO = PCMBIODEF,     /* Destination bit encoding     */
    lpSpdBlk->ulDstFrq = 11025L,        /* Destination sample frequency */
    lpSpdBlk->ulDstOff = 0L,            /* Destination byte offset      */
    lpSpdBlk->ulDstLen = 0xffffffffL,   /* Destination byte length      */
    lpSpdBlk->mhDstCfg = (VISMEMHDL) 0, /* Destination cfg memory buff  */ 
    lpSpdBlk->flVolAdj = 0,             /* Volume adjust multiplier     */
    lpSpdBlk->usFFTOrd = FFTORDDEF,     /* FFT filter size              */
    lpSpdBlk->usWinOrd = WINORDDEF,     /* FFT window size              */
    lpSpdBlk->flSpdMul = SPDMULDEF,     /* Speed multiplier default     */
    lpSpdBlk->flPchMul = PCHMULDEF,     /* Pitch multiplier default     */
    lpSpdBlk->flSynThr = SYNTHRDEF,     /* Vocoder synthesis threshold  */
//
//lpSpdBlk->bfFrcOBS = FALSE,         /* Force Osc Bank Syn flag      */
lpSpdBlk->bfFrcOBS = TRUE,
    lpSpdBlk->mhGloCfg = (VISMEMHDL) 0; /* Global cfg memory buff       */       
    lpSpdBlk->lpPolDsp = NULL;          /* Conversion poll display proc */
    lpSpdBlk->ulRsvPol = 0L;            /* Reserved                     */
    lpSpdBlk->lpMsgDsp = NULL;          /* Reserved                     */
    lpSpdBlk->ulRsvErr = 0L;            /* Reserved                     */

    /********************************************************************/
    /* Get dash parameters                                              */
    /* Note: ceil (LOGBASTWO (x)) may actually incr due to rounding err */
    /********************************************************************/
    while (usArgCnt--) {
        if (!strncmp (*ppcArgArr, "-debug", 6));    /* Handled in Ini   */
        else if (!strncmp (*ppcArgArr, "-exit", 5));/* Handled in Ini   */
        else if (!strncmp (*ppcArgArr, "-kf", 3));  /* Handled in Ini   */
        else if (!strncmp (*ppcArgArr, "-ks", 3));  /* Handled in Ini   */
        else if (FORALLVER && (!strncmp (*ppcArgArr, "-di", 3)) && !GetFmtPCM (&(*ppcArgArr)[3], &lpSpdBlk->usSrcFmt, &lpSpdBlk->usSrcPCM)); 
        else if (FORALLVER && (!strncmp (*ppcArgArr, "-fi", 3)) && !GetSmpFrq (&(*ppcArgArr)[3], &ulSrcFrqOvr)); 
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-lf", 3)) {
            lpSpdBlk->usFFTOrd = (WORD) strtol (&(*ppcArgArr)[3], NULL, 10);
            lpSpdBlk->usFFTOrd = lpSpdBlk->usFFTOrd ? (WORD) LOGBASTWO (lpSpdBlk->usFFTOrd) : FFTORDDEF;
        }
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-lw", 3)) {
            lpSpdBlk->usWinOrd = (WORD) strtol (&(*ppcArgArr)[3], NULL, 10);
            lpSpdBlk->usWinOrd = lpSpdBlk->usWinOrd ? (WORD) LOGBASTWO (lpSpdBlk->usWinOrd) : WINORDDEF;
        }
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-sm", 3)) lpSpdBlk->flSpdMul = (float) atof (&(*ppcArgArr)[3]); 
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-va", 3)) lpSpdBlk->flVolAdj = (float) atof (&(*ppcArgArr)[3]); 
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-b", 2)) pcbCfgBlk->usRepFlg |= BFRBKIFLG; 
        else return (*ppcArgArr);
        /****************************************************************/
        /****************************************************************/
        *ppcArgArr++;
    }

    /********************************************************************/
    /* Set sample frequency based upon input data type & override       */
    /********************************************************************/
    lpSpdBlk->ulSrcFrq = GetFrqOvr (lpSpdBlk->usSrcPCM, ulSrcFrqOvr, lpSpdBlk->ulSrcFrq);

    /********************************************************************/
    /* Force Output File format = Input format                          */
    /********************************************************************/
    lpSpdBlk->usDstFmt = lpSpdBlk->usSrcFmt;
    lpSpdBlk->usDstPCM = lpSpdBlk->usSrcPCM;
    lpSpdBlk->usDstChn = lpSpdBlk->usSrcChn;
    lpSpdBlk->usDstBIO = lpSpdBlk->usSrcBIO;
    lpSpdBlk->ulDstFrq = lpSpdBlk->ulSrcFrq;

    /********************************************************************/
    /********************************************************************/
    return (NULL);

}

WORD    DshPrmRel (SPDBLK FAR *lpSpdBlk)
{
    return (0);
}

/************************************************************************/
/************************************************************************/
CFGMAP  cmPCMTypMap = {
        6, "VIS000WAV000MPC008VIS008WAV008MPC016VIS016WAV016DLG004DLG008G11F08G11I08",
        UNKPCM000,
        UNKPCM000,
        MPCPCM008, 
        MPCPCM008, 
        MPCPCM008, 
        MPCPCM016, 
        MPCPCM016, 
        MPCPCM016, 
        DLGPCM004,
        DLGPCM008,
        G11PCMF08,
        G11PCMI08,
};
CFGMAP  cmFilFmtMap = {
        6, "VIS000VIS008VIS016MPC008MPC016WAV000WAV008WAV016DLG004DLG008G11F08G11I08",
        VISHDRFMT,
        VISHDRFMT,
        VISHDRFMT,
        ALLPURFMT,
        ALLPURFMT,
        WAVHDRFMT,
        WAVHDRFMT,
        WAVHDRFMT,
        ALLPURFMT,
        ALLPURFMT,
        ALLPURFMT,
        ALLPURFMT,
};

/************************************************************************/
/************************************************************************/
int GetFmtPCM (char * szTypNam, FILFMT *pusFilFmt, PCMTYP *pusPCMTyp)
{

    /********************************************************************/
    /********************************************************************/
    if (CfgPrmMap (&cmFilFmtMap, szTypNam, pusFilFmt)) return (-1);
    if (CfgPrmMap (&cmPCMTypMap, szTypNam, pusPCMTyp)) return (-1);
    return (0);

}

int GetSmpFrq (char * szSmpFrq, DWORD *pulSmpFrq)
{

    /********************************************************************/
    /********************************************************************/
    *pulSmpFrq = (DWORD) (atof (szSmpFrq) * 1000.);
    if ((*pulSmpFrq < SRCFRQMIN) || (*pulSmpFrq > SRCFRQMAX)) return (-1);
    return (0);

}

DWORD   GetFrqOvr (PCMTYP usPCMTyp, DWORD ulOvrFrq, DWORD ulDefFrq)
{
    if (ulOvrFrq) return (ulOvrFrq);
    if (ulOvrFrq = SpdCapQry (usPCMTyp, FRQDEFQRY, 0L)) return (ulOvrFrq);
    return (ulDefFrq);            
}

