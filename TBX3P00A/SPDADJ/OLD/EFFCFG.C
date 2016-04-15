/************************************************************************/
/* Frequency Effects Param Config: EffCfg.c             V2.00  11/10/93 */
/* Copyright (c) 1987-1995 Andrew J. Michalik                           */
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
#include "geneff.h"                     /* Frequency effects support    */
#include "libsup.h"                     /* Frequency effects lib supp   */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */
#include <math.h>                       /* Math library defs            */

/************************************************************************/
/*                          Forward References                          */
/************************************************************************/
int     GetFmtPCM (char *, FILFMT *, PCMTYP *);
int     GetSmpFrq (char *, DWORD *);
DWORD   GetFrqOvr (PCMTYP, DWORD, DWORD);

/************************************************************************/
/*                          Help Messages                               */
/************************************************************************/
char  * MsgUseIni =  
"\n\
    Voice Information Systems (1-800-234-VISI) Frequency Effects Utility\n\
     v3.00x.0 (C)  Copyright A.J. Michalik 1987-1995  U.S. Pat. Pending\n\n";

char  * MsgDspUse =  
"\
Usage: FrqEff [-help] OldFile [NewFile] [-b -di -fi -ng -ps -sm -va]\n";

char  * MsgDspHlp = "\
       OldFile: Source filename.\n\
       NewFile: Destination filename, if different from OldFile.\n\
       -b:      Backup file creation inhibited.\n\
       -diXXXX: Data Input, default Wave Multimedia Format.\n\
                DLG004 = Dialogic 4 bit (OKI)   (6 kHz).\n\
                DLG008 = Dialogic 8 bit (u-Law) (6 kHz).\n\
                G11F08 = CCITT G.711 (Folded)   u-Law 8 bit (8 kHz).\n\
                G11I08 = CCITT G.711 (Inverted) a-Law 8 bit (8 kHz).\n\
                VIS016 = VIS Interchange Format 16 bit (11.025 kHz).\n\
                WAV008 = Wave Multimedia Format  8 bit (11.025 kHz).\n\
                WAV016 = Wave Multimedia Format 16 bit (11.025 kHz).\n\
       -fiX.x : Frequency of Input  override (X.x kHz).\n\
       -lfXXXX: Length of FFT input buffer, default 512 points.\n\
       -lwXXXX: Length of FFT data window, default 512 points.\n\
       -smX.x:  Speed time multiplier, default is 1.\n\
       -va:     Volume Adjust, default is 0 dB.\n\
Notes: Environment TMP= specifies temp work files directory.\n\
";

/************************************************************************/
/************************************************************************/
CFGBLK  cbCfgBlk = {                    /* Configuration file block     */
    0,                                  /* File replacement flags       */
    0,                                  /* Global config file name      */       
};
FRQBLK  fbFrqBlk = {                    /* Function communication block */
    WAVHDRFMT,                          /* Source file format           */
    MPCPCM016,                          /* Source PCM type              */
    1,                                  /* Source file chan count       */
    PCMBIODEF,                          /* Source bit encoding          */
    11025L,                             /* Source sample frequency      */
    0L,                                 /* Source byte offset           */
    0xffffffffL,                        /* Source byte length           */
    (VISMEMHDL) 0,                      /* Source cfg memory buff       */       
    WAVHDRFMT,                          /* Destination file format      */
    MPCPCM016,                          /* Destination PCM type         */
    1,                                  /* Destination file chan count  */
    PCMBIODEF,                          /* Destination bit encoding     */
    11025L,                             /* Destination sample frequency */
    0L,                                 /* Destination byte offset      */
    0xffffffffL,                        /* Destination byte length      */
    (VISMEMHDL) 0,                      /* Destination cfg memory buff  */ 
    0,                                  /* Volume adjust multiplier     */
    FFTORDDEF,                          /* FFT filter size              */
    WINORDDEF,                          /* FFT window size              */
    SPDMULDEF,                          /* Speed multiplier default     */
    PCHMULDEF,                          /* Pitch multiplier default     */
    SYNTHRDEF,                          /* Vocoder synthesis threshold  */
    FALSE,                              /* Force Osc Bank Syn flag      */
    (VISMEMHDL) 0,                      /* Global cfg memory buff       */       
    0L,                                 /* Total bytes processed count  */
};

/************************************************************************/
/*                  Command Line Dash Parameter Support                 */
/************************************************************************/
char *  GetDshPrm (WORD usArgCnt, char **ppcArgArr, CFGBLK *pcbCfgBlk, 
        FRQBLK *lpFrqBlk)
{
    DWORD   ulSrcFrqOvr = 0L;              /* Sample frequency override    */    

    /********************************************************************/
    /* Get dash parameters                                              */
    /* Note: ceil (LOGBASTWO (x)) may actually incr due to rounding err */
    /********************************************************************/
    while (0 < usArgCnt--) {
        if (!strncmp (*ppcArgArr, "-debug", 6)) EffSetDeb ((WORD) strtol (&(*ppcArgArr)[6], NULL, 16));
        else if (FORALLVER && (!strncmp (*ppcArgArr, "-di", 3)) && !GetFmtPCM (&(*ppcArgArr)[3], &lpFrqBlk->usSrcFmt, &lpFrqBlk->usSrcPCM)); 
        else if (FORALLVER && (!strncmp (*ppcArgArr, "-fi", 3)) && !GetSmpFrq (&(*ppcArgArr)[3], &ulSrcFrqOvr)); 
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-lf", 3)) {
            lpFrqBlk->usFFTOrd = (WORD) strtol (&(*ppcArgArr)[3], NULL, 10);
            lpFrqBlk->usFFTOrd = lpFrqBlk->usFFTOrd ? (WORD) LOGBASTWO (lpFrqBlk->usFFTOrd) : FFTORDDEF;
        }
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-lw", 3)) {
            lpFrqBlk->usWinOrd = (WORD) strtol (&(*ppcArgArr)[3], NULL, 10);
            lpFrqBlk->usWinOrd = lpFrqBlk->usWinOrd ? (WORD) LOGBASTWO (lpFrqBlk->usWinOrd) : WINORDDEF;
        }
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-sm", 3)) lpFrqBlk->flSpdMul = (float) atof (&(*ppcArgArr)[3]); 
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-va", 3)) lpFrqBlk->flVolAdj = (float) atof (&(*ppcArgArr)[3]); 
        else if (FORALLVER &&  !strncmp (*ppcArgArr, "-b", 2)) pcbCfgBlk->usRepFlg |= BFRBKIFLG; 
        else return (*ppcArgArr);
        /****************************************************************/
        /****************************************************************/
        *ppcArgArr++;
    }

    /********************************************************************/
    /* Set sample frequency based upon input data type & override       */
    /********************************************************************/
    lpFrqBlk->ulSrcFrq = GetFrqOvr (lpFrqBlk->usSrcPCM, ulSrcFrqOvr, lpFrqBlk->ulSrcFrq);

    /********************************************************************/
    /********************************************************************/
    return (NULL);

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
    if (ulOvrFrq = PCMCapQry (usPCMTyp, FRQDEFQRY, 0L)) return (ulOvrFrq);
    return (ulDefFrq);            
}

