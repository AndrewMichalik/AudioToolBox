/************************************************************************/
/* Effects Buffer Stream Support: EffEBS.c              V2.00  03/15/95 */
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
#include "geneff.h"                     /* Sound Effects definitions    */
#include "effsup.h"                     /* Effects support definitions  */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */
#include <math.h>                       /* Math library defs            */
  
/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL EffPtoEBS (short sSrcHdl, DWORD FAR *lpulBytRem, 
          LPEBSB lpDstEBS, DWORD ulSmpMax, PCMTYP usSrcPCM, WORD usChnCnt,
          WORD usBIOEnc, DWORD ulSmpFrq, LPITCB pibDstITC, EBSFRDPRC fpPCMRd_G16)
{
    DWORD   ulDstRem;
    DWORD   ulInpSmp;
    LPSTR   lpDstBuf;
    DWORD   ulFilByt;

    /********************************************************************/
    /* Check / Allocate destination buffer, init characteristics        */
    /********************************************************************/
    if (!(ulDstRem = EBSAloReq (lpDstEBS, EBSPCMDEF, (WORD) EBSAtDRes (usSrcPCM),
        usChnCnt, ulSmpFrq, EBSCNTDEF))) {
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }

    /********************************************************************/
    /* Calculate available destination samples and source request       */
    /********************************************************************/
    ulInpSmp = min (ulSmpMax, min (ulDstRem, 
        EBSByt2Sl (usSrcPCM, usChnCnt, *lpulBytRem)));

    /********************************************************************/
    /* Set destination pointer past existing samples                    */
    /********************************************************************/
    if (NULL == (lpDstBuf = GloMemLck (lpDstEBS->mhBufHdl))) {
        if (!lpDstEBS->ulSmpCnt) lpDstEBS->mhBufHdl = GloAloRel (lpDstEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }
    lpDstBuf = &lpDstBuf[EBSSmp2Bh (lpDstEBS->usPCMTyp, lpDstEBS->usChnCnt, lpDstEBS->ulSmpCnt)];

    /********************************************************************/
    /* Read file PCM data in rounded integral bytes                     */
    /********************************************************************/
    ulInpSmp = EBSSmpLow (usSrcPCM, usChnCnt, ulInpSmp);
    ulInpSmp = fpPCMRd_G16 (sSrcHdl, min (*lpulBytRem, BYTMAXFIO), 
        (LPGSMP) lpDstBuf, ulInpSmp, usSrcPCM, lpDstEBS->usChnCnt, 
        usBIOEnc, pibDstITC, &ulFilByt, NULL, NULL, 0L);
    GloMemUnL (lpDstEBS->mhBufHdl);

    /********************************************************************/
    /* Check return code and update End Of Stream indicator             */
    /********************************************************************/
    if ((-1L == ulInpSmp) || (0L == ulInpSmp)) {
        if (!lpDstEBS->ulSmpCnt) lpDstEBS->mhBufHdl = GloAloRel (lpDstEBS->mhBufHdl);
        lpDstEBS->usEOSCod = ulInpSmp ? FIOEOSERR : FIOEOSEOF;
        *lpulBytRem = 0L;
        return (ulInpSmp);
    }
    else lpDstEBS->usEOSCod = (ulFilByt < *lpulBytRem) ? FIOEOS_OK : FIOEOSEOF;

    /********************************************************************/
    /********************************************************************/
    lpDstEBS->ulSmpCnt += ulInpSmp;
    *lpulBytRem -= ulFilByt;

    /********************************************************************/
    /********************************************************************/
    return (ulInpSmp);
}

DWORD FAR PASCAL EffMtoEBS (LPVOID lpSrcBuf, DWORD FAR *lpulBytRem, 
          LPEBSB lpDstEBS, DWORD ulSmpMax, PCMTYP usSrcPCM, WORD usChnCnt,
          WORD usBIOEnc, DWORD ulSmpFrq, LPITCB pibDstITC, EBSMRDPRC fpPCMPtoG16)
{
    DWORD   ulDstRem;
    DWORD   ulInpSmp;
    LPSTR   lpDstBuf;
    DWORD   ulMemByt;

    /********************************************************************/
    /* Check / Allocate destination buffer, init characteristics        */
    /********************************************************************/
    if (!(ulDstRem = EBSAloReq (lpDstEBS, EBSPCMDEF, (WORD) EBSAtDRes (usSrcPCM),
        usChnCnt, ulSmpFrq, EBSCNTDEF))) {
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }

    /********************************************************************/
    /* Calculate available destination samples and source request       */
    /********************************************************************/
    ulInpSmp = min (ulSmpMax, min (ulDstRem, 
        EBSByt2Sl (usSrcPCM, usChnCnt, *lpulBytRem)));

    /********************************************************************/
    /* Set destination pointer past existing samples                    */
    /********************************************************************/
    if (NULL == (lpDstBuf = GloMemLck (lpDstEBS->mhBufHdl))) {
        if (!lpDstEBS->ulSmpCnt) lpDstEBS->mhBufHdl = GloAloRel (lpDstEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }
    lpDstBuf = &lpDstBuf[EBSSmp2Bh (lpDstEBS->usPCMTyp, lpDstEBS->usChnCnt, lpDstEBS->ulSmpCnt)];

    /********************************************************************/
    /* Read memory PCM data in rounded integral bytes                   */
    /********************************************************************/
    ulInpSmp = EBSSmpLow (usSrcPCM, usChnCnt, ulInpSmp);
    ulInpSmp = fpPCMPtoG16 (lpSrcBuf, min (*lpulBytRem, BYTMAXFIO), 
        (LPGSMP) lpDstBuf, ulInpSmp, usSrcPCM, lpDstEBS->usChnCnt, 
        usBIOEnc, pibDstITC, &ulMemByt, NULL);
    GloMemUnL (lpDstEBS->mhBufHdl);

    /********************************************************************/
    /* Check return code and update End Of Stream indicator             */
    /********************************************************************/
    if ((-1L == ulInpSmp) || (0L == ulInpSmp)) {
        if (!lpDstEBS->ulSmpCnt) lpDstEBS->mhBufHdl = GloAloRel (lpDstEBS->mhBufHdl);
        lpDstEBS->usEOSCod = ulInpSmp ? FIOEOSERR : FIOEOSEOF;
        *lpulBytRem = 0L;
        return (ulInpSmp);
    }
    else lpDstEBS->usEOSCod = (ulMemByt < *lpulBytRem) ? FIOEOS_OK : FIOEOSEOF;

    /********************************************************************/
    /********************************************************************/
    lpDstEBS->ulSmpCnt += ulInpSmp;
    *lpulBytRem -= ulMemByt;

    /********************************************************************/
    /* Shift remaining source memory bytes to front of buffer           */
    /********************************************************************/
    if (*lpulBytRem) _fmemmove (lpSrcBuf, (LPBYTE) lpSrcBuf + ulMemByt, 
        (WORD) *lpulBytRem);

    /********************************************************************/
    /********************************************************************/
    return (ulInpSmp);
}

DWORD FAR PASCAL EffEBStoE (LPEBSB lpSrcEBS, LPEBSB lpDstEBS, DWORD ulReqSmp)
{
    LPSTR   lpSrcBuf;
    LPSTR   lpDstBuf;
    DWORD   ulDstTot;

    /********************************************************************/
    /* If source is null, set EOS indicator & return                    */
    /********************************************************************/
    if (!lpSrcEBS->mhBufHdl) {
        lpDstEBS->usEOSCod = lpSrcEBS->usEOSCod;
        return (0L);
    }

    /********************************************************************/
    /* Insure that # requested samples is in range                      */
    /********************************************************************/
    ulReqSmp = min (ulReqSmp, lpSrcEBS->ulSmpCnt);

    /********************************************************************/
    /* If entire source is used, delete source after complete           */
    /* If destination is null...                                        */
    /********************************************************************/
    if ((ulReqSmp == lpSrcEBS->ulSmpCnt) && !lpDstEBS->mhBufHdl) {
        /****************************************************************/
        /* Copy source to dest, delete source memory references         */
        /****************************************************************/
        *lpDstEBS = *lpSrcEBS;
        lpSrcEBS->mhBufHdl = 0L;
        return (lpDstEBS->ulSmpCnt);
    }

    /********************************************************************/
    /* If destination is null & entire source is not used...            */
    /********************************************************************/
//  allocate

    /********************************************************************/
    /* If destination is not null, check available room...              */
    /********************************************************************/
    ulDstTot = EBSByt2Sl (lpDstEBS->usPCMTyp, lpDstEBS->usChnCnt, GloMemSiz (lpDstEBS->mhBufHdl));
    if (lpDstEBS->ulSmpCnt >= ulDstTot) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }
    if (NULL == (lpSrcBuf = GloMemLck (lpSrcEBS->mhBufHdl))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }
    if (NULL == (lpDstBuf = GloMemLck (lpDstEBS->mhBufHdl))) {
        GloMemUnL (lpSrcEBS->mhBufHdl);
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }
    ulReqSmp = min (ulReqSmp, ulDstTot - lpDstEBS->ulSmpCnt);

    /********************************************************************/
    /* Append maximum source samples to destination                     */
    /********************************************************************/
    _fmemmove (&lpDstBuf[EBSSmp2Bh (lpDstEBS->usPCMTyp, lpDstEBS->usChnCnt, lpDstEBS->ulSmpCnt)], 
        lpSrcBuf, (WORD) EBSSmp2Bh (lpSrcEBS->usPCMTyp, lpSrcEBS->usChnCnt, ulReqSmp));

    /********************************************************************/
    /* Shift remaining source samples to front of buffer                */
    /* If entire source used, delete source memory references           */
    /* Update destination End of Stream flag                            */
    /********************************************************************/
    lpDstEBS->usEOSCod = EBSUpdRel (lpSrcEBS, lpSrcBuf, ulReqSmp);

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (lpDstEBS->mhBufHdl);

    /********************************************************************/
    /********************************************************************/
    lpDstEBS->ulSmpCnt += ulReqSmp;
    return (ulReqSmp);

}

DWORD FAR PASCAL EffEBStoP (LPEBSB lpSrcEBS, DWORD ulSmpMax, short sDstHdl,
          DWORD ulBytMax, PCMTYP usDstPCM, WORD usBIOEnc, LPITCB pibDstITC, 
          EBSFWRPRC fpPCMWr_PCM)
{
    WORD    usBytCnt;
    DWORD   ulOutSmp;
    LPSTR   lpSrcBuf;

    /********************************************************************/
    /********************************************************************/
    if (!lpSrcEBS->mhBufHdl) return (0L);

    /********************************************************************/
    /* Write file PCM data in integral bytes                            */
    /********************************************************************/
    if (NULL == (lpSrcBuf = GloMemLck (lpSrcEBS->mhBufHdl))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        return ((DWORD) -1L);
    }
    ulOutSmp = min (ulSmpMax, lpSrcEBS->usEOSCod ? lpSrcEBS->ulSmpCnt 
        : EBSSmpLow (usDstPCM, lpSrcEBS->usChnCnt, lpSrcEBS->ulSmpCnt));
    usBytCnt = (WORD) fpPCMWr_PCM ((LPGSMP) lpSrcBuf, ulOutSmp, sDstHdl, 
        min (ulBytMax, BYTMAXFIO), usDstPCM, lpSrcEBS->usChnCnt, usBIOEnc,
        pibDstITC, &ulOutSmp, NULL, NULL, 0L);
    if ((-1 == usBytCnt) || (0 == usBytCnt)) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        return (usBytCnt ? -1L : 0L);
    }

    /********************************************************************/
    /* Shift remaining source samples to front of buffer                */
    /* If entire source used, delete source memory references           */
    /********************************************************************/
    EBSUpdRel (lpSrcEBS, lpSrcBuf, ulOutSmp);

    /********************************************************************/
    /********************************************************************/
    return (usBytCnt);
}

DWORD FAR PASCAL EffEBStoM (LPEBSB lpSrcEBS, DWORD ulSmpMax, LPVOID lpDstBuf,
          DWORD ulBytMax, PCMTYP usDstPCM, WORD usBIOEnc, LPITCB pibDstITC, 
          EBSMWRPRC fpPCMG16toP)
{
    WORD    usBytCnt;
    DWORD   ulOutSmp;
    LPSTR   lpSrcBuf;

    /********************************************************************/
    /********************************************************************/
    if (!lpSrcEBS->mhBufHdl) return (0L);

    /********************************************************************/
    /* Write memory PCM data in integral bytes                          */
    /********************************************************************/
    if (NULL == (lpSrcBuf = GloMemLck (lpSrcEBS->mhBufHdl))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        return ((DWORD) -1L);
    }
    ulOutSmp = min (ulSmpMax, lpSrcEBS->usEOSCod ? lpSrcEBS->ulSmpCnt 
        : EBSSmpLow (usDstPCM, lpSrcEBS->usChnCnt, lpSrcEBS->ulSmpCnt));
    usBytCnt = (WORD) fpPCMG16toP ((LPGSMP) lpSrcBuf, ulOutSmp, lpDstBuf, 
        min (ulBytMax, BYTMAXFIO), usDstPCM, lpSrcEBS->usChnCnt, usBIOEnc, 
        pibDstITC, &ulOutSmp, NULL);
    if ((-1 == usBytCnt) || (0 == usBytCnt)) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        return (usBytCnt ? -1L : 0L);
    }

    /********************************************************************/
    /* Shift remaining source samples to front of buffer                */
    /* If entire source used, delete source memory references           */
    /********************************************************************/
    EBSUpdRel (lpSrcEBS, lpSrcBuf, ulOutSmp);

    /********************************************************************/
    /********************************************************************/
    return (usBytCnt);
}

