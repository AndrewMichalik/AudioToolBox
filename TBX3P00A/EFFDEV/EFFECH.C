/************************************************************************/
/* Effects Echo Support: EffEch.c                       V2.00  03/15/95 */
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
#include "geneff.h"                     /* Sound Effects definitions    */
#include "effsup.h"                     /* Sound Effects definitions    */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation          */
#include <math.h>                       /* Math library defs            */

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL  EffEchAlo (WORD usEchAlg, DWORD ulDlySmp, float flDec_dB, 
                 LPECHB pfbEchBlk)
{
    _fmemset (pfbEchBlk, 0, sizeof (*pfbEchBlk));

    /********************************************************************/
    /* Check / Allocate delayed source work buffer                      */
    /********************************************************************/
    if (!EBSAloMem (&pfbEchBlk->ebDlyEBS, EBSPCMDEF, 0, 0, 0, ulDlySmp)) 
        return ((WORD) -1);

    /********************************************************************/
    /********************************************************************/
    pfbEchBlk->usEchAlg = usEchAlg;
    pfbEchBlk->ulDlySmp = ulDlySmp;
    pfbEchBlk->flDec_dB = flDec_dB;
    return (0);
}

DWORD FAR PASCAL EffEchDec (LPEBSB lpSrcEBS, LPEBSB lpDstEBS, LPECHB pfbEchBlk)
{
    LPGSMP  lpSrcBuf;
    LPGSMP  lpDlyBuf;
    LPGSMP  lpDstBuf;
    LNGFRA  lfWavMax;
    LNGFRA  lfVolAdj;
    DWORD   ulDstRem;
    DWORD   ulInpSmp;
    DWORD   uli;

    /********************************************************************/
    /********************************************************************/
    if (!lpSrcEBS->mhBufHdl || !lpSrcEBS->ulSmpCnt || !pfbEchBlk->ebDlyEBS.mhBufHdl) 
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
    /* Lock source, delay and destination memory buffers                */
    /********************************************************************/
    if (NULL == (lpSrcBuf = GloMemLck (lpSrcEBS->mhBufHdl))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }
    if (NULL == (lpDlyBuf = GloMemLck (pfbEchBlk->ebDlyEBS.mhBufHdl))) {
        lpSrcEBS->mhBufHdl = GloUnLRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }
    if (NULL == (lpDstBuf = GloMemLck (lpDstEBS->mhBufHdl))) {
        lpSrcEBS->mhBufHdl = GloUnLRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }
    lpDstBuf = &lpDstBuf[EBSSmp2Bh (lpDstEBS->usPCMTyp, lpDstEBS->usChnCnt, lpDstEBS->ulSmpCnt)];

    /********************************************************************/
    /********************************************************************/
    lfWavMax = (LNGFRA) (LNGFRAONE * ATDTO_MXM (lpSrcEBS->usAtDRes));
    lfVolAdj = (LNGFRA) (LNGFRAONE * DB_TO_VLT (pfbEchBlk->flDec_dB));
    ulInpSmp = min (lpSrcEBS->ulSmpCnt, ulDstRem);

    /********************************************************************/
    /* Copy source to destination (unchanged)                           */
    /********************************************************************/
    _fmemcpy (lpDstBuf, lpSrcBuf, (WORD) (ulInpSmp * sizeof (*lpDstBuf)));

    /********************************************************************/
    /* Pad delay buffer with trailing zeros                             */
    /********************************************************************/
    if (pfbEchBlk->ebDlyEBS.ulSmpCnt < pfbEchBlk->ulDlySmp) 
        _fmemset (&lpDlyBuf[pfbEchBlk->ebDlyEBS.ulSmpCnt], 0, 
        (WORD) ((pfbEchBlk->ulDlySmp - pfbEchBlk->ebDlyEBS.ulSmpCnt) 
        * sizeof(*lpDlyBuf)));
    pfbEchBlk->ebDlyEBS.ulSmpCnt = pfbEchBlk->ulDlySmp;

    /********************************************************************/
    /* Compute first pass with delayed data from previous               */
    /********************************************************************/
    for (uli = 0L; uli < min (ulInpSmp, pfbEchBlk->ebDlyEBS.ulSmpCnt); uli++) {
        LNGFRA  lfCurWav = (LNGFRAONE * *lpDstBuf) + (lfVolAdj * lpDlyBuf[uli]);
        *lpDstBuf++ = (GENSMP) (CLPATDGEN (lfCurWav, lfWavMax) >> LNGFRANRM);
    }

    /********************************************************************/
    /* Compute next pass with delayed original data                     */
    /********************************************************************/
    for (uli = 0L; uli < ulInpSmp - min (ulInpSmp, pfbEchBlk->ebDlyEBS.ulSmpCnt); uli++) 
//      *lpDstBuf++ = *lpDstBuf + (GENSMP) ((lfVolAdj * lpSrcBuf[uli]) >> LNGFRANRM);
        *lpDstBuf++ = (GENSMP) (CLPATDGEN ((LNGFRAONE * *lpDstBuf) 
            + (lfVolAdj * lpSrcBuf[uli]), lfWavMax) >> LNGFRANRM);
    lpDstEBS->ulSmpCnt += ulInpSmp;

    /********************************************************************/
    /* Copy tail of psDstBuf for feedback if not final block            */
    /* Stuttering occurs as ulDlySmp gets larger than available buffers */
    /********************************************************************/
    uli = min (ulInpSmp, pfbEchBlk->ulDlySmp);
    _fmemcpy (lpDlyBuf, &lpSrcBuf[ulInpSmp - uli], (WORD) (uli * sizeof (*lpDlyBuf)));
    pfbEchBlk->ebDlyEBS.ulSmpCnt = uli;

    /********************************************************************/
    /* Shift remaining source samples to front of buffer, unlock memory */
    /* If entire source used, delete source memory references           */
    /* Update destination End of Stream flag                            */
    /********************************************************************/
    lpDstEBS->usEOSCod = EBSUpdRel (lpSrcEBS, lpSrcBuf, ulInpSmp);

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (pfbEchBlk->ebDlyEBS.mhBufHdl);
    GloMemUnL (lpDstEBS->mhBufHdl);

    /********************************************************************/
    /********************************************************************/
    return (ulInpSmp);

}

WORD FAR PASCAL  EffEchRel (LPECHB pfbEchBlk)
{
    pfbEchBlk->ebDlyEBS.mhBufHdl = GloAloRel (pfbEchBlk->ebDlyEBS.mhBufHdl);
    return (0);
}


