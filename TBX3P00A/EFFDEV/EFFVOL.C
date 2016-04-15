/************************************************************************/
/* Effects Volume Level Support: EffVol.c               V2.00  03/15/95 */
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
#include "..\pcmdev\genpcm.h"           /* PCM/APCM conv routine defs   */
#include "..\fiodev\filutl.h"           /* File utilities definitions   */
#include "geneff.h"                     /* Sound Effects definitions    */
#include "effsup.h"                     /* Sound Effects definitions    */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation          */
#include <stdio.h>                      /* Standard I/O                 */
#include <math.h>                       /* Math library defs            */

/************************************************************************/
/************************************************************************/
float   CmpNrmAtt (DWORD FAR *, DWORD, DWORD, float, float, float, float);
DWORD   DynCmpExp (LPGSMP, DWORD, LPDCEB, DWORD);

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL EffNrmLvl (short sFilHdl, DWORD ulBytRem, float flNoiLvl, 
                float flNoiExc, float flTrgLvl, float flTrgExc, float flAdjMax, 
                PCMTYP usPCMTyp, WORD usChnCnt, WORD usBIOEnc, DWORD ulSmpFrq, 
                float far *pflNrmAtt, LPITCB lpITCBlk, EFFPOLPRC fpPolPrc, 
                DWORD ulPolDat, EBSFRDPRC fpPCMRd_G16)
{
    #define     NRMENVLEN   1000
    VISMEMHDL   mhWrkHdl;
    VISMEMHDL   mhEnvHdl;
    LPGSMP      lpWrkBuf;
    DWORD  FAR *lpEnvBuf;
    DWORD       ulCurSmp;
    DWORD       ulAbsWav;
    DWORD       ulFilByt;
    DWORD       ulAtDMax = (DWORD) pow (2, EBSAtDRes (usPCMTyp)) / 2;
    DWORD       ulFilPos = FilGetPos (sFilHdl);
    DWORD       ulBytTot = ulBytRem;
    ITCBLK      ibITCBlk = *lpITCBlk;
    DWORD       ulSmpCnt = 0L;
    WORD        usRetCod = 0;
    DWORD       uli;

    /********************************************************************/
    /* Initialize attenuation level to zero                             */
    /********************************************************************/
    *pflNrmAtt = 0;

    /********************************************************************/
    /* Allocate file I/O and envelope memory buffers                    */
    /********************************************************************/
    if (NULL == (lpWrkBuf = GloAloLck (GMEM_MOVEABLE, &mhWrkHdl, 
      EBSCNTDEF * sizeof (GENSMP)))) {
        return ((WORD) -1);
    }
    if (NULL == (lpEnvBuf = GloAloLck (GMEM_MOVEABLE, &mhEnvHdl, 
      NRMENVLEN * sizeof (DWORD)))) {
        GloUnLRel (mhWrkHdl);
        return ((WORD) -1);
    }
    _fmemset (lpEnvBuf, 0, NRMENVLEN * sizeof (DWORD));

    /********************************************************************/
    /* Show/Hide Progress indicator for time consuming operations       */
    /********************************************************************/
    if (fpPolPrc) fpPolPrc (EFFPOLBEG, ulBytRem, ulPolDat);

    /********************************************************************/
    /* Scan file; build envelope histogram                              */
    /********************************************************************/
    while (TRUE) {
        ulCurSmp = fpPCMRd_G16 (sFilHdl, min (ulBytRem, BYTMAXFIO), lpWrkBuf, 
            EBSCNTDEF, usPCMTyp, usChnCnt, usBIOEnc, &ibITCBlk, &ulFilByt,
            NULL, NULL, 0L);
        if (!ulCurSmp || (-1L == ulCurSmp)) {
            usRetCod = (WORD) ulCurSmp;
            break;
        }

        /****************************************************************/
        /****************************************************************/
        if (fpPolPrc && !fpPolPrc (EFFPOLCNT, ulBytTot - ulBytRem, ulPolDat)) {
            usRetCod = (WORD) -1;
            break;
        }

        /****************************************************************/
        /* Increment each envelope bin based upon signal level          */
        /****************************************************************/
        for (uli=0L; uli < ulCurSmp; uli++) {
            ulAbsWav = (lpWrkBuf[uli] < 0) ? -lpWrkBuf[uli] : lpWrkBuf[uli];
            if (ulAbsWav > ulAtDMax) ulAbsWav = ulAtDMax;
            lpEnvBuf[(NRMENVLEN - 1L) * ulAbsWav / ulAtDMax]++;
        }
        ulBytRem -= ulFilByt;
        ulSmpCnt += ulCurSmp;

    }
    if (fpPolPrc) fpPolPrc (EFFPOLEND, ulBytTot, ulPolDat);

    /********************************************************************/
    /* Compute normalization attenuation in dB; check limits            */
    /********************************************************************/
    *pflNrmAtt = CmpNrmAtt (lpEnvBuf, NRMENVLEN, ulSmpCnt, flNoiLvl, flNoiExc,
        flTrgLvl, flTrgExc);
    *pflNrmAtt = min (*pflNrmAtt, +flAdjMax);
    *pflNrmAtt = max (*pflNrmAtt, -flAdjMax);
    
    /********************************************************************/
    /********************************************************************/
    GloUnLRel (mhWrkHdl);
    GloUnLRel (mhEnvHdl);
    FilSetPos (sFilHdl, ulFilPos, SEEK_SET);

    /********************************************************************/
    /********************************************************************/
    return (usRetCod);

}

float   CmpNrmAtt (DWORD FAR *lpEnvBuf, DWORD ulEnvLen, DWORD ulSmpCnt, 
        float flNoiLvl, float flNoiExc, float flTrgLvl, float flTrgExc)
{
    DWORD   ulSigIdx = max (min ((DWORD) (flNoiLvl * ulEnvLen), ulEnvLen), 0L);
    DWORD   ulLowSmp = 0L;
    DWORD   uli;

    /********************************************************************/
    /* Check for non-zero sample count                                  */
    /********************************************************************/
    if (!ulSmpCnt) return (0);

    /********************************************************************/
    /* Scan envelope bins; sum to find if all are below noise threshold */
    /********************************************************************/
    for (uli=0L; uli < ulSigIdx; uli++) ulLowSmp += lpEnvBuf[uli];

    /********************************************************************/
    /* Check if most (or all) of signal is at or below noise level      */
    /********************************************************************/
    if ((ulLowSmp / (float) ulSmpCnt) >= (1 - flNoiExc)) return (0);

    /********************************************************************/
    /* Scan envelope bins; find bin index which exceeds norm threshold  */
    /* Calculate normalization multiplier based upon "previous" uli+1   */
    /* Note: "previous" uli+1 = uli; uli is always less than ulEnvLen   */
    /********************************************************************/
    for (uli=ulSigIdx; uli < ulEnvLen; uli++) {
        if (((ulLowSmp + lpEnvBuf[uli]) / (float) ulSmpCnt) >= (1 - flTrgExc)) 
            return ((float) VLTTO_DB_ ((flTrgLvl * ulEnvLen) / uli));
        else ulLowSmp += lpEnvBuf[uli];
    }

    /********************************************************************/
    /********************************************************************/
    return (0);

}

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL EffVolAdj (LPEBSB lpSrcEBS, LPEBSB lpDstEBS, float flVolAdj)
{
    LPGSMP  lpSrcBuf;
    LNGFRA  lfWavMax;
    LNGFRA  lfVolAdj;
    DWORD   ulDstRem;
    DWORD   ulInpSmp;
    DWORD   uli;

    /********************************************************************/
    /********************************************************************/
    if (!lpSrcEBS->mhBufHdl || !lpSrcEBS->ulSmpCnt) 
        return (EffEBStoE (lpSrcEBS, lpDstEBS, 0L));
    if (!flVolAdj) 
        return (EffEBStoE (lpSrcEBS, lpDstEBS, lpSrcEBS->ulSmpCnt));

    /********************************************************************/
    /* Check / Allocate destination buffer, init characteristics        */
    /********************************************************************/
    if (!(ulDstRem = EBSAloReq (lpDstEBS, EBSPCMDEF, lpSrcEBS->usAtDRes, 
      lpSrcEBS->usChnCnt, lpSrcEBS->ulSmpFrq, lpSrcEBS->ulSmpCnt))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }

    /********************************************************************/
    /********************************************************************/
    if (NULL == (lpSrcBuf = GloMemLck (lpSrcEBS->mhBufHdl))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }

    /********************************************************************/
    /* Adjust volume level                                              */
    /* Note: Supported PCM values are one less than A to D resolution   */
    /********************************************************************/
    lfWavMax = (LNGFRA) (LNGFRAONE * ATDTO_MXM (lpSrcEBS->usAtDRes));
    lfVolAdj = (LNGFRA) (LNGFRAONE * DB_TO_VLT (flVolAdj));
    uli = ulInpSmp = min (lpSrcEBS->ulSmpCnt, ulDstRem);
    while (uli--) {
        LNGFRA  lfCurWav = *lpSrcBuf * lfVolAdj;
        *lpSrcBuf++ = (GENSMP) (CLPATDGEN (lfCurWav, lfWavMax) >> LNGFRANRM);
    }
    GloMemUnL (lpSrcEBS->mhBufHdl);

    /********************************************************************/
    /********************************************************************/
    return (EffEBStoE (lpSrcEBS, lpDstEBS, ulInpSmp));

}

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL EffDCEAlo (WORD usPCMTyp, WORD usChnCnt, DWORD ulSmpFrq, 
                float flCmpThr, DWORD ulCmpAtk, float flCmpMin,
                float flExpThr, DWORD ulExpAtk, float flExpMax,
                DWORD ulCE_Dcy, float flNoiThr, float flNoiAtt, LPDCEB lpDCEBlk)
{
    /********************************************************************/
    /********************************************************************/
    _fmemset (lpDCEBlk, 0, sizeof (*lpDCEBlk));
    lpDCEBlk->ulAtDMax = (DWORD) pow (2, EBSAtDRes (usPCMTyp)) / 2;
    lpDCEBlk->ulCmpThr = (DWORD) (flCmpThr * lpDCEBlk->ulAtDMax);
    lpDCEBlk->flCmpMin = (float) DB_TO_VLT (-fabs (flCmpMin));
    lpDCEBlk->flCmpAtk = (float) FRQTOFCON (ulCmpAtk, ulSmpFrq);
    lpDCEBlk->flCmpLvl = 1;
    lpDCEBlk->ulExpThr = (DWORD) (flExpThr * lpDCEBlk->ulAtDMax);
    lpDCEBlk->flExpMax = (float) DB_TO_VLT (+fabs (flExpMax));
    lpDCEBlk->flExpAtk = (float) FRQTOFCON (ulExpAtk, ulSmpFrq);
    lpDCEBlk->flExpLvl = 1;
    lpDCEBlk->flCE_Dcy = (float) FRQTOFCON (ulCE_Dcy, ulSmpFrq);
    lpDCEBlk->ulNoiThr = (DWORD) (flNoiThr * lpDCEBlk->ulAtDMax);
    lpDCEBlk->flNoiMul = (float) DB_TO_VLT (-fabs (flNoiAtt));
//    lpDCEBlk->ulEnvNrm = (DWORD) (lpDCEBlk->ulAtDMax / max (fabs (flCmpMin), fabs (flExpMax)));

    /********************************************************************/
    /********************************************************************/
    return (0);

}
WORD FAR PASCAL EffDCERel (LPDCEB lpDCEBlk)
{
    return (0);
}
DWORD FAR PASCAL EffVolDCE (LPEBSB lpSrcEBS, LPEBSB lpDstEBS, LPDCEB lpDCEBlk)
{
    LPGSMP  lpSrcBuf;
    DWORD   ulDstRem;
    DWORD   ulInpSmp;

    /********************************************************************/
    /********************************************************************/
    if (!lpSrcEBS->mhBufHdl || !lpSrcEBS->ulSmpCnt) 
        return (EffEBStoE (lpSrcEBS, lpDstEBS, 0L));
    if (!lpDCEBlk->flCmpMin && !lpDCEBlk->flExpMax) 
        return (EffEBStoE (lpSrcEBS, lpDstEBS, lpSrcEBS->ulSmpCnt));

    /********************************************************************/
    /********************************************************************/
    if (!(ulDstRem = EBSAloReq (lpDstEBS, EBSPCMDEF, lpSrcEBS->usAtDRes,
      lpSrcEBS->usChnCnt, lpSrcEBS->ulSmpFrq, lpSrcEBS->ulSmpCnt))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }

    /********************************************************************/
    /********************************************************************/
    if (NULL == (lpSrcBuf = GloMemLck (lpSrcEBS->mhBufHdl))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }

    /********************************************************************/
    /* Dynamic compress / expand                                        */
    /********************************************************************/
    ulInpSmp = DynCmpExp (lpSrcBuf, min (lpSrcEBS->ulSmpCnt, ulDstRem),
        lpDCEBlk, lpDCEBlk->ulEnvNrm);

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (lpSrcEBS->mhBufHdl);

    /********************************************************************/
    /********************************************************************/
    return (EffEBStoE (lpSrcEBS, lpDstEBS, ulInpSmp));

}
DWORD   DynCmpExp (LPGSMP lpSrcBuf, DWORD ulSmpCnt, LPDCEB lpDCEBlk, DWORD ulEnvNrm)
{
    long    lCurWav;
    DWORD   ulAbsWav;
    DCEBLK  dce = *lpDCEBlk;
    DWORD   uli = ulSmpCnt;

    /********************************************************************/
    /********************************************************************/
    while (uli--) {
        lCurWav = (long) (*lpSrcBuf * dce.flExpLvl);
        ulAbsWav = lCurWav < 0 ? -lCurWav : lCurWav;
        if (ulAbsWav > dce.ulCmpThr) {
            /************************************************************/
            /* Signal is in compression region; reduce level            */
            /* Note: Supported PCM values are one less than A/D res     */
            /* Note: ulAbsWav > 0                                       */
            /* Note: Comparison to ulAtDMax corrects for over range     */
            /************************************************************/
            dce.flExpLvl -= (dce.flExpLvl - max (dce.flCmpMin,
                dce.ulExpThr / (float) ulAbsWav)) * dce.flCmpAtk;
            lCurWav  = (long) (*lpSrcBuf * dce.flExpLvl);
            if ((lCurWav > -(long) dce.ulAtDMax) && (lCurWav < (long) dce.ulAtDMax))
                *lpSrcBuf = (GENSMP) lCurWav;
            else
                *lpSrcBuf = (GENSMP) (lCurWav < 0 ? -(long) (dce.ulAtDMax-1L) : (dce.ulAtDMax-1L));
            if (ulEnvNrm) *lpSrcBuf = (GENSMP) (ulEnvNrm * VLTTO_DB_ (dce.flExpLvl));
        }
        else if ((ulAbsWav < dce.ulExpThr) && (ulAbsWav > dce.ulNoiThr)) {
            /************************************************************/
            /* Signal is in expansion region; increase level            */
            /* Note: Over range is avoided since signal is at low level */
            /************************************************************/
            dce.flExpLvl += (min (dce.flExpMax, 
                dce.ulCmpThr / (float) ulAbsWav) - dce.flExpLvl) * dce.flExpAtk;
            *lpSrcBuf = (GENSMP) (*lpSrcBuf * dce.flExpLvl);
            if (ulEnvNrm) *lpSrcBuf = (GENSMP) (ulEnvNrm * VLTTO_DB_ (dce.flExpLvl));
        }
        else {
            /************************************************************/
            /* If signal is in noise region, apply noise gate           */
            /* Note: Over range is avoided since signal is reduced      */
            /************************************************************/
            if (ulAbsWav < dce.ulNoiThr) {
                *lpSrcBuf = (GENSMP) (*lpSrcBuf * dce.flNoiMul);
            } 
            else {
                *lpSrcBuf = (GENSMP) (*lpSrcBuf * dce.flExpLvl);
            }
            dce.flExpLvl -= (dce.flExpLvl - 1) * dce.flCE_Dcy;
            if (ulEnvNrm) *lpSrcBuf = (GENSMP) (ulEnvNrm * VLTTO_DB_ (dce.flExpLvl));
        }
        /****************************************************************/
        /****************************************************************/
        lpSrcBuf++;
    }

    /********************************************************************/
    /********************************************************************/
    *lpDCEBlk = dce;
    return (ulSmpCnt);

}

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL EffL16toG (LPEBSB lpSrcEBS, LPEBSB lpDstEBS, WORD usNewRes)
{

    LPGSMP  lpSrcBuf;
    DWORD   ulDstRem;
    DWORD   ulInpSmp;

    /********************************************************************/
    /********************************************************************/
    if (!lpSrcEBS->mhBufHdl || !lpSrcEBS->ulSmpCnt) 
        return (EffEBStoE (lpSrcEBS, lpDstEBS, 0L));

    /********************************************************************/
    /********************************************************************/
    if (!(ulDstRem = EBSAloReq (lpDstEBS, EBSPCMDEF, lpSrcEBS->usAtDRes,
      lpSrcEBS->usChnCnt, lpSrcEBS->ulSmpFrq, lpSrcEBS->ulSmpCnt))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }

    /********************************************************************/
    /********************************************************************/
    if (NULL == (lpSrcBuf = GloMemLck (lpSrcEBS->mhBufHdl))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }

    /********************************************************************/
    /********************************************************************/
    ulInpSmp = PCML16toG (lpSrcBuf, (LPGSMP) lpSrcBuf, 
        (WORD) min (lpSrcEBS->ulSmpCnt, ulDstRem), usNewRes);

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (lpSrcEBS->mhBufHdl);

    /********************************************************************/
    /********************************************************************/
    return (EffEBStoE (lpSrcEBS, lpDstEBS, ulInpSmp));

}

DWORD FAR PASCAL EffGtoL16 (LPEBSB lpSrcEBS, LPEBSB lpDstEBS, WORD usNewRes)
{

    LPGSMP  lpSrcBuf;
    DWORD   ulDstRem;
    DWORD   ulInpSmp;

    /********************************************************************/
    /********************************************************************/
    if (!lpSrcEBS->mhBufHdl || !lpSrcEBS->ulSmpCnt) 
        return (EffEBStoE (lpSrcEBS, lpDstEBS, 0L));

    /********************************************************************/
    /********************************************************************/
    if (!(ulDstRem = EBSAloReq (lpDstEBS, EBSPCMDEF, lpSrcEBS->usAtDRes,
      lpSrcEBS->usChnCnt, lpSrcEBS->ulSmpFrq, lpSrcEBS->ulSmpCnt))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }

    /********************************************************************/
    /********************************************************************/
    if (NULL == (lpSrcBuf = GloMemLck (lpSrcEBS->mhBufHdl))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }

    /********************************************************************/
    /********************************************************************/
    ulInpSmp = PCMGtoL16 ((LPGSMP) lpSrcBuf, lpSrcBuf, 
        (WORD) min (lpSrcEBS->ulSmpCnt, ulDstRem), usNewRes);

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (lpSrcEBS->mhBufHdl);

    /********************************************************************/
    /********************************************************************/
    return (EffEBStoE (lpSrcEBS, lpDstEBS, ulInpSmp));

}

DWORD FAR PASCAL EffVolAtD (LPEBSB lpSrcEBS, LPEBSB lpDstEBS, WORD usDstRes)
{
    LPGSMP  lpSrcBuf;
    DWORD   ulDstRem;
    DWORD   ulInpSmp;

    /********************************************************************/
    /********************************************************************/
    if (!lpSrcEBS->mhBufHdl || !lpSrcEBS->ulSmpCnt) 
        return (EffEBStoE (lpSrcEBS, lpDstEBS, 0L));
    if (lpSrcEBS->usAtDRes == usDstRes) 
        return (EffEBStoE (lpSrcEBS, lpDstEBS, lpSrcEBS->ulSmpCnt));

    /********************************************************************/
    /* Check / Allocate destination buffer, init characteristics        */
    /********************************************************************/
    if (!(ulDstRem = EBSAloReq (lpDstEBS, EBSPCMDEF, lpSrcEBS->usAtDRes, 
      lpSrcEBS->usChnCnt, lpSrcEBS->ulSmpFrq, lpSrcEBS->ulSmpCnt))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }

    /********************************************************************/
    /********************************************************************/
    if (NULL == (lpSrcBuf = GloMemLck (lpSrcEBS->mhBufHdl))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }
    ulInpSmp = min (lpSrcEBS->ulSmpCnt, ulDstRem);

    /********************************************************************/
    /* Adjust A to D resolution                                         */
    /********************************************************************/
    PCMGtoL16 (lpSrcBuf, lpSrcBuf, ulInpSmp, lpSrcEBS->usAtDRes);
    PCML16toG (lpSrcBuf, lpSrcBuf, ulInpSmp, lpDstEBS->usAtDRes = usDstRes);
    GloMemUnL (lpSrcEBS->mhBufHdl);

    /********************************************************************/
    /********************************************************************/
    return (EffEBStoE (lpSrcEBS, lpDstEBS, ulInpSmp));

}


