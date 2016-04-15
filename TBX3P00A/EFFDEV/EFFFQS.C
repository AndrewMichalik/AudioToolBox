/************************************************************************/
/* Effects Frequency shifts: EffFQS.c                   V2.00  03/15/95 */
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
#define MSTDIFPTS(x)    ((x/2L)+1L)     /* MST diff buffer mag/amp cnt  */      
#define MSTINPPTS(x)    (x/8L)          /* MST win dependent input cnt  */      

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL EffVocAlo (EFFFFTCBK lpVocFnc, LPVOCB lpVocBlk, 
                LPVOID lpUsrDat, WORD usFFTOrd, WORD usWinOrd, 
                float flIntRat, float flPchMul, float flSynThr, 
                DWORD ulSmpFrq)
{
    /********************************************************************/
    /* Initialize entire struct (& FFT overlapped block)                */
    /********************************************************************/
    _fmemset (lpVocBlk, 0, sizeof (*lpVocBlk));

    /********************************************************************/
    /* Check and initialize data window & FFT size values               */
    /********************************************************************/
    if ((usWinOrd <= 0) || (usFFTOrd <= 0)) return ((WORD) -1);
    lpVocBlk->ulWinPts = (DWORD) pow (2., (double) usWinOrd);
    lpVocBlk->ulFFTPts = (DWORD) pow (2., (double) usFFTOrd);

    /********************************************************************/
    /* Check and initialize sample input / output time values           */
    /********************************************************************/
    if (flIntRat <= 0) return ((WORD) -1);
    lpVocBlk->lInpTim  = - (long) lpVocBlk->ulWinPts;
    lpVocBlk->lOutTim  = - (long) (lpVocBlk->ulWinPts * flIntRat);

    /********************************************************************/
    /* Check and initialize sample input / output count values          */
    /********************************************************************/
    lpVocBlk->ulInpNum = MSTINPPTS (lpVocBlk->ulWinPts);
    lpVocBlk->ulOutDen = (DWORD) (flIntRat * lpVocBlk->ulInpNum);
    if (!lpVocBlk->ulInpNum || !lpVocBlk->ulOutDen) return ((WORD) -1);

    /********************************************************************/
    /* Initialize VocBlk fundamental freq and size parameters           */
    /********************************************************************/
    if (ulSmpFrq <= 0) return ((WORD) -1);
    lpVocBlk->flBinFrq = ulSmpFrq / (float) lpVocBlk->ulFFTPts;           

    /********************************************************************/
    /* Allocate analysis buffers                                        */
    /********************************************************************/
    if (VocMSTAlo (&lpVocBlk->mbInpMST, lpVocBlk->ulFFTPts, lpVocBlk->ulWinPts, 
      lpVocBlk->ulInpNum, lpVocBlk->ulOutDen, ulSmpFrq, VOCMSTANA)) {
        return ((WORD) -1);
    }

    /********************************************************************/
    /* NULL selection: Choose Mod Short Term or Osc Bank re-synthesis   */
    /********************************************************************/
    if (NULL == lpVocFnc) {
        if (1 == flPchMul) lpVocFnc = MSTVocMST; 
        else lpVocFnc = MSTVocOBS;
    }

    /********************************************************************/
    /* Allocate synthesis buffers                                       */
    /********************************************************************/
    if (MSTVocMST == lpVocFnc) {
        if (VocMSTAlo (&lpVocBlk->mbOutMST, lpVocBlk->ulFFTPts, lpVocBlk->ulWinPts, 
          lpVocBlk->ulInpNum, lpVocBlk->ulOutDen, ulSmpFrq, VOCMSTSYN)) {
            VocMSTRel (&lpVocBlk->mbInpMST);
            return ((WORD) -1);
        }
    } 
    else if (MSTVocOBS == lpVocFnc) {
        lpVocBlk->flPchMul = flPchMul;
        lpVocBlk->flSynThr = flSynThr; 
        if (VocOBSAlo (&lpVocBlk->obOutOBS, lpVocBlk->ulFFTPts, lpVocBlk->ulWinPts,
          lpVocBlk->ulInpNum, lpVocBlk->ulOutDen, ulSmpFrq)) {
            VocMSTRel (&lpVocBlk->mbInpMST);
            return ((WORD) -1);
        }
    } 
    else return ((WORD) -1);

    /********************************************************************/
    /* Allocate output work buf (2 x (#FFT / 2 + 1))                    */
    /********************************************************************/
    if (!(lpVocBlk->obOvrBlk.mhWrkHdl = GloAloMem (GMEM_MOVEABLE, sizeof (float) * 
      (2 * MSTDIFPTS(lpVocBlk->ulFFTPts))))) {
        VocMSTRel (&lpVocBlk->mbInpMST);
        VocMSTRel (&lpVocBlk->mbOutMST);
        VocOBSRel (&lpVocBlk->obOutOBS);
        return ((WORD) -1);
    } 

    /********************************************************************/
    /********************************************************************/
    lpVocBlk->lpVocFnc = lpVocFnc;
    lpVocBlk->lpUsrDat = lpUsrDat;

    /********************************************************************/
    /********************************************************************/
    return (0);

}

WORD FAR PASCAL EffVocRel (LPVOCB lpVocBlk)
{
    /********************************************************************/
    /* Release resources                                                */
    /********************************************************************/
    VocMSTRel (&lpVocBlk->mbInpMST);
    VocMSTRel (&lpVocBlk->mbOutMST);
    VocOBSRel (&lpVocBlk->obOutOBS);
    GloAloRel (lpVocBlk->obOvrBlk.mhWrkHdl);

    /********************************************************************/
    /* Insure safe multiple release calls                               */
    /********************************************************************/
    _fmemset (lpVocBlk, 0, sizeof (*lpVocBlk));
    return (0);

}

/************************************************************************/
/*          Vocoder Effects overlapped input / output functions         */
/************************************************************************/
DWORD FAR PASCAL EffFFTVoc (LPEBSB lpSrcEBS, LPEBSB lpDstEBS, LPVOCB lpVocBlk,
                 EFFPOLPRC fpPolPrc, DWORD ulPolDat)
{
    /********************************************************************/
    /********************************************************************/
    if (!lpSrcEBS->mhBufHdl || !lpSrcEBS->ulSmpCnt) 
        return (EffEBStoE (lpSrcEBS, lpDstEBS, 0L));
    if (!lpVocBlk->ulFFTPts || !lpVocBlk->ulWinPts || !lpVocBlk->ulInpNum 
      || !lpVocBlk->ulOutDen || !lpVocBlk->lpVocFnc) 
        return (EffEBStoE (lpSrcEBS, lpDstEBS, lpSrcEBS->ulSmpCnt));

    /********************************************************************/
    /* Requires a minimum of ulWinPts samples for normal processing     */
    /********************************************************************/
    if ((lpSrcEBS->ulSmpCnt < lpVocBlk->ulWinPts) && !lpSrcEBS->usEOSCod) 
        return (0L);

    /********************************************************************/
    /* Convert input in ulWinPts sized chunks with ulInpNum tail        */
    /********************************************************************/
    return (EffFFTOvr (lpSrcEBS, lpDstEBS, lpVocBlk->ulWinPts, 
        lpVocBlk->ulWinPts - lpVocBlk->ulInpNum, lpVocBlk->ulInpNum, 
        &lpVocBlk->obOvrBlk, lpVocBlk->lpVocFnc, lpVocBlk, fpPolPrc, ulPolDat));

}

/************************************************************************/
/*          Vocoder Effects overlapped input / output functions         */
/************************************************************************/
WORD FAR PASCAL MSTVocMST (float FAR * FAR *ppflSrcBuf, DWORD FAR *lpulDstCnt,
                DWORD ulBufPts, DWORD ulTotPts, DWORD ulHdrPts, DWORD ulCtrPts, 
                DWORD ulDstRem, VOCBLK FAR *pvbVocBlk)
{
    DWORD   ulOutCnt;

    /********************************************************************/
    /* The EffFFTFtr (caller) delivers a set of data points where       */
    /* ulCtrPts at offset ulHdrPts have been isolated to implement      */
    /* the overlap-save method of FFT corruption prevention.            */
    /********************************************************************/
    
    /********************************************************************/
    /* Default returned data count to zero                              */
    /********************************************************************/
    *lpulDstCnt = 0L;

//ajm - before or after completion pass check??
    /********************************************************************/
    /* Space available in output?                                       */ 
    /********************************************************************/
    ulOutCnt = (pvbVocBlk->ulOutDen * ulCtrPts / pvbVocBlk->ulInpNum)
        + (ulCtrPts == pvbVocBlk->ulInpNum) ? 0L : pvbVocBlk->ulDlyPts;
    if (ulDstRem < ulOutCnt) return (FALSE);
    
    /********************************************************************/
    /* Completion pass?
    /********************************************************************/
    if (ulCtrPts < pvbVocBlk->ulInpNum) {
        DWORD   ulFilCnt;
        ulFilCnt  = min (pvbVocBlk->ulInpNum - ulCtrPts, pvbVocBlk->ulDlyPts);
        ulCtrPts += ulFilCnt;
        pvbVocBlk->ulDlyPts -= ulFilCnt;
    }
    if (!ulCtrPts) return (FALSE);

    /********************************************************************/
    /********************************************************************/
    pvbVocBlk->lInpTim += pvbVocBlk->ulInpNum;    
    pvbVocBlk->lOutTim += pvbVocBlk->ulOutDen;    

    /********************************************************************/
    /********************************************************************/
    MSTVocAna (*ppflSrcBuf, pvbVocBlk->ulInpNum, pvbVocBlk->lInpTim, 
        pvbVocBlk->ulWinPts, pvbVocBlk->ulFFTPts, pvbVocBlk->flBinFrq, 
        &pvbVocBlk->mbInpMST);

    /********************************************************************/
    /********************************************************************/
    ulOutCnt = MSTVocSyn (*ppflSrcBuf, pvbVocBlk->ulOutDen, pvbVocBlk->lOutTim,
        pvbVocBlk->ulWinPts, pvbVocBlk->ulFFTPts, pvbVocBlk->flBinFrq, 
        &pvbVocBlk->mbOutMST);

    /********************************************************************/
    /* Adjust delayed input count if no output                          */
    /********************************************************************/
    if (!ulOutCnt) pvbVocBlk->ulDlyPts += ulCtrPts;

    /********************************************************************/
    /* Output number of points factored by ratio of input / expected    */
    /********************************************************************/
    *lpulDstCnt = ulOutCnt * ulCtrPts / pvbVocBlk->ulInpNum;

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);

}

WORD FAR PASCAL MSTVocOBS (float FAR * FAR *ppflSrcBuf, DWORD FAR *lpulDstCnt,
                DWORD ulBufPts, DWORD ulTotPts, DWORD ulHdrPts, DWORD ulCtrPts, 
                DWORD ulDstRem, VOCBLK FAR *pvbVocBlk)
{
    DWORD   ulOutCnt;

    /********************************************************************/
    /* The EffFFTFtr (caller) delivers a set of data points where       */
    /* ulCtrPts at offset ulHdrPts have been isolated to implement      */
    /* the overlap-save method of FFT corruption prevention.            */
    /********************************************************************/
    
    /********************************************************************/
    /* Default returned data count to zero                              */
    /********************************************************************/
    *lpulDstCnt = 0L;

//ajm - before or after completion pass check??
    /********************************************************************/
    /* Space available in output?                                       */ 
    /********************************************************************/
    ulOutCnt = (pvbVocBlk->ulOutDen * ulCtrPts / pvbVocBlk->ulInpNum)
        + (ulCtrPts == pvbVocBlk->ulInpNum) ? 0L : pvbVocBlk->ulDlyPts;
    if (ulDstRem < ulOutCnt) return (FALSE);
    
    /********************************************************************/
    /* Completion pass?
    /********************************************************************/
    if (ulCtrPts < pvbVocBlk->ulInpNum) {
        DWORD   ulFilCnt;
        ulFilCnt  = min (pvbVocBlk->ulInpNum - ulCtrPts, pvbVocBlk->ulDlyPts);
        ulCtrPts += ulFilCnt;
        pvbVocBlk->ulDlyPts -= ulFilCnt;
    }
    if (!ulCtrPts) return (FALSE);

    /********************************************************************/
    /********************************************************************/
    pvbVocBlk->lInpTim += pvbVocBlk->ulInpNum;    
    pvbVocBlk->lOutTim += pvbVocBlk->ulOutDen;    

    /********************************************************************/
    /********************************************************************/
    MSTVocAna (*ppflSrcBuf, pvbVocBlk->ulInpNum, pvbVocBlk->lInpTim, 
        pvbVocBlk->ulWinPts, pvbVocBlk->ulFFTPts, pvbVocBlk->flBinFrq, 
        &pvbVocBlk->mbInpMST);

    /********************************************************************/
    /********************************************************************/
    ulOutCnt = OBSVocSyn (*ppflSrcBuf, pvbVocBlk->ulOutDen, 
//Perfect with FFT odd inversion, ulWinPts < ulFFTPts; NG with ulWinPts >= ulFFTPts
//        pvbVocBlk->lOutTim,
//Eliminates initial "whistle" 
pvbVocBlk->lInpTim + (pvbVocBlk->ulWinPts / 2L) - (pvbVocBlk->ulInpNum / 2),
//pvbVocBlk->lInpTim + (pvbVocBlk->ulWinPts / 2L) + pvbVocBlk->ulOutDen,
        pvbVocBlk->ulWinPts, pvbVocBlk->ulFFTPts, pvbVocBlk->flPchMul, 
        pvbVocBlk->flSynThr, &pvbVocBlk->obOutOBS);

    /********************************************************************/
    /* Adjust delayed input count if no output                          */
    /********************************************************************/
    if (!ulOutCnt) pvbVocBlk->ulDlyPts += ulCtrPts;

    /********************************************************************/
    /* Output number of points factored by ratio of input / expected    */
    /********************************************************************/
    *lpulDstCnt = ulOutCnt * ulCtrPts / pvbVocBlk->ulInpNum;

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);

}
