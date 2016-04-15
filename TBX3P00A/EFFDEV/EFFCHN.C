/************************************************************************/
/* Effects Channel Count Support: EffChn.c              V2.00  03/15/95 */
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
#include <math.h>                       /* Math library defs            */

/************************************************************************/
/* Maximum channel# change source size to insure no buffer overflow     */
/************************************************************************/
DWORD FAR PASCAL EffChnSSz (DWORD ulSmpCnt, WORD usSrcChn, WORD usDstChn)
{
    /********************************************************************/
    /********************************************************************/
    if (usSrcChn == usDstChn) return (ulSmpCnt);
    if (!usSrcChn || !usDstChn) return (ulSmpCnt);
    if ((usSrcChn > 2) || (usDstChn > 2)) return (0L);
    return (ulSmpCnt * (DWORD) usSrcChn / (DWORD) usDstChn);    
    
}

DWORD FAR PASCAL EffChnDSz (DWORD ulSmpCnt, WORD usSrcChn, WORD usDstChn)
{
    /********************************************************************/
    /********************************************************************/
    if (usSrcChn == usDstChn) return (ulSmpCnt);
    if (!usSrcChn || !usDstChn) return (ulSmpCnt);
    if ((usSrcChn > 2) || (usDstChn > 2)) return (0L);
    return (ulSmpCnt * (DWORD) usDstChn / (DWORD) usSrcChn);    
}

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL EffChXtoM (LPEBSB lpSrcEBS, LPEBSB lpDstEBS, WORD usChnCnt)
{
    LPGSMP  lpSrcBuf;
    LPSTR   lpDstBuf;
    DWORD   ulDstRem;
    DWORD   ulInpSmp;
    long    lWavMax;
    DWORD   uli;
    DWORD   ulj;

    /********************************************************************/
    /********************************************************************/
    if (!lpSrcEBS->mhBufHdl || !lpSrcEBS->ulSmpCnt) 
        return (EffEBStoE (lpSrcEBS, lpDstEBS, 0L));
    if (!lpSrcEBS->usChnCnt || (usChnCnt <= 1))
        return (EffEBStoE (lpSrcEBS, lpDstEBS, lpSrcEBS->ulSmpCnt));

    /********************************************************************/
    /* Allocate destination based upon target channel count             */
    /********************************************************************/
    if (!(ulDstRem = EBSAloReq (lpDstEBS, EBSPCMDEF, lpSrcEBS->usAtDRes,
      usChnCnt, lpSrcEBS->ulSmpFrq, lpSrcEBS->ulSmpCnt))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }

    /********************************************************************/
    /* Lock source and destination memory buffers                       */
    /********************************************************************/
    if (NULL == (lpSrcBuf = GloMemLck (lpSrcEBS->mhBufHdl))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
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
    /* Source channels are interleaved                                  */
    /********************************************************************/
    lWavMax  = (long) ATDTO_MXM (lpSrcEBS->usAtDRes);
    ulInpSmp = min (lpSrcEBS->ulSmpCnt, ulDstRem * lpSrcEBS->usChnCnt);
    for (uli=0; uli<ulInpSmp/lpSrcEBS->usChnCnt; uli++) {
        long    lCurWav;
        long    lMixSum = 0L;
        LPGSMP  pMixBuf = &lpSrcBuf[uli * lpSrcEBS->usChnCnt];
        for (ulj=0; ulj<lpSrcEBS->usChnCnt; ulj++) lMixSum += pMixBuf[ulj];
        lCurWav = lMixSum / lpSrcEBS->usChnCnt;
        *((LPGSMP) lpDstBuf)++ = (GENSMP) (CLPATDGEN (lCurWav, lWavMax));
    }
    lpDstEBS->ulSmpCnt += ulInpSmp/lpSrcEBS->usChnCnt;
    
    /********************************************************************/
    /* Shift remaining source samples to front of buffer, unlock memory */
    /* If entire source used, delete source memory references           */
    /* Update destination End of Stream flag                            */
    /********************************************************************/
    lpDstEBS->usEOSCod = EBSUpdRel (lpSrcEBS, lpSrcBuf, ulInpSmp);

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (lpDstEBS->mhBufHdl);
    lpDstEBS->usChnCnt = usChnCnt;

    /********************************************************************/
    /********************************************************************/
    return (ulInpSmp/lpSrcEBS->usChnCnt);

}

DWORD FAR PASCAL EffChXtoX (LPGSMP lpMixBuf, DWORD ulSmpCnt, WORD usSrcChn,
                 WORD usDstChn, WORD usAtDRes)
{
    #define CHNCNTINC   2
    DWORD   uli = ulSmpCnt;

    /********************************************************************/
    /********************************************************************/
    if (usSrcChn == usDstChn) return (ulSmpCnt);
    if (!usSrcChn || !usDstChn) return (ulSmpCnt);
    if ((usSrcChn > 2) || (usDstChn > 2)) return (0L);
    
    /********************************************************************/
    /* Mix stereo to mono                                               */
    /********************************************************************/
    if (usSrcChn > usDstChn) {
        long    lWavMax  = (long) ATDTO_MXM (usAtDRes);
        GENSMP huge *hpSrc__A = lpMixBuf;
        GENSMP huge *hpSrc__B = ((GENSMP huge *) lpMixBuf) + 1;
        GENSMP huge *hpDstBuf = lpMixBuf;
        while (uli--) {
            long    lCurWav = ((long) *hpSrc__A + (long) *hpSrc__B) / 2L;
            *hpDstBuf++ = (GENSMP) CLPATDGEN (lCurWav, lWavMax);
            hpSrc__A += CHNCNTINC;
            hpSrc__B += CHNCNTINC;
        }
        return (ulSmpCnt / 2L);
    }

    /********************************************************************/
    /* Separate mono to stereo                                          */
    /********************************************************************/
    if (usSrcChn < usDstChn) {
        GENSMP huge *hpSrcBuf = ((GENSMP huge *) lpMixBuf) + ulSmpCnt - 1;
        GENSMP huge *hpDst__A = ((GENSMP huge *) lpMixBuf) + (2 * ulSmpCnt) - 2;
        GENSMP huge *hpDst__B = ((GENSMP huge *) lpMixBuf) + (2 * ulSmpCnt) - 1;
        while (uli--) {
            *hpDst__A = *hpDst__B = *hpSrcBuf--;
            hpDst__A -= CHNCNTINC;
            hpDst__B -= CHNCNTINC;
        }
        return (ulSmpCnt * 2L);
    }

    /********************************************************************/
    /********************************************************************/
    return (ulSmpCnt);

}


