/************************************************************************/
/* Conversion Utility Param Config: CvtCfg.c            V2.00  08/15/95 */
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
#include "cvtlib\gencvt.h"              /* PCM conversion defs          */
#include "cvtsup.h"                     /* PCM conversion supp          */

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
int     GetAAFTyp (char *, WORD, WORD *, WORD *);
BOOL    GetEquPrm (char *, CVTBLK *);

/************************************************************************/
/*                          Help Messages                               */
/************************************************************************/
char  * MsgUseIni =  
"\n\
       Voice Information Systems (1-800-234-VISI)  PCM Conversion Utility\n\
       v3.00a.1  (C) Copyright A.J. Michalik 1987-1996  U.S. Pat. Pending\n\n";



char  * MsgDspUse =  
"\
Usage: PCMCvt [-help] OldFile [NewFile] [-ac -b  -ci -di -do -fi -fo -li -ng\n\
                 -ni -no -pi -qa -qd -qe -ql -qr -q2 -q6 -si -so -va -vd -vn]\n";

char  * MsgHlp001 = "\
       OldFile: Source filename.\n\
       NewFile: Destination filename, if different from OldFile.\n\
       -acX.x:  Auto-Crop guard time, default 0.05 seconds\n\
       -b:      Backup file creation inhibited.\n\
       -ci:     Channel Input count, default is one.\n\
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
                MPC008 = Multimedia  8 bit unsigned (11.025 kHz).\n\
                MPC016 = Multimedia 16 bit linear   (11.025 kHz).\n\
                VIS016 = VIS Interchange Format 16 bit (11.025 kHz).\n\
                WAV008 = Wave Multimedia Format  8 bit (11.025 kHz).\n\
                WAV016 = Wave Multimedia Format 16 bit (11.025 kHz).\n"
#if (defined (DLG)) /****************************************************/
"\
       -doXXXX: Data Output, default Dialogic 4 bit (OKI) (6 kHz).\n"
#endif /*****************************************************************/
#if (defined (G21)) /****************************************************/
"\
       -doXXXX: Data Output, default C-ITU (CCITT) G.721 4 bit (8 kHz).\n"
#endif /*****************************************************************/
#if (defined (HAR)) /****************************************************/
"\
       -doXXXX: Data Output, default Harris Semiconductor 1 bit (32 kHz).\n"
#endif /*****************************************************************/
#if (defined (MPC)) /****************************************************/
"\
       -doXXXX: Data Output, default Wave Multimedia Format (11.025 kHz).\n"
#endif /*****************************************************************/
#if (defined (NWV)) /****************************************************/
"\
       -doXXXX: Data Output, default New Voice 1 bit (32 kHz).\n"
#endif /*****************************************************************/
#if (defined (PTC)) /****************************************************/
"\
       -doXXXX: Data Output, default Perception 1 bit (32 kHz).\n"
#endif /*****************************************************************/
#if (defined (RTX)) /****************************************************/
"\
       -doXXXX: Data Output, default Rhetorex 4 bit (8 kHz).\n"
#endif /*****************************************************************/
;
char  * MsgHlp002 = "\
       -fiX.x : Frequency of Input  override (X.x kHz).\n\
       -foX.x : Frequency of Output override (X.x kHz).\n\
       -liXXXX: Length of Input file, default all bytes.\n\
       -ngXXXX: Name of Global configuration file, default none.\n\
       -niXXXX: Name of Input  configuration file, default none.\n\
       -noXXXX: Name of Output configuration file, default none.\n\
       -piXXXX: Position of Input file beginning, default byte 0.\n\
       -qa:     Quality All: FFT DTMF, LP anti-alias & Resample filters on.\n\
       -qdXXXX: FFT DTMF filter off/on, default is off.\n\
       -qeXXXX: FFT Equalizer off/on/settings, default is off.\n\
       -qlXXXX: FFT Low pass anti-alias filter off/on, default is off.\n\
       -qrXXXX: FFT Resample filter off/on, default is off.\n\
       -q2XXXX: 2nd Order LP Anti-alias filter off/on/fast, default is on.\n\
       -q6XXXX: 6th Order LP Anti-alias filter off/on/fast, default is off.\n\
       -siXXXX: Swap Input  bit, nibble, byte or word; default is none.\n\
       -soXXXX: Swap Output bit, nibble, byte or word; default is none.\n\
       -vaX.x:  Volume Adjust (+/-), default is 0.0 dB.\n\
       -vdX.x:  Volume Dynamic compress/expand, X.x default is 6 dB.\n\
       -vnX.x:  Volume Normalize, X.x default is 0.2%% over range.\n\
Notes: Environment TMP= specifies temp work files directory.\n";

/************************************************************************/
/*                  Conversion Communication Blocks                     */
/************************************************************************/
CFGBLK  cbCfgBlk = {                    /* Configuration file block     */
    0,                                  /* File replacement flags       */
    0,                                  /* Input  config file name      */       
    0,                                  /* Output config file name      */       
    0,                                  /* Global config file name      */       
};
CVTBLK  cbCvtBlk;                       /* Function communication block */

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
    if (CVTVERNUM != CvtPCMVer()) {
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
        if (!strncmp (*ppcArgArr, "-debug", 6)) CvtSetDeb ((WORD) strtol (&(*ppcArgArr)[6], NULL, 16));
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
    switch (CvtPCMIni (0, (DWORD)(LPSTR) szKeyFil, (DWORD)(LPSTR) szKeyStr)) {
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
    /* Free conversion resources                                        */
    /********************************************************************/
    CvtPCMTrm ();
    return (0);
}

/************************************************************************/
/*                  Command Line Dash Parameter Support                 */
/************************************************************************/
char *  DshPrmLod (WORD usArgCnt, char **ppcArgArr, CFGBLK *pcbCfgBlk, 
        CVTBLK *lpCvtBlk)
{
    DWORD   ulSrcFrqOvr = 0L;           /* Sample frequency override    */    
    DWORD   ulDstFrqOvr = 0L;           /* Sample frequency override    */    

    /********************************************************************/
    /* Initialize function communication block                          */
    /********************************************************************/
    CvtCfgIni (&cbCvtBlk);

    /********************************************************************/
    /* Get default parameters                                           */
    /********************************************************************/
    lpCvtBlk->usSrcFmt = WAVHDRFMT;     /* Source file format           */
    lpCvtBlk->usSrcPCM = MPCPCM016;     /* Source PCM type              */
    lpCvtBlk->usSrcChn = 1;             /* Source file chan count       */
    lpCvtBlk->usSrcBIO = PCMBIODEF;     /* Source bit encoding          */
    lpCvtBlk->ulSrcFrq = 11025L;        /* Source sample frequency      */
    lpCvtBlk->ulSrcOff = 0L;            /* Source byte offset           */
    lpCvtBlk->ulSrcLen = 0xffffffffL;   /* Source byte length           */
    lpCvtBlk->mhSrcCfg = (VISMEMHDL) 0; /* Source cfg memory buff       */       
    lpCvtBlk->usDstFmt = ALLPURFMT;     /* Destination file format      */
    lpCvtBlk->usDstPCM = PCMTYPDEF;     /* Destination PCM type         */
    lpCvtBlk->usDstChn = CHNCNTDEF;     /* Destination file chan count  */
    lpCvtBlk->usDstBIO = PCMBIODEF;     /* Destination bit encoding     */
    lpCvtBlk->ulDstFrq = SMPFRQDEF;     /* Destination sample frequency */
    lpCvtBlk->ulDstOff = 0L;            /* Destination byte offset      */
    lpCvtBlk->ulDstLen = 0xffffffffL;   /* Destination byte length      */
    lpCvtBlk->mhDstCfg = (VISMEMHDL) 0; /* Destination cfg memory buff  */       
    lpCvtBlk->flCrpFrm = 0;             /* FndSndEvt frame size   (sec) */
    lpCvtBlk->flCrpRes = 0;             /* FndSndEvt resolution   (sec) */
    lpCvtBlk->flCrpSnd = 0;             /* FndSndEvt snd thresh  (%max) */
    lpCvtBlk->usCrpAtk = 0;             /* FndSndEvt atk ratio   (%frm) */
    lpCvtBlk->usCrpDcy = 0;             /* FndSndEvt dcy ratio   (%frm) */
    lpCvtBlk->flCrpGrd = CRPGRDDEF;     /* FndSndEvt guard time   (sec) */
    lpCvtBlk->flVolAdj = 0;             /* Volume adjust mult      (dB) */
    lpCvtBlk->usNrmLvl = 0;             /* Volume norm target level (%) */
    lpCvtBlk->flNrmExc = 0;             /* Volume norm except thrsh (%) */
    lpCvtBlk->usNrmMax = 0;             /* Volume norm max adjust  (dB) */
    lpCvtBlk->flDCELvl = 0;             /* DynCmpExp  level        (dB) */
    lpCvtBlk->usCmpThr = 0;             /* Compander  threshold     (%) */
    lpCvtBlk->usCmpAtk = 0;             /* Compander  attack time  (Hz) */
    lpCvtBlk->flCmpMin = 0;             /* Compander  minimum      (dB) */
    lpCvtBlk->usExpThr = 0;             /* Expander   threshold     (%) */
    lpCvtBlk->usExpAtk = 0;             /* Expander   attack time  (Hz) */
    lpCvtBlk->flExpMax = 0;             /* Expander   maximum      (dB) */
    lpCvtBlk->usCE_Dcy = 0;             /* Cmp / Exp  decay time   (Hz) */
    lpCvtBlk->usNoiThr = 0;             /* Noise Gate threshold     (%) */
    lpCvtBlk->flNoiAtt = 0;             /* Noise Gate attenuation  (dB) */
    lpCvtBlk->usTDLAAF = AAF_Q2TYP;     /* Time Domain Anti-alias (Ord) */
    lpCvtBlk->bfTDLFst = FALSE;         /* Anti-alias fast math on/off  */
    lpCvtBlk->bfFFTDTM = FALSE;         /* FFT DTMF filter   on/off     */
    lpCvtBlk->bfFFTAAF = FALSE;         /* FFT Anti-aliasing on/off     */
    lpCvtBlk->bfFFTFEq = FALSE;         /* FFT Freq Equalize on/off     */
    lpCvtBlk->bfFFTRes = FALSE;         /* FFT Freq Resample on/off     */
    lpCvtBlk->usFFTOrd = FFTORDDEF;     /* FFT filter size              */
    lpCvtBlk->flFEqGai = 0;             /* Freq equalizer gain adj (dB) */
    lpCvtBlk->ulFEqBot = FEQBOTDEF;     /* Freq equalizer bot freq (Hz) */
    lpCvtBlk->ulFEqTop = FEQTOPDEF;     /* Freq equalizer top freq (Hz) */
    lpCvtBlk->usFEqCnt = FEQCNTDEF;     /* Freq equalizer point count   */
    lpCvtBlk->flFEqPnt[0] = FEQVOX001;  /*  300- 800: +3dB (<300Hz:0dB) */
    lpCvtBlk->flFEqPnt[1] = FEQVOX002;  /*  800-1300: +6dB              */
    lpCvtBlk->flFEqPnt[2] = FEQVOX003;  /* 1300-1800: +6dB              */
    lpCvtBlk->flFEqPnt[3] = FEQVOX004;  /* 1800-2300: +6dB              */
    lpCvtBlk->flFEqPnt[4] = FEQVOX005;  /* 2300-2800: +6dB              */
    lpCvtBlk->flFEqPnt[5] = FEQVOX006;  /* 2800-3300: +6dB              */
    lpCvtBlk->flFEqPnt[6] = FEQVOX007;  /* 3300-3800: +3dB (>3.8Kz:0dB) */
    lpCvtBlk->mhGloCfg = (VISMEMHDL) 0; /* Global cfg memory buff       */       
    lpCvtBlk->lpPolDsp = NULL;          /* Conversion poll display proc */
    lpCvtBlk->ulRsvPol = 0L;            /* Reserved                     */
    lpCvtBlk->lpMsgDsp = NULL;          /* Reserved                     */
    lpCvtBlk->ulRsvErr = 0L;            /* Reserved                     */

    /********************************************************************/
    /* Get dash parameters                                              */
    /********************************************************************/
    while (usArgCnt--) {
        if (!strncmp (*ppcArgArr, "-debug", 6));    /* Handled in Ini   */
        else if (!strncmp (*ppcArgArr, "-exit", 5));/* Handled in Ini   */
        else if (!strncmp (*ppcArgArr, "-kf", 3));  /* Handled in Ini   */
        else if (!strncmp (*ppcArgArr, "-ks", 3));  /* Handled in Ini   */
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-ac", 3)) {
            lpCvtBlk->flCrpGrd = strlen(&(*ppcArgArr)[3]) ? (float) atof (&(*ppcArgArr)[3]) : CRPGRDDEF;
            lpCvtBlk->flCrpFrm = CRPFRMDEF;
        }
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-ci", 3)) lpCvtBlk->usSrcChn = (WORD) strtol (&(*ppcArgArr)[3], NULL, 10);  
        else if (FORALLVER && (!strncmp (*ppcArgArr, "-di", 3)) && !GetFmtPCM (&(*ppcArgArr)[3], &lpCvtBlk->usSrcFmt, &lpCvtBlk->usSrcPCM)); 
        else if (FORALLVER && (!strncmp (*ppcArgArr, "-do", 3)) && !GetFmtPCM (&(*ppcArgArr)[3], &lpCvtBlk->usDstFmt, &lpCvtBlk->usDstPCM)); 
        else if (FORALLVER && (!strncmp (*ppcArgArr, "-si", 3)) && !GetSwpFlg (&(*ppcArgArr)[3], &lpCvtBlk->usSrcBIO));
        else if (FORALLVER && (!strncmp (*ppcArgArr, "-so", 3)) && !GetSwpFlg (&(*ppcArgArr)[3], &lpCvtBlk->usDstBIO));
        else if (FORALLVER && (!strncmp (*ppcArgArr, "-fi", 3)) && !GetSmpFrq (&(*ppcArgArr)[3], &ulSrcFrqOvr)); 
        else if (FORALLVER && (!strncmp (*ppcArgArr, "-fo", 3)) && !GetSmpFrq (&(*ppcArgArr)[3], &ulDstFrqOvr)); 
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-li", 3)) lpCvtBlk->ulSrcLen = strtol (&(*ppcArgArr)[3], NULL, 10);  
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-pi", 3)) lpCvtBlk->ulSrcOff = strtol (&(*ppcArgArr)[3], NULL, 10);  
        else if (FORALLVER && (!strncmp (*ppcArgArr, "-ng", 3))) strncpy (pcbCfgBlk->szGloCfg, &(*ppcArgArr)[3], FIOMAXNAM); 
        else if (FORALLVER && (!strncmp (*ppcArgArr, "-ni", 3))) strncpy (pcbCfgBlk->szSrcCfg, &(*ppcArgArr)[3], FIOMAXNAM); 
        else if (FORALLVER && (!strncmp (*ppcArgArr, "-no", 3))) strncpy (pcbCfgBlk->szDstCfg, &(*ppcArgArr)[3], FIOMAXNAM); 
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-qd", 3)) lpCvtBlk->bfFFTDTM = (BOOL) _stricmp (&(*ppcArgArr)[3], "off");  
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-qe", 3)) lpCvtBlk->bfFFTFEq = GetEquPrm (&(*ppcArgArr)[3], lpCvtBlk);  
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-ql", 3)) lpCvtBlk->bfFFTAAF = (BOOL) _stricmp (&(*ppcArgArr)[3], "off");  
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-qr", 3)) lpCvtBlk->bfFFTRes = (BOOL) _stricmp (&(*ppcArgArr)[3], "off");  
        else if (FORALLVER && (!strncmp (*ppcArgArr, "-q2", 3)) && !GetAAFTyp (&(*ppcArgArr)[3], AAF_Q2TYP, &lpCvtBlk->usTDLAAF, &lpCvtBlk->bfTDLFst));
        else if (FORALLVER && (!strncmp (*ppcArgArr, "-q6", 3)) && !GetAAFTyp (&(*ppcArgArr)[3], AAF_Q6TYP, &lpCvtBlk->usTDLAAF, &lpCvtBlk->bfTDLFst));
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-qa", 3)) {
            lpCvtBlk->bfFFTDTM = TRUE; 
            lpCvtBlk->bfFFTAAF = TRUE;
            lpCvtBlk->bfFFTRes = TRUE; 
        }
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-va", 3)) lpCvtBlk->flVolAdj = (float) atof (&(*ppcArgArr)[3]); 
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-vd", 3)) {
            lpCvtBlk->flDCELvl = (float) atof (&(*ppcArgArr)[3]); 
            lpCvtBlk->flDCELvl = lpCvtBlk->flDCELvl ? lpCvtBlk->flDCELvl : DCELVLDEF;
        }
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-vn", 3)) {
            lpCvtBlk->flNrmExc = 
                (*ppcArgArr)[3] ? (float) atof (&(*ppcArgArr)[3]) : NRMEXCDEF;
            lpCvtBlk->usNrmLvl = NRMLVLDEF;
        }
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-b", 2)) pcbCfgBlk->usRepFlg |= BFRBKIFLG; 
        else return (*ppcArgArr);
        /****************************************************************/
        /****************************************************************/
        *ppcArgArr++;
    }

    /********************************************************************/
    /* Set sample frequency based upon input data type & override       */
    /********************************************************************/
    lpCvtBlk->ulSrcFrq = GetFrqOvr (lpCvtBlk->usSrcPCM, ulSrcFrqOvr, lpCvtBlk->ulSrcFrq);
    lpCvtBlk->ulDstFrq = GetFrqOvr (lpCvtBlk->usDstPCM, ulDstFrqOvr, lpCvtBlk->ulDstFrq);

    /********************************************************************/
    /* Get configuration file parameters                                */
    /********************************************************************/
    if (_fstrlen (pcbCfgBlk->szSrcCfg) && CvtCfgLod (pcbCfgBlk->szSrcCfg, 
        &lpCvtBlk->mhSrcCfg)) return (pcbCfgBlk->szSrcCfg);
    if (_fstrlen (pcbCfgBlk->szDstCfg) && CvtCfgLod (pcbCfgBlk->szDstCfg, 
        &lpCvtBlk->mhDstCfg)) return (pcbCfgBlk->szDstCfg);
    if (_fstrlen (pcbCfgBlk->szGloCfg) && CvtCfgLod (pcbCfgBlk->szGloCfg, 
        &lpCvtBlk->mhGloCfg)) return (pcbCfgBlk->szGloCfg);

    /********************************************************************/
    /********************************************************************/
    return (NULL);

}

WORD    DshPrmRel (CVTBLK FAR *lpCvtBlk)
{
    /********************************************************************/
    /********************************************************************/
    CvtCfgRel (&lpCvtBlk->mhSrcCfg);
    CvtCfgRel (&lpCvtBlk->mhDstCfg);
    CvtCfgRel (&lpCvtBlk->mhGloCfg);
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
//ajm -doWAV000 support
//if (UNKPCM000 == usPCMTyp) return (0L);
    if (ulOvrFrq = CvtCapQry (usPCMTyp, FRQDEFQRY, 0L)) return (ulOvrFrq);
    return (ulDefFrq);            
}

int GetAAFTyp (char *szTypNam, WORD usTDLOrd, WORD *pusTDLAAF, WORD *pbfTDLFst)
{
    /********************************************************************/
    /* Get Anti-aliasing filter type flags                              */
    /********************************************************************/
    if (!*szTypNam || !_stricmp (szTypNam, "on")) {
        *pusTDLAAF = usTDLOrd;
        return (0);    
    }
    if (!_stricmp (szTypNam, "off")) {
        *pusTDLAAF = 0;
        return (0);    
    }
    if (!_stricmp (szTypNam, "fast")) {
        *pusTDLAAF = usTDLOrd;
        *pbfTDLFst = TRUE;
        return (0);    
    }
    /********************************************************************/
    /* Invalid selection                                                */
    /********************************************************************/
    return (-1);
}

/************************************************************************/
/************************************************************************/
char *  EquTokGet (char *szArg, char *szStr)
{
    #define TBXARGDEL   ","
    WORD    usDebFlg = CvtSetDeb (0);

    /********************************************************************/
    /********************************************************************/
    szStr = strtok (szStr, TBXARGDEL);
    if ((usDebFlg & CFG___DEB) && szStr)
        printf ("%Fs = %Fs\n", (LPSTR) szArg, (LPSTR) szStr);

    /********************************************************************/
    /* Restore debug flag to original state after sestructive read.     */
    /********************************************************************/
    CvtSetDeb (usDebFlg);
    return (szStr);
}

BOOL    GetEquPrm (char *szEquStr, CVTBLK *lpCvtBlk)
{
    WORD    usi;

    /********************************************************************/
    /* Turning equalizer off?                                           */
    /********************************************************************/
    if (!strlen  (szEquStr)) return (TRUE);  
    if (!_stricmp (szEquStr, "off")) return (FALSE);  

    /********************************************************************/
    /* Scan for equalizer parameters.                                   */
    /********************************************************************/
    if (NULL == (szEquStr = EquTokGet ("GainAdjust", szEquStr))) return (FALSE);

    /********************************************************************/
    /* Get required parameters.                                         */
    /********************************************************************/
    lpCvtBlk->flFEqGai = (float) atof (szEquStr);
    if (NULL == (szEquStr = EquTokGet ("BottomFrequency", NULL))) return (FALSE);
    lpCvtBlk->ulFEqBot = max (0L, (DWORD) (atof (szEquStr) * 1000.));
    if (NULL == (szEquStr = EquTokGet ("TopFrequency", NULL))) return (FALSE);
    lpCvtBlk->ulFEqTop = min (SRCFRQMAX, (DWORD) (atof (szEquStr) * 1000.));

    /********************************************************************/
    /* Read equalizer band data.                                        */ 
    /* Note: Final (EOF) point value is ignored, usi is "count of".     */
    /********************************************************************/
    for (usi=0; usi < FEQMAXCNT; usi++) {
        if (NULL == (szEquStr = EquTokGet ("Band", NULL))) break;
        lpCvtBlk->flFEqPnt[usi] = (float) atof (szEquStr);
    }
    if (usi) lpCvtBlk->usFEqCnt = usi;

    /****************************************************************/
    /****************************************************************/
    return (TRUE);

}
