/************************************************************************/
/* Effects Fade Support: EffFad.c                       V2.00  03/15/95 */
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
WORD    FAR PASCAL  EffFadAlo (WORD usFadAlg, float flFadBeg, float flFadEnd,
                    DWORD ulSmpCnt, LPFADB pfbFadBlk)
{
    _fmemset (pfbFadBlk, 0, sizeof (*pfbFadBlk));

    /********************************************************************/
    /********************************************************************/
    if (!ulSmpCnt) return (0);

    /********************************************************************/
    /********************************************************************/
    pfbFadBlk->usFadAlg = usFadAlg;
    pfbFadBlk->flFadBas = (float) pow (10., flFadBeg / 10); /* Squared dB   */;
    pfbFadBlk->flFadRat = (float)  
        ((pow (10., flFadEnd / 10) - pfbFadBlk->flFadBas) / ulSmpCnt);

    /********************************************************************/
    /********************************************************************/
    return (0);

}

DWORD   FAR PASCAL  EffFad_IO (LPEBSB lpSrcEBS, LPEBSB lpDstEBS, LPFADB pfbFadBlk)
{
    LPGSMP  lpSrcBuf;
    DWORD   ulDstRem;
    DWORD   ulInpSmp;
    long    lWavMax;
    DWORD   uli;

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
    /* Vary the power level (square) at a linear rate                   */
    /********************************************************************/
    lWavMax  = (long) ATDTO_MXM (lpSrcEBS->usAtDRes);
    uli = ulInpSmp = min (lpSrcEBS->ulSmpCnt, ulDstRem);
    while (uli--) {
        long    lCurWav = (long) (*lpSrcBuf * sqrt (pfbFadBlk->flFadBas += pfbFadBlk->flFadRat));
        *lpSrcBuf++ = (GENSMP) CLPATDGEN (lCurWav, lWavMax);
    }
    GloMemUnL (lpSrcEBS->mhBufHdl);

    /********************************************************************/
    /********************************************************************/
    return (EffEBStoE (lpSrcEBS, lpDstEBS, ulInpSmp));

}

WORD FAR PASCAL  EffFadRel (LPFADB pfbFadBlk)
{
    return (0);
}

