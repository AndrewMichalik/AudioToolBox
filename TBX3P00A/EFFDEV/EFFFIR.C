/************************************************************************/
/* Effects FIR Time Domain Filters: EffFIR.c            V2.00  07/15/95 */
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
DWORD   FAR PASCAL  FtrFIRFlt (LPGSMP, DWORD, LPTDOF, WORD, WORD FAR *, EFFPOLPRC, DWORD);
DWORD   FAR PASCAL  FtrFIRLFA (LPGSMP, DWORD, LPTDOF, WORD, WORD FAR *, EFFPOLPRC, DWORD);

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL EffFIRAlo (PASTYP usPasTyp, WORD usFtrOrd, DWORD ulStpFrq, 
                DWORD ulRsv001, float flPasFac, float flRipRat, DWORD ulSmpFrq,
                BOOL bfFstFlg, LPTDOF lpTDoFtr)
{

WORD usFtrLen = usFtrOrd+1;

    /********************************************************************/
    /********************************************************************/
    _fmemset (lpTDoFtr, 0, sizeof (*lpTDoFtr));         /* Initialize   */
    if (ulStpFrq >= ulSmpFrq/2L) return (0);

    /********************************************************************/
    /********************************************************************/
    return (EffFIRNOp (usFtrLen, ulStpFrq / (float) ulSmpFrq, EFFWINREC, 
        bfFstFlg, lpTDoFtr));
}

WORD    FAR PASCAL  EffFIRRel (LPTDOF lpTDoFtr)
{
    return (0);
}

DWORD FAR PASCAL EffFIRFtr (LPEBSB lpSrcEBS, LPEBSB lpDstEBS, LPTDOF lpTDoFtr)
{
    LPGSMP  lpSrcBuf;
    DWORD   ulDstRem;
    DWORD   ulInpSmp;

    /********************************************************************/
    /********************************************************************/
    if (!lpSrcEBS->mhBufHdl || !lpSrcEBS->ulSmpCnt) 
        return (EffEBStoE (lpSrcEBS, lpDstEBS, 0L));
    if (!lpTDoFtr->usTyp || !lpTDoFtr->tbZer.usLen)
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
    /********************************************************************/
    if (lpTDoFtr->bfFst) {
        ulInpSmp = FtrFIRLFA (lpSrcBuf, min (lpSrcEBS->ulSmpCnt, ulDstRem), 
            lpTDoFtr, lpSrcEBS->usAtDRes, NULL, NULL, 0L);
    }
    else {
        ulInpSmp = FtrFIRFlt (lpSrcBuf, min (lpSrcEBS->ulSmpCnt, ulDstRem), 
            lpTDoFtr, lpSrcEBS->usAtDRes, NULL, NULL, 0L);
    }

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (lpSrcEBS->mhBufHdl);

    /********************************************************************/
    /********************************************************************/
    return (EffEBStoE (lpSrcEBS, lpDstEBS, ulInpSmp));

}

DWORD FAR PASCAL FtrFIRLFA (LPGSMP lpSrcBuf, DWORD ulSmpCnt, LPTDOF lpTDoFtr,
                 WORD usAtDRes, WORD FAR *lpErrCod, EFFPOLPRC fpPolPrc, DWORD ulPolDat)
{
    LNGFRA  lfWavMax = (LNGFRA) (LNGFRAONE * ATDTO_MXM (usAtDRes));
    TDFBLK  tbZer = lpTDoFtr->tbZer;
    DWORD   uli = ulSmpCnt;

    /********************************************************************/
    /* Show/Hide Progress indicator for time consuming operations       */
    /********************************************************************/
    if (fpPolPrc) fpPolPrc (EFFPOLBEG, ulSmpCnt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    while (uli--) {
        LNGFRA  lfx;
        WORD    usj;
        /****************************************************************/
        /****************************************************************/
        lfx = tbZer.lfCoe[0] * (tbZer.gsHis[0] = *lpSrcBuf);
        for (usj = 1; usj < tbZer.usLen; usj++) 
            lfx += tbZer.lfCoe[usj] * tbZer.gsHis[usj];
        *lpSrcBuf++ = (GENSMP) (CLPATDGEN (lfx, lfWavMax) >> LNGFRANRM);

        /****************************************************************/
        /****************************************************************/
        _fmemmove (&tbZer.gsHis[1], &tbZer.gsHis[0], 
            (tbZer.usLen - 1) * sizeof (*tbZer.gsHis));

        /****************************************************************/
        /****************************************************************/
        if (fpPolPrc && !fpPolPrc (EFFPOLCNT,  ulSmpCnt - uli, ulPolDat)) {
            if (lpErrCod) *lpErrCod = EFF_E_ABT;
            break;
        }
    }
    if (fpPolPrc) fpPolPrc (EFFPOLEND,  ulSmpCnt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    lpTDoFtr->tbZer = tbZer;
    return (ulSmpCnt);
}

DWORD FAR PASCAL FtrFIRFlt (LPGSMP lpSrcBuf, DWORD ulSmpCnt, LPTDOF lpTDoFtr,
                 WORD usAtDRes, WORD FAR *lpErrCod, EFFPOLPRC fpPolPrc, DWORD ulPolDat)
{
    float   flWavMax  = (float) ATDTO_MXM (usAtDRes);
    TDFBLK  tbZer = lpTDoFtr->tbZer;
    DWORD   uli = ulSmpCnt;

    /********************************************************************/
    /* Show/Hide Progress indicator for time consuming operations       */
    /********************************************************************/
    if (fpPolPrc) fpPolPrc (EFFPOLBEG, ulSmpCnt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    while (uli--) {
        float   flx;
        WORD    usj;
        /****************************************************************/
        /****************************************************************/
        flx = tbZer.flCoe[0] * (tbZer.gsHis[0] = *lpSrcBuf);
        for (usj = 1; usj < tbZer.usLen; usj++) 
            flx += tbZer.flCoe[usj] * tbZer.gsHis[usj];
            *lpSrcBuf++ = (GENSMP) CLPATDGEN (flx, flWavMax);

        /****************************************************************/
        /****************************************************************/
        _fmemmove (&tbZer.gsHis[1], &tbZer.gsHis[0], 
            (tbZer.usLen - 1) * sizeof (*tbZer.gsHis));

        /****************************************************************/
        /****************************************************************/
        if (fpPolPrc && !fpPolPrc (EFFPOLCNT,  ulSmpCnt - uli, ulPolDat)) {
            if (lpErrCod) *lpErrCod = EFF_E_ABT;
            break;
        }
    }
    if (fpPolPrc) fpPolPrc (EFFPOLEND,  ulSmpCnt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    lpTDoFtr->tbZer = tbZer;
    return (ulSmpCnt);
}

//void FIRLPFRes (LNGFRA lfCurWav, CVSFTR FAR *lpTDoFtr)
//{
//    WORD    usi;
//    /********************************************************************/
//    /* Reset filter history                                             */
//    /********************************************************************/
//    for (usi=0; usi<lpTDoFtr->fbPol.usLen; usi++) lpTDoFtr->fbPol.lfHis[usi] = lfCurWav;
//}

//DWORD   FIR007LFA (LPGSMP lpSrcBuf, WORD ulSmpCnt, LPTDOF lpTDoFtr)
//{
//    TDFBLK  tbZer = lpTDoFtr->tbZer;
//    DWORD   uli = ulSmpCnt;
//
//    /********************************************************************/
//    /********************************************************************/
//    while (uli--) {
//        LNGFRA  lfx;
//        lfx = tbZer.lfCoe[0] * (tbZer.gsHis[0] = *lpSrcBuf)
//            + tbZer.lfCoe[1] * tbZer.gsHis[1]
//            + tbZer.lfCoe[2] * tbZer.gsHis[2]
//            + tbZer.lfCoe[3] * tbZer.gsHis[3]
//            + tbZer.lfCoe[4] * tbZer.gsHis[4]
//            + tbZer.lfCoe[5] * tbZer.gsHis[5]
//            + tbZer.lfCoe[6] * tbZer.gsHis[6];
//        *lpSrcBuf++ = (GENSMP) (lfx >> LNGFRANRM);
//        _fmemmove (&tbZer.gsHis[1], &tbZer.gsHis[0], 
//            (tbZer.usLen - 1) * sizeof (*tbZer.gsHis));
//    }
//
//    /********************************************************************/
//    /********************************************************************/
//    lpTDoFtr->tbZer = tbZer;
//    return (ulSmpCnt);
//}




