/************************************************************************/
/* Effects Noise Reduce Support: EffNoi.c               V2.00  03/15/95 */
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
/************************************************************************/
DWORD FAR PASCAL EffNoiRed (LPEBSB lpSrcEBS, LPEBSB lpDstEBS, DWORD ulMutThr,
                 float flLvl_dB)
{
    LPGSMP  lpSrcBuf;
    LNGFRA  lfVolAdj;
    DWORD   ulDstRem;
    DWORD   ulInpSmp;
    DWORD   uli;

    /********************************************************************/
    /********************************************************************/
    if (!lpSrcEBS->mhBufHdl || !lpSrcEBS->ulSmpCnt) 
        return (EffEBStoE (lpSrcEBS, lpDstEBS, 0L));
    if (!ulMutThr) 
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
    /* Attenuate signals below threshold level                          */
    /* Note: Over range is avoided since signal level is reduced        */
    /* Note: use negative of absolute value of attenuation              */
    /********************************************************************/
    lfVolAdj = (LNGFRA) (LNGFRAONE * DB_TO_VLT (-fabs (flLvl_dB)));
    uli = ulInpSmp = min (lpSrcEBS->ulSmpCnt, ulDstRem);
    while (uli--) if (abs (*lpSrcBuf) < (long) ulMutThr) 
        *lpSrcBuf++ = (GENSMP) ((*lpSrcBuf * lfVolAdj) >> LNGFRANRM);
    GloMemUnL (lpSrcEBS->mhBufHdl);

    /********************************************************************/
    /********************************************************************/
    return (EffEBStoE (lpSrcEBS, lpDstEBS, ulInpSmp));

}

