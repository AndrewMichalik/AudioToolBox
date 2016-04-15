/************************************************************************/
/* Sound Chop Param Config: ChpCfg.c                    V2.00  08/15/95 */
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
#include "chplib\genchp.h"              /* Sound Chop defs              */
#include "chpsup.h"                     /* Sound Chop supp              */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */
#include <stdio.h>                      /* Standard I/O                 */

/************************************************************************/
/*                          Forward References                          */
/************************************************************************/
int     GetFmtPCM (char *, FILFMT *, PCMTYP *);
int     GetSwpFlg (char *, WORD *);
int     GetSmpFrq (char *, DWORD FAR *);
DWORD   GetFrqOvr (PCMTYP, DWORD, DWORD);

/************************************************************************/
/*                          Help Messages                               */
/************************************************************************/
char  * MsgUseIni =  
"\n\
       Voice Information Systems (1-800-234-VISI) Sound Chopping Utility\n\
       v3.00a.1 (C) Copyright A.J. Michalik 1987-1996  U.S. Pat. Pending\n\n";

char  * MsgDspUse =  
"Usage: SndChp [-help] SndFile [VoxExt -ac -co -di -fi -fl -fr -st -sa -sd]\n";

char  * MsgDspHlp = "\
       SndFile: Source filename.\n\
       VoxExt:  Destination audio file extension (Default is .vox).\n\
       -acX.x:  Auto-Crop guard time, default 0.05 seconds\n\
       -coXXXX: Destination file name counter offset, default is +0.\n\
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
       -fiX.x:  Frequency of Input  override (X.x kHz).\n\
       -flX.x:  Frame length, default 1.0 seconds.\n\
       -frX.x:  Resolution, default .01 seconds/bin.\n\
       -stXXXX: Sound threshold level (%%max), default 2.0%%.\n\
       -saXXXX: Sensitivity attack ratio (%%frame), default 20%%.\n\
       -sdXXXX: Sensitivity decay  ratio (%%frame), default 10%%.\n\
Notes: Environment TMP= specifies temp work files directory.\n";

/************************************************************************/
/*                      Chop Communication Blocks                       */
/************************************************************************/
CFGBLK  cbCfgBlk = {                    /* Configuration file block     */
    0L,                                 /* Dest file name counter off   */
    0,                                  /* Input  config file name      */       
    0,                                  /* Global config file name      */       
};
CHPBLK  cbChpBlk;                       /* Function communication block */

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
    if (CHPVERNUM != ChpSndVer()) {
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
        if (!strncmp (*ppcArgArr, "-debug", 6)) ChpSetDeb ((WORD) strtol (&(*ppcArgArr)[6], NULL, 16));
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
    switch (ChpSndIni (0, (DWORD)(LPSTR) szKeyFil, (DWORD)(LPSTR) szKeyStr)) {
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
    /* Free chop resources                                              */
    /********************************************************************/
    ChpSndTrm ();
    return (0);
}

/************************************************************************/
/*                  Command Line Dash Parameter Support                 */
/************************************************************************/
char *  DshPrmLod (WORD usArgCnt, char **ppcArgArr, CFGBLK *pcbCfgBlk, 
        CHPBLK *lpChpBlk)
{
    DWORD   ulSrcFrqOvr = 0L;              /* Sample frequency override    */    

    /********************************************************************/
    /* Initialize function communication block                          */
    /********************************************************************/
    ChpCfgIni (lpChpBlk);

    /********************************************************************/
    /* Get default parameters                                           */
    /********************************************************************/
    lpChpBlk->usSrcFmt = WAVHDRFMT;     /* Source file format           */
    lpChpBlk->usSrcPCM = MPCPCM016;     /* Source PCM type              */
    lpChpBlk->usSrcChn = 1;             /* Source file chan count       */
    lpChpBlk->usSrcBIO = PCMBIODEF;     /* Source bit encoding          */
    lpChpBlk->ulSrcFrq = 11025L;        /* Source sample frequency      */
    lpChpBlk->ulSrcOff = 0L;            /* Source byte offset           */
    lpChpBlk->ulSrcLen = 0xffffffffL;   /* Source byte length           */
    lpChpBlk->mhSrcCfg = (VISMEMHDL) 0; /* Source cfg memory buff       */       
    lpChpBlk->usDstFmt = WAVHDRFMT;     /* Destination file format      */
    lpChpBlk->flChpFrm = CHPFRMDEF;     /* FndSndEvt frame size   (sec) */
    lpChpBlk->flChpRes = CHPRESDEF;     /* FndSndEvt resolution   (sec) */
    lpChpBlk->flChpSnd = CHPSNDDEF;     /* FndSndEvt snd thresh  (%max) */
    lpChpBlk->usChpAtk = CHPATKDEF;     /* FndSndEvt atk ratio   (%frm) */
    lpChpBlk->usChpDcy = CHPDCYDEF;     /* FndSndEvt dcy ratio   (%frm) */
    lpChpBlk->flChpGrd = CHPGRDDEF;     /* FndSndEvt guard time   (sec) */
    lpChpBlk->mhGloCfg = (VISMEMHDL) 0; /* Global cfg memory buff       */       
    lpChpBlk->lpPolDsp = NULL;          /* Conversion poll display proc */
    lpChpBlk->ulRsvPol = 0L;            /* Reserved                     */
    lpChpBlk->lpMsgDsp = NULL;          /* Reserved                     */
    lpChpBlk->ulRsvErr = 0L;            /* Reserved                     */

    /********************************************************************/
    /* Get dash parameters                                              */
    /********************************************************************/
    while (usArgCnt--) {
        if (!strncmp (*ppcArgArr, "-debug", 6));    /* Handled in Ini   */
        else if (!strncmp (*ppcArgArr, "-exit", 5));/* Handled in Ini   */
        else if (!strncmp (*ppcArgArr, "-kf", 3));  /* Handled in Ini   */
        else if (!strncmp (*ppcArgArr, "-ks", 3));  /* Handled in Ini   */
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-co", 3)) pcbCfgBlk->lNamOff = atol (&(*ppcArgArr)[3]); 
        else if (FORALLVER && (!strncmp (*ppcArgArr, "-di", 3)) && !GetFmtPCM (&(*ppcArgArr)[3], &lpChpBlk->usSrcFmt, &lpChpBlk->usSrcPCM)); 
//        else if (FORALLVER && (!strncmp (*ppcArgArr, "-si", 3)) && !GetSwpFlg (&(*ppcArgArr)[3], &lpChpBlk->usSrcBIO));
        else if (FORALLVER && (!strncmp (*ppcArgArr, "-fi", 3)) && !GetSmpFrq (&(*ppcArgArr)[3], &ulSrcFrqOvr)); 
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-fl", 3)) {
            lpChpBlk->flChpFrm = (float) atof (&(*ppcArgArr)[3]);
            lpChpBlk->flChpFrm = lpChpBlk->flChpFrm ? lpChpBlk->flChpFrm : CHPFRMDEF;
        }
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-fr", 3)) {
            lpChpBlk->flChpRes = (float) atof (&(*ppcArgArr)[3]);
            lpChpBlk->flChpRes = lpChpBlk->flChpRes ? lpChpBlk->flChpRes : CHPRESDEF;
        }
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-ac", 3)) lpChpBlk->flChpGrd = (float) atof (&(*ppcArgArr)[3]);
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-st", 3)) lpChpBlk->flChpSnd = (float) atof (&(*ppcArgArr)[3]);
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-sa", 3)) lpChpBlk->usChpAtk = atoi (&(*ppcArgArr)[3]);
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-sd", 3)) lpChpBlk->usChpDcy = atoi (&(*ppcArgArr)[3]);
        else return (*ppcArgArr);
        /****************************************************************/
        /****************************************************************/
        *ppcArgArr++;
    }

    /********************************************************************/
    /* Set sample frequency based upon input data type & override       */
    /********************************************************************/
    lpChpBlk->ulSrcFrq = GetFrqOvr (lpChpBlk->usSrcPCM, ulSrcFrqOvr, lpChpBlk->ulSrcFrq);

    /********************************************************************/
    /* Force Output File format = Input format                          */
    /********************************************************************/
    lpChpBlk->usDstFmt = lpChpBlk->usSrcFmt;

    /********************************************************************/
    /********************************************************************/
    return (NULL);

}

WORD    DshPrmRel (CHPBLK FAR *lpChpBlk)
{
    return (0);
}

/************************************************************************/
/************************************************************************/
CFGMAP  cmPCMTypMap = {
        6, "VIS000WAV000MPC008WAV008MPC016VIS016WAV016DLG004RTX004DLG008G11F08G11I08G21004NWV001PTC001HAR001RTX003RTX004",
        UNKPCM000,
        UNKPCM000,
        MPCPCM008,
        MPCPCM008,
        MPCPCM016,
        MPCPCM016,
        MPCPCM016,
        DLGPCM004,
        DLGPCM004,
        DLGPCM008,
        G11PCMF08,
        G11PCMI08,
        G21PCM004,                      // Error if not available
        NWVPCM001,                      // Error if not available
        PTCPCM001,                      // Error if not available
        HARPCM001,                      // Error if not available
        RTXPCM003,                      // Error if not available
        RTXPCM004,                      // Error if not available
};
CFGMAP  cmFilFmtMap = {
        6, "VIS000VIS016MPC008MPC016WAV000WAV008WAV016DLG004DLG008G11F08G11I08G21004NWV001PTC001HAR001RTX003RTX004",
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
        ALLPURFMT,
        ALLPURFMT,
        ALLPURFMT,
        ALLPURFMT,
        ALLPURFMT,
        ALLPURFMT,
};

/************************************************************************/
/************************************************************************/
int GetFmtPCM (char *szTypNam, FILFMT *pusFilFmt, PCMTYP *pusPCMTyp)
{
    /********************************************************************/
    /********************************************************************/
    if (CfgPrmMap (&cmFilFmtMap, szTypNam, pusFilFmt)) return (-1);
    if (CfgPrmMap (&cmPCMTypMap, szTypNam, pusPCMTyp)) return (-1);

//Better solution than #if?
#if (!defined (G21)) /***************************************************/
    if (G21PCM004 == *pusPCMTyp) return (-1);
#endif /*****************************************************************/
#if (!defined (HAR)) /***************************************************/
    if (HARPCM001 == *pusPCMTyp) return (-1);
#endif /*****************************************************************/
#if (!defined (NWV)) /***************************************************/
    if (NWVPCM001 == *pusPCMTyp) return (-1);
#endif /*****************************************************************/
#if (!defined (PTC)) /***************************************************/
    if (PTCPCM001 == *pusPCMTyp) return (-1);
#endif /*****************************************************************/
#if (!defined (RTX)) /***************************************************/
    if (RTXPCM003 == *pusPCMTyp) return (-1);
    if (RTXPCM004 == *pusPCMTyp) return (-1);
#endif /*****************************************************************/

    return (0);

}

int GetSwpFlg (char *szTypNam, WORD *pusSwpFlg)
{
    WORD    usOrgFlg = *pusSwpFlg;

    /********************************************************************/
    /* Get and check bit/nibble/byte/word swap flags                    */
    /********************************************************************/
    *pusSwpFlg |= !_stricmp (szTypNam, "bit")    ? FIOENCSWP | FIOSWPBIT : FIOENCNON;  
    *pusSwpFlg |= !_stricmp (szTypNam, "nibble") ? FIOENCSWP | FIOSWPNIB : FIOENCNON;  
    *pusSwpFlg |= !_stricmp (szTypNam, "byte")   ? FIOENCSWP | FIOSWPBYT : FIOENCNON;  
    *pusSwpFlg |= !_stricmp (szTypNam, "word")   ? FIOENCSWP | FIOSWPWRD : FIOENCNON;  

    /********************************************************************/
    /* Return zero if successful (ie, flags have changed)               */
    /********************************************************************/
    return (usOrgFlg == *pusSwpFlg);
}

int GetSmpFrq (char *szSmpFrq, DWORD FAR *lpSmpFrq)
{
    /********************************************************************/
    /********************************************************************/
    *lpSmpFrq = (DWORD) (atof (szSmpFrq) * 1000.);
    if ((*lpSmpFrq < SRCFRQMIN) || (*lpSmpFrq > SRCFRQMAX)) return (-1);
    return (0);

}

DWORD   GetFrqOvr (PCMTYP usPCMTyp, DWORD ulOvrFrq, DWORD ulDefFrq)
{
    /********************************************************************/
    /* Note: Support services must be initialized for query functions   */
    /********************************************************************/
    if (ulOvrFrq) return (ulOvrFrq);
    if (ulOvrFrq = ChpCapQry (usPCMTyp, FRQDEFQRY, 0L)) return (ulOvrFrq);
    return (ulDefFrq);            
}
