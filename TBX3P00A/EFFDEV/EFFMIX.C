/************************************************************************/
/* Effects Mix Support: EffMix.c                        V2.00  03/15/95 */
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
#include "effsup.h"                     /* Sound Effects definitions    */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <math.h>                       /* Math library defs            */

/************************************************************************/
/************************************************************************/
DWORD   FAR PASCAL  EffMixMul (LPEBSB FAR *lpSrcEBS, LPEBSB lpDstEBS,
                    WORD usMixCnt, float FAR *flMixLvl)
{
    #define MAXCHNCNT   2
    LPGSMP  lpSrcBuf[MAXCHNCNT];
    LNGFRA  lfVolAdj[MAXCHNCNT];
    LNGFRA  lfWavMax;
    DWORD   ulDstRem;
    DWORD   ulSmpOut;
    DWORD   uli;

    /********************************************************************/
    /* Insure valid number of mix channels                              */
    /********************************************************************/
    usMixCnt = min (usMixCnt, MAXCHNCNT);

    /********************************************************************/
    /* Check all inputs for valid EBS                                   */
    /* Note: should the input stream be flushed to output?              */
    /********************************************************************/
    for (uli = 0; uli < usMixCnt; uli++) 
        if (!lpSrcEBS[uli]->mhBufHdl || !lpSrcEBS[uli]->ulSmpCnt) return (0L);

    /********************************************************************/
    /* Check / Allocate destination buffer, init characteristics        */
    /********************************************************************/
    if (!(ulDstRem = EBSAloReq (lpDstEBS, EBSPCMDEF, lpSrcEBS[0]->usAtDRes,
      lpSrcEBS[0]->usChnCnt, lpSrcEBS[0]->ulSmpFrq, lpSrcEBS[0]->ulSmpCnt))) {
        lpSrcEBS[0]->mhBufHdl = GloAloRel (lpSrcEBS[0]->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }

    /********************************************************************/
    /* Lock source data streams                                         */
    /********************************************************************/
    for (uli = 0; uli < usMixCnt; uli++) {
        if (NULL == (lpSrcBuf[uli] = GloMemLck (lpSrcEBS[uli]->mhBufHdl))) {
            lpSrcEBS[uli]->mhBufHdl = GloAloRel (lpSrcEBS[uli]->mhBufHdl);
            lpDstEBS->usEOSCod = FIOEOSERR;
            return ((DWORD) -1L);
        }
    }

    /********************************************************************/
    /* Adjust levels and sample counts                                  */
    /********************************************************************/
    ulSmpOut = min (ulSmpOut, ulDstRem);
    for (uli = 0; uli < usMixCnt; uli++) {
        lfVolAdj[uli] = (LNGFRA) (LNGFRAONE * flMixLvl[uli]);
        ulSmpOut = min (ulSmpOut, lpSrcEBS[uli]->ulSmpCnt);
    }
    lfWavMax = (LNGFRA) (LNGFRAONE * ATDTO_MXM (lpSrcEBS[0]->usAtDRes));

    /********************************************************************/
    /* Mix to source data stream [0]                                    */
    /* Future: Modify to accept usMixCnt != 2                           */
    /********************************************************************/
    uli = ulSmpOut;
    while (uli--) {
        LNGFRA  lfCurWav = (lpSrcBuf[0][uli] * lfVolAdj[0]) + (lpSrcBuf[1][uli] * lfVolAdj[1]);
        lpSrcBuf[0][uli] = (GENSMP) (CLPATDGEN (lfCurWav, lfWavMax) >> LNGFRANRM);
    }

    /********************************************************************/
    /* Unlock and release mixed source to destination                   */
    /********************************************************************/
    GloMemUnL (lpSrcEBS[0]->mhBufHdl);
    for (uli = 1; uli < usMixCnt; uli++) {
        EBSUpdRel (lpSrcEBS[uli], lpSrcBuf[uli], ulSmpOut);
    }
    return (EffEBStoE (lpSrcEBS[0], lpDstEBS, ulSmpOut));

}


