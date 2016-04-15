/************************************************************************/
/* Generic PCM Conversion Support: GenPCM.c             V2.00  07/15/95 */
/* Copyright (c) 1987-1995 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#include "genpcm.h"                     /* PCM/APCM conversion defs     */
#include "pcmsup.h"                     /* PCM/APCM xlate lib defs      */
#include "..\os_dev\winmsg.h"           /* User message support         */
#include "regdev\pcmreg.h"              /* User Registration defs       */
                                        
#include <string.h>                     /* String manipulation funcs    */
#include <stdlib.h>                     /* Misc string/math/error funcs */

/************************************************************************/
/*                      Global variables                                */
/************************************************************************/
PCMGLO PCMGlo = {                       /* PCM Cvt Library Globals      */
    "PCM Conversion",                   /* Library name                 */
    {'s','r','0','0','z','h','a','0'},  /* Operating/Demo Only Key      */
    {'1','0','0','0'},                  /* Op/Demo sequence number      */
    {'0','0','0','0'},                  /* Security check sum           */
    REGDEMKEY,                          /* usRegKey                     */
    FALSE,                              /* Debug errors flag            */
    NULL,                               /* Global instance handle       */
};

/************************************************************************/
/************************************************************************/
int FAR PASCAL LibMain (HANDLE hLibIns, WORD usDatSeg, WORD usHeapSz,
               LPSTR lpCmdLin)
{

    /********************************************************************/
    /********************************************************************/
    if (usHeapSz != 0) UnlockData(0);
    PCMGlo.hLibIns = hLibIns;
    PCMResIni (hLibIns);

    /********************************************************************/
    /********************************************************************/
    return (1);

}

/************************************************************************/
/************************************************************************/
int FAR PASCAL WEP (int nParam)
{

    return (1);

}

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL PCMDLLIni (WORD usReqTyp, DWORD ulPrm001, DWORD ulPrm002) 
{
    WORD    usCmpCod;
    
    /********************************************************************/
    /* If usReqTyp == 0, check for accelerator key                      */
    /********************************************************************/
    if (!usReqTyp && !ulPrm002) {
        /****************************************************************/
        /* Check for accelerator key and/or AppKey/UsrKey combo         */
        /****************************************************************/
        if (REGDEMKEY == ChkRegKey (PCMGlo.RelKeyArr, PCMGlo.SeqNumArr, &usCmpCod)) {
            MsgDspUsr ("Error: Accelerator Key not installed!");
            return ((WORD) -1);
        } 
        /****************************************************************/
        /****************************************************************/
        PCMGlo.usRegKey = GetEncKey (PCMGlo.SeqNumArr, 1, &usCmpCod);
    }
    if (!usReqTyp && ulPrm002) {
        PCMGlo.usRegKey = ChkRegKey ((LPSTR) ulPrm002, PCMGlo.SeqNumArr, &usCmpCod);
    }

    /********************************************************************/
    /********************************************************************/
    return (0);

}

WORD FAR PASCAL PCMSupVer ()
{
    /********************************************************************/
    /********************************************************************/
    return (PCMVERNUM);
}

/************************************************************************/
/************************************************************************/
WORD    PASCAL FAR  PCMSetDeb (WORD usDebFlg)
{
    WORD    usDebOrg = PCMGlo.usDebFlg;
    PCMGlo.usDebFlg = usDebFlg;
    return (usDebOrg);
}

/************************************************************************/
/************************************************************************/
PCMTYP  FAR PASCAL  PCMTypEnu (PCMTYP usPCMTyp)
{
#if ((defined (DLG) || defined (RTX) || defined (_P_DLG))) /*************/
    if (usPCMTyp < DLGPCM004) return (DLGPCM004);
    if (usPCMTyp < DLGPCM008) return (DLGPCM008);
#endif /*****************************************************************/

#if ((defined (G11) || defined (_P_G11))) /******************************/
    if (usPCMTyp < G11PCMF08) return (G11PCMF08);
    if (usPCMTyp < G11PCMI08) return (G11PCMI08);
#endif /*****************************************************************/

#if ((defined (G21) || defined (_P_G21))) /******************************/
    if (usPCMTyp < G21PCM004) return (G21PCM004);
    if (usPCMTyp < G23PCM003) return (G23PCM003);
#endif /*****************************************************************/

#if ((defined (MSW) || defined (_P_MSW))) /******************************/
    if (usPCMTyp < MPCPCM008) return (MPCPCM008);
    if (usPCMTyp < MPCPCM016) return (MPCPCM016);
    if (usPCMTyp < MSAPCM004) return (MSAPCM004);
#endif /*****************************************************************/

#if ((defined (NWV) || defined (PTC) || defined (_P_NWV))) /*************/
    if (usPCMTyp < NWVPCM001) return (NWVPCM001);
#endif /*****************************************************************/

#if ((defined (PTC) || defined (_P_PTC))) /******************************/
    if (usPCMTyp < PTCPCM001) return (PTCPCM001);
#endif /*****************************************************************/

#if ((defined (RTX) || defined (_P_RTX))) /******************************/
    if (usPCMTyp < RTXPCM004) return (RTXPCM004);
#endif /*****************************************************************/

    /********************************************************************/
    /********************************************************************/
    return (0);
}

WORD    FAR PASCAL  PCMChnEnu (PCMTYP usPCMTyp, WORD usChnCnt)
{
    switch (usPCMTyp) {
        case DLGPCM004:
        case DLGPCM008:
            if (usChnCnt < 1) return (1);
            break;
        case NWVPCM001:
            if (usChnCnt < 1) return (1);
            break;
        case MPCPCM008:
        case MPCPCM016:
        case MSAPCM004:
            if (usChnCnt < 1) return (1);
            if (usChnCnt < 2) return (2);
            break;
        case PTCPCM001:
            if (usChnCnt < 1) return (1);
            break;
        case RTXPCM004:
            if (usChnCnt < 1) return (1);
            break;
    }

    /********************************************************************/
    /********************************************************************/
    return (0);
}

DWORD   FAR PASCAL  PCMFrqEnu (PCMTYP usPCMTyp, DWORD ulSmpFrq)
{
    switch (usPCMTyp) {
        case DLGPCM004:
        case DLGPCM008:
            if (ulSmpFrq < 6000L) return (6000L);
            if (ulSmpFrq < 6053L) return (6053L);
            if (ulSmpFrq < 8000L) return (8000L);
            break;
        case G11PCMF08:
        case G11PCMI08:
            if (ulSmpFrq < 8000L) return (8000L);
            break;
        case MPCPCM008:
        case MPCPCM016:
        case MSAPCM004:
            if (ulSmpFrq < 11025L) return (11025L);
            if (ulSmpFrq < 22050L) return (22050L);
            if (ulSmpFrq < 44100L) return (44100L);
            break;
        case NWVPCM001:
            if (ulSmpFrq < 16000L) return (16000L);
            if (ulSmpFrq < 24000L) return (24000L);
            if (ulSmpFrq < 32000L) return (32000L);
            break;
        case PTCPCM001:
            if (ulSmpFrq < 32000L) return (32000L);
            break;
        case RTXPCM004:
            if (ulSmpFrq < 6000L) return (6000L);
            if (ulSmpFrq < 8000L) return (8000L);
            break;
    }

    /********************************************************************/
    /********************************************************************/
    return (0);
}

/************************************************************************/
/************************************************************************/
LPSTR   lstrncpy (lpDstStr, lpSrcBlk, usBytCnt)
LPSTR   lpDstStr;
LPSTR   lpSrcBlk;
WORD    usBytCnt;
{
    LPSTR   lpDstOrg = lpDstStr;

    /********************************************************************/
    /********************************************************************/
    while (usBytCnt-- > 0) if (NULL == (*lpDstStr++ = *lpSrcBlk++)) break;
    return (lpDstOrg);

}
LPSTR FAR PASCAL PCMDesQry (PCMTYP usPCMTyp, LPSTR szMsgTxt, WORD usMaxLen)
{
    /********************************************************************/
    /********************************************************************/
    if (!usMaxLen) return (szMsgTxt);
    *szMsgTxt = '\0';

    /********************************************************************/
    /********************************************************************/
    switch (usPCMTyp) {
        case DLGPCM004:
            MsgLodStr (PCMGlo.hLibIns, SI_DLGPCM004, szMsgTxt, usMaxLen);
            break;
        case DLGPCM008:
            MsgLodStr (PCMGlo.hLibIns, SI_DLGPCM008, szMsgTxt, usMaxLen);
            break;
        case G11PCMF08:
            MsgLodStr (PCMGlo.hLibIns, SI_G11PCMF08, szMsgTxt, usMaxLen);
            break;
        case G11PCMI08:
            MsgLodStr (PCMGlo.hLibIns, SI_G11PCMI08, szMsgTxt, usMaxLen);
            break;
        case MPCPCM008:
            MsgLodStr (PCMGlo.hLibIns, SI_MPCPCM008, szMsgTxt, usMaxLen);
            break;
        case MPCPCM016:
            MsgLodStr (PCMGlo.hLibIns, SI_MPCPCM016, szMsgTxt, usMaxLen);
            break;
        case MSAPCM004:
            MsgLodStr (PCMGlo.hLibIns, SI_MSAPCM004, szMsgTxt, usMaxLen);
            break;
        case NWVPCM001:
            MsgLodStr (PCMGlo.hLibIns, SI_NWVPCM001, szMsgTxt, usMaxLen);
            break;
        case PTCPCM001:
            MsgLodStr (PCMGlo.hLibIns, SI_PTCPCM001, szMsgTxt, usMaxLen);
            break;
        case RTXPCM004:
            MsgLodStr (PCMGlo.hLibIns, SI_RTXPCM004, szMsgTxt, usMaxLen);
            break;
        default:
            MsgLodStr (PCMGlo.hLibIns, SI_UNKPCM000, szMsgTxt, usMaxLen);
    }
    return (szMsgTxt);

}    

/************************************************************************/
/*                      Accelerator key support                         */
/************************************************************************/
#define TBxGlo          PCMGlo
#define TBXMAXSTR       PCMMAXSTR
#include "regdev\tbxreg.c"               /* User Registration code   */

#define _hInst          PCMGlo.hLibIns    
#include "regdev\kescb.c"                /* Registration key code    */
