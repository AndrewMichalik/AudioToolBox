/************************************************************************/
/* Effects Re-Sampling Support: EffRes.c                V2.00  03/15/95 */
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
#include <string.h>                     /* String manipulation funcs    */
#include <math.h>                       /* Floating point math funcs    */

/************************************************************************/
/************************************************************************/
WORD    FAR PASCAL LinSmpInt (GENSMP _huge *, GENSMP _huge *, WORD, DWORD, DWORD, struct _RESBLK FAR *);     
WORD    FAR PASCAL LinSmpDec (GENSMP _huge *, GENSMP _huge *, WORD, DWORD, DWORD, struct _RESBLK FAR *);     

/************************************************************************/
/* Maximum resample source size to insure no buffer overflow            */
/* Maximum resample destination size to insure no buffer overflow       */
/************************************************************************/
LPRESB  FAR PASCAL EffResSet (LPRESB prbResBlk)
{
    return ((LPRESB) _fmemset (prbResBlk, 0, sizeof (RESBLK)));
}

DWORD FAR PASCAL EffResSSz (DWORD ulDstSmp, DWORD ulSrcFrq, DWORD ulDstFrq, 
                 LPRESB prbResBlk)
{
    DWORD   ulRnd_Up;
    DWORD   ulPhsOff;

    /********************************************************************/
    /* Return no change for equal frequencies                           */
    /********************************************************************/
    if (!ulSrcFrq || !ulDstFrq || (ulSrcFrq == ulDstFrq)) return (ulDstSmp);

    /********************************************************************/
    /* If NULL == prbResBlk, return min size independent of phase pos   */
    /********************************************************************/
    if (!prbResBlk) return ((DWORD) (ulSrcFrq * (ulDstSmp / (double) ulDstFrq)));

    /********************************************************************/
    /* (ulSrcFrq - 1L) is the amount to round up source before trunc.   */
    /* Subtract fractional source sample remaining to prevent overflow  */
    /* Phase offset / ulSrcFrq = the fractional source sample remaining */
    /********************************************************************/
    ulPhsOff = (DWORD) fmod (ulDstFrq * (double) prbResBlk->ulPhsPos, ulSrcFrq);
    ulRnd_Up = (ulSrcFrq - 1L - ulPhsOff);
    return ((DWORD) ((ulSrcFrq * (double) ulDstSmp + ulRnd_Up) / (double) ulDstFrq));

}

DWORD FAR PASCAL EffResDSz (DWORD ulSrcSmp, DWORD ulSrcFrq, DWORD ulDstFrq, 
                 LPRESB prbResBlk)
{
    DWORD   ulPhsOff;

    /********************************************************************/
    /* Return no change for equal frequencies                           */
    /********************************************************************/
    if (!ulSrcFrq || !ulDstFrq || (ulSrcFrq == ulDstFrq)) return (ulSrcSmp);

    /********************************************************************/
    /* If NULL == prbResBlk, return max size independent of phase pos   */
    /********************************************************************/
    if (!prbResBlk) 
        return ((DWORD) ceil (ulDstFrq * (ulSrcSmp / (double) ulSrcFrq)));

    /********************************************************************/
    /* Phase offset / ulSrcFrq = the fractional source sample remaining */
    /********************************************************************/
    ulPhsOff = (DWORD) fmod (ulDstFrq * (double) prbResBlk->ulPhsPos, ulSrcFrq);
    return ((DWORD) ((ulDstFrq * (double) ulSrcSmp + ulPhsOff) / (double) ulSrcFrq));
}

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL EffFrqRes (LPEBSB lpSrcEBS, LPEBSB lpDstEBS, DWORD ulDstFrq,
                 LPRESB lpResBlk)
{
    DWORD   ulDstRem;
    DWORD   ulInpSmp;
    DWORD   ulOutSmp;
    LPSTR   lpSrcBuf;
    LPSTR   lpDstBuf;

    /********************************************************************/
    /********************************************************************/
    if (!lpSrcEBS->mhBufHdl || !lpSrcEBS->ulSmpCnt) 
        return (EffEBStoE (lpSrcEBS, lpDstEBS, 0L));

    /********************************************************************/
    /* Check frequencies for valid range                                */
    /********************************************************************/
    if ((lpSrcEBS->ulSmpFrq == ulDstFrq) || !lpSrcEBS->ulSmpFrq || !ulDstFrq)
        return (EffEBStoE (lpSrcEBS, lpDstEBS, lpSrcEBS->ulSmpCnt));

    /********************************************************************/
    /* Check / Allocate destination buffer                              */
    /********************************************************************/
    if (!(ulDstRem = EBSAloReq (lpDstEBS, lpSrcEBS->usPCMTyp, lpSrcEBS->usAtDRes,
      lpSrcEBS->usChnCnt, lpSrcEBS->ulSmpFrq, EffResDSz (lpSrcEBS->ulSmpCnt,
      lpSrcEBS->ulSmpFrq, ulDstFrq, lpResBlk)))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }

    /********************************************************************/
    /* Adjust max read request for Interpolation/Decimation effects     */
    /********************************************************************/
    ulInpSmp = min (lpSrcEBS->ulSmpCnt, 
        EffResSSz (ulDstRem, lpSrcEBS->ulSmpFrq, ulDstFrq, lpResBlk)); 

    /********************************************************************/
    /********************************************************************/
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
    lpDstBuf = &lpDstBuf[EBSSmp2Bh (lpDstEBS->usPCMTyp, lpDstEBS->usChnCnt, lpDstEBS->ulSmpCnt)];

    /********************************************************************/
    /********************************************************************/
    lpDstEBS->ulSmpCnt += ulOutSmp = EffIntDec ((LPGSMP) lpSrcBuf, 
        (LPGSMP) lpDstBuf, ulInpSmp, lpSrcEBS->ulSmpFrq, ulDstFrq, lpResBlk,     
         NULL, NULL, 0L);

    /********************************************************************/
    /* Shift remaining source samples to front of buffer, unlock memory */
    /* If entire source used, delete source memory references           */
    /* Update destination End of Stream flag                            */
    /********************************************************************/
    lpDstEBS->usEOSCod = EBSUpdRel (lpSrcEBS, lpSrcBuf, ulInpSmp);

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (lpDstEBS->mhBufHdl);
    lpDstEBS->ulSmpFrq = ulDstFrq;

    /********************************************************************/
    /********************************************************************/
    return (ulOutSmp);
}

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL EffIntDec (LPGSMP lpSrcBuf, LPGSMP lpDstBuf,
         DWORD ulSrcCnt, DWORD ulSrcFrq, DWORD ulDstFrq, LPRESB lpResBlk,     
         WORD FAR *lpErrCod, EFFPOLPRC fpPolPrc, DWORD ulPolDat)
{
    DWORD   ulInpCnt = 0L;
    DWORD   ulOutCnt = 0L;
    WORD    usReqCnt;

    /********************************************************************/
    /* Validate input variables                                         */
    /********************************************************************/
    if ((!ulSrcCnt) || (!ulSrcFrq) || (!ulDstFrq)) return (0); 
    if (ulSrcFrq == ulDstFrq) return (ulSrcCnt);

    /********************************************************************/
    /* Note: Assume single channel (mono) buffers                       */
    /********************************************************************/
    if ((lpSrcBuf == lpDstBuf) && (ulSrcFrq < ulDstFrq)) {
        DWORD   ulBufSiz = EffResDSz (ulSrcCnt, ulSrcFrq, ulDstFrq, lpResBlk); 
        lpSrcBuf = hmemmove (((GENSMP _huge *) lpDstBuf) + ulBufSiz - ulSrcCnt, 
            (GENSMP _huge *) lpSrcBuf, ulSrcCnt * sizeof (GENSMP));
    }

    /********************************************************************/
    /* Show/Hide Progress indicator for time consuming operations       */
    /********************************************************************/
    if (fpPolPrc) fpPolPrc (EFFPOLBEG, ulSrcCnt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    while (ulSrcCnt > 0L) {
        usReqCnt = (WORD) min (ulSrcCnt, 
            EffResSSz (BYTMAXFIO / sizeof (GENSMP), ulSrcFrq, ulDstFrq, lpResBlk));

        /****************************************************************/
        /* Interpolate/Decimate                                         */
        /****************************************************************/
        if (ulSrcFrq < ulDstFrq) {
            ulOutCnt += (DWORD) LinSmpInt (
                ((GENSMP _huge *) lpSrcBuf) + ulInpCnt, 
                ((GENSMP _huge *) lpDstBuf) + ulOutCnt, 
                usReqCnt, ulSrcFrq, ulDstFrq, lpResBlk);
        }
        else {
            ulOutCnt += (DWORD) LinSmpDec (
                ((GENSMP _huge *) lpSrcBuf) + ulInpCnt, 
                ((GENSMP _huge *) lpDstBuf) + ulOutCnt, 
                usReqCnt, ulSrcFrq, ulDstFrq, lpResBlk);
        }

        ulInpCnt += (DWORD) usReqCnt;
        ulSrcCnt -= (DWORD) usReqCnt;

        /****************************************************************/
        /****************************************************************/
        if (fpPolPrc && !fpPolPrc (EFFPOLCNT,  ulInpCnt, ulPolDat)) {
            ulInpCnt = (DWORD) -1L;
            if (lpErrCod) *lpErrCod = EFF_E_ABT;
            break;
        }
    }
    if (fpPolPrc) fpPolPrc (EFFPOLEND,  ulInpCnt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    return (ulOutCnt);

}

WORD FAR PASCAL LinSmpDec (GENSMP _huge *hpSrcBuf, GENSMP _huge *hpDstBuf,
         WORD usSrcCnt, DWORD ulSrcFrq, DWORD ulDstFrq, LPRESB prbResBlk)     
{

    ldiv_t  ldPosRem;                   /* Interpolation position & rem */
    DWORD   ulIntNum;                   /* Interpolation numerator      */
    DWORD   ulIntRem;                   /* Interpolation remainder      */
    WORD    usRgtPos;                   /* Interpolation right position */
    DWORD   ulPhsOff;                   /* Interpolation local phase    */
    GENSMP  gsFstWav;                   /* Interpolation first value    */
    long    lLftWav;                    /* Interpolation left  value    */
    long    lRgtWav;                    /* Interpolation right value    */
    WORD    usDstCnt;               
    WORD    usi;

    /********************************************************************/
    /* Validate input variables                                         */
    /********************************************************************/
    if ((!usSrcCnt) || (!ulSrcFrq) || (!ulDstFrq)) return (0); 
    gsFstWav = prbResBlk->gsCurWav; 

    /********************************************************************/
    /* Reduce sample count from higher source rate to lower dest rate   */
    /* Current sample point Interp moves linearly from 0 to usSrcCnt-1  */
    /*      as a line (y = mx + b) Interp = flStpCnt * usi + flPhsOff.  */
    /*      Thus Interp(olate) moves by flStpCnt = ulSrcFrq / ulDstFrq  */
    /*      from initial phase offset flPhsOff.                         */
    /* usLftPos =  floor (ulPhsOff + Interp)                            */
    /*             where ulPhsOff = ulPhsPos / ulDstFrq                 */
    /* flNewWav =  SrcFrq[LftPos] * (1 - rem (Interp))                  */
    /*          +  SrcFrq[RgtPos] * (rem (Interp))                      */
    /* usDstCnt <  1 + (usSrcCnt * ulDstFrq / ulSrcFrq)                 */
    /* Note: Over-range is avoided since fractions always sum to unity  */
    /********************************************************************/
    ulPhsOff = (DWORD) fmod ((double) ulDstFrq * prbResBlk->ulPhsPos, ulSrcFrq);
    usDstCnt = (WORD) EffResDSz (usSrcCnt, ulSrcFrq, ulDstFrq, prbResBlk);
    prbResBlk->ulPhsPos += (DWORD) usSrcCnt;
    prbResBlk->gsCurWav = hpSrcBuf[usSrcCnt-1];

    for (usi = 0; usi < usDstCnt; usi++) {
        ulIntNum = ((DWORD) usi + 1L) * ulSrcFrq - ulPhsOff;
        ldPosRem = ldiv (ulIntNum, ulDstFrq);

        usRgtPos = (WORD) ldPosRem.quot;
        ulIntRem = ldPosRem.rem;

        lLftWav = (long) (usRgtPos ? hpSrcBuf[usRgtPos-1] : gsFstWav);
        // Note: When the last source is dead on, ulIntRem == 0,
        // and hpSrcBuf[usRgtPos] is at end of buffer
        lRgtWav = (long) (usRgtPos < usSrcCnt ? hpSrcBuf[usRgtPos] : 0L);

        hpDstBuf[usi] = (GENSMP) 
            ((lLftWav * (long) (ulDstFrq - ulIntRem) 
            + lRgtWav * (long) ulIntRem) / (long) ulDstFrq);

    }

    /********************************************************************/
    /********************************************************************/
    return (usDstCnt);

}

WORD FAR PASCAL LinSmpInt (GENSMP _huge *hpSrcBuf, GENSMP _huge *hpDstBuf, 
         WORD usSrcCnt, DWORD ulSrcFrq, DWORD ulDstFrq, LPRESB prbResBlk)     
{

    ldiv_t  ldPosRem;                   /* Interpolation position & rem */
    DWORD   ulIntNum;                   /* Interpolation numerator      */
    DWORD   ulIntRem;                   /* Interpolation remainder      */
    WORD    usRgtPos;                   /* Interpolation right position */
    DWORD   ulPhsOff;                   /* Interpolation local phase    */
    GENSMP  gsFstWav;                   /* Interpolation first value    */
    long    lLftWav;                    /* Interpolation left  value    */
    long    lRgtWav;                    /* Interpolation right value    */
    WORD    usDstCnt;               
    WORD    usPasCnt;               
    WORD    usi;

    /********************************************************************/
    /* Validate input variables                                         */
    /********************************************************************/
    if ((!usSrcCnt) || (!ulSrcFrq) || (!ulDstFrq)) return (0); 
    gsFstWav = prbResBlk->gsCurWav; 

    /********************************************************************/
    /* Increase sample count from lower source rate to higher dest rate */
    /* Current sample point Interp moves linearly from 0 to usSrcCnt-1  */
    /*      as a line (y = mx + b) Interp = flStpCnt * usi + flPhsOff.  */
    /*      Thus Interp(olate) moves by flStpCnt = ulSrcFrq / ulDstFrq  */
    /*      from initial phase offset flPhsOff.                         */
    /* usLftPos =  floor (ulPhsOff + Interp)                            */
    /*             where ulPhsOff = ulPhsPos / ulDstFrq                 */
    /* flNewWav =  SrcFrq[LftPos] * (1 - rem (Interp))                  */
    /*          +  SrcFrq[RgtPos] * (rem (Interp))                      */
    /* usDstCnt <  1 + (usSrcCnt * ulDstFrq / ulSrcFrq)                 */
    /*                                                                  */
    /* Note: Reverse computation permits permits "in place" conversion; */
    /* Note: Over-range is avoided since fractions always sum to unity  */
    /********************************************************************/
    ulPhsOff = (DWORD) fmod ((double) ulDstFrq * prbResBlk->ulPhsPos, ulSrcFrq);
    usPasCnt = usDstCnt = (WORD) EffResDSz (usSrcCnt, ulSrcFrq, ulDstFrq, prbResBlk);
    prbResBlk->ulPhsPos += (DWORD) usSrcCnt;
    prbResBlk->gsCurWav = hpSrcBuf[usSrcCnt-1];

    usPasCnt = usDstCnt;
    usi = (hpSrcBuf == hpDstBuf) ? usDstCnt - 1 : 0;

    while (usPasCnt--) {
        ulIntNum = ((DWORD) usi + 1L) * ulSrcFrq - ulPhsOff; 
        ldPosRem = ldiv (ulIntNum, ulDstFrq);

        usRgtPos = (WORD) ldPosRem.quot;
        ulIntRem = ldPosRem.rem;

        lLftWav = (long) (usRgtPos ? hpSrcBuf[usRgtPos-1] : gsFstWav);
        // Note: When the last source is dead on, ulIntRem == 0,
        // and hpSrcBuf[usRgtPos] is at end of buffer
        lRgtWav = (long) (usRgtPos < usSrcCnt ? hpSrcBuf[usRgtPos] : 0L);

        hpDstBuf[usi] = (GENSMP) 
            ((lLftWav * (long) (ulDstFrq - ulIntRem) 
            + lRgtWav * (long) ulIntRem) / (long) ulDstFrq);

        usi += (hpSrcBuf == hpDstBuf) ? -1 : +1;
    }

    /********************************************************************/
    /********************************************************************/
    return (usDstCnt);

}


