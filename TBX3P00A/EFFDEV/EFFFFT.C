/************************************************************************/
/* Effects FFT based Filters: EffFFT.c                  V2.00  07/15/95 */
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

#include <math.h>                       /* Math library defs            */
#include <string.h>                     /* String manipulation          */

/************************************************************************/
/* Convert input in 4L FFT sized chunks with 2L Center, 1L sides        */
/************************************************************************/
#define TOTBLKCNT   4L
#define CTRBLKCNT   2L
#define HDRBLKCNT   1L

/************************************************************************/
/* General purpose routines for providing overlap-saved EBS buffers to  */
/* user-defined functions that then perform FFT operations              */
/************************************************************************/
DWORD FAR PASCAL EffFFTSSz (DWORD ulDstSmp, WORD usFFTOrd, float flResRat) 
{
//    (DWORD) pow (2., (double) usFFTOrd);
    return (0);
}

DWORD FAR PASCAL EffFFTDSz (DWORD ulSrcSmp, WORD usFFTOrd, float flResRat) 
{
//    (DWORD) pow (2., (double) usFFTOrd);
    return (0);
}

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL EffFFTAlo (WORD usFFTOrd, float flResRat, LPFFTB lpFFTBlk, 
                EFFPOLPRC fpPolPrc, DWORD ulPolDat)
{
    float FAR * lpWinBuf;
    float FAR * lpXfrBuf;
    DWORD       uli;

    /********************************************************************/
    /* Initialize entire struct (& FFT overlapped block)                */
    /********************************************************************/
    _fmemset (lpFFTBlk, 0, sizeof (*lpFFTBlk));

    /********************************************************************/
    /* Check and initialize data window & FFT size values               */
    /********************************************************************/
    lpFFTBlk->ulFFTPts = (DWORD) pow (2., (double) usFFTOrd);

    /********************************************************************/
    /* Allocate data window buffer (2L x #FFT points for overlap-save)  */
    /* Initialize to unity window                                       */
    /********************************************************************/
    if (NULL == (lpWinBuf = GloAloLck (GMEM_MOVEABLE, &lpFFTBlk->mhWinHdl, 
      2L * lpFFTBlk->ulFFTPts * sizeof (float)))) {
        return ((WORD) -1);
    }
    for (uli=0L; uli < 2 * lpFFTBlk->ulFFTPts; uli++) lpWinBuf[uli] = 1;
    GloMemUnL (lpFFTBlk->mhWinHdl);

    /********************************************************************/
    /* Allocate transfer filter buf (2L x #FFT points for overlap-save) */
    /* Initialize to unity (real portion, DC & Nyquist special case)    */
    /********************************************************************/
    if (NULL == (lpXfrBuf = GloAloLck (GMEM_MOVEABLE, &lpFFTBlk->mhXfrHdl, 
      2L * lpFFTBlk->ulFFTPts * sizeof (float)))) {
        lpFFTBlk->mhWinHdl = GloAloRel (lpFFTBlk->mhWinHdl);
        return ((WORD) -1);
    }
    for (uli=0L; uli < 2 * lpFFTBlk->ulFFTPts; uli++)  lpXfrBuf[uli] = 0;
    lpXfrBuf[0] = lpXfrBuf[1] = 1;
    for (uli=2L; uli < 2 * lpFFTBlk->ulFFTPts; uli+=2) lpXfrBuf[uli] = 1;
    GloMemUnL (lpFFTBlk->mhXfrHdl);

    /********************************************************************/
    /* Allocate output work buf (TOTBLKCNT x #FFT points for A-B-C-D)   */
    /********************************************************************/
    if (!(lpFFTBlk->obOvrBlk.mhWrkHdl = GloAloMem (GMEM_MOVEABLE, sizeof (float) * 
      (TOTBLKCNT * lpFFTBlk->ulFFTPts)))) {
        lpFFTBlk->mhWinHdl = GloAloRel (lpFFTBlk->mhWinHdl);
        lpFFTBlk->mhXfrHdl = GloAloRel (lpFFTBlk->mhXfrHdl);
        return ((WORD) -1);
    } 

    /********************************************************************/
    /* Check and initialize sample input / output time values           */
    /* Beat up by 2 even if original requires less than 2 change        */
    /* This reduces linear interpolation / decimation distortion        */
    /* Don't use FFT to downsample - linear will perform better         */
    /* Note: ceil (LOGBASTWO (x)) may actually incr due to rounding err */
    /********************************************************************/
    if (flResRat <= 0) return ((WORD) -1);
    lpFFTBlk->flResRat = (flResRat <= 1) 
        ? 1: (float) pow (2, ceil (LOGBASTWO (flResRat)));

    /********************************************************************/
    /********************************************************************/
    lpFFTBlk->fpPolPrc = fpPolPrc;
    lpFFTBlk->ulPolDat = ulPolDat;

    /********************************************************************/
    /********************************************************************/
    return (0);

}
WORD FAR PASCAL EffFFTRel (LPFFTB lpFFTBlk)
{
    /********************************************************************/
    /* Release resources                                                */
    /********************************************************************/
    GloAloRel (lpFFTBlk->mhWinHdl);
    GloAloRel (lpFFTBlk->mhXfrHdl);
    GloAloRel (lpFFTBlk->obOvrBlk.mhWrkHdl);

    /********************************************************************/
    /* Insure safe multiple release calls                               */
    /********************************************************************/
    _fmemset (lpFFTBlk, 0, sizeof (*lpFFTBlk));
    return (0);
}

DWORD FAR PASCAL EffFFTFtr (LPEBSB lpSrcEBS, LPEBSB lpDstEBS, LPFFTB lpFFTBlk, 
                 EFFFFTCBK lpFtrFnc, LPVOID lpUsrDat)
{
    DWORD       ulBlkPts;               /* FFT Blk size (# of pts)      */
    DWORD       ulDstCnt;

    /********************************************************************/
    /********************************************************************/
    if (!lpSrcEBS->mhBufHdl || !lpSrcEBS->ulSmpCnt) 
        return (EffEBStoE (lpSrcEBS, lpDstEBS, 0L));
    if (!lpFFTBlk->ulFFTPts || !lpFtrFnc) 
        return (EffEBStoE (lpSrcEBS, lpDstEBS, lpSrcEBS->ulSmpCnt));

    /********************************************************************/
    /* Check and initialize sample input block count                    */
    /* Reduce size of block count to insure that re-sampled data fits   */
    /********************************************************************/
    ulBlkPts = (lpFFTBlk->flResRat <= 1) ?
        lpFFTBlk->ulFFTPts : (DWORD) (lpFFTBlk->ulFFTPts / lpFFTBlk->flResRat);

    /********************************************************************/
    /* Real FFT size is half of complex.                                */
    /* FFT buffer = 4n real FFT points * bytes/float (A-D buffer)       */
    /* Wrk buffer = 2n real FFT points * bytes/float (Prv buffer)       */
    /********************************************************************/
    if (!(ulBlkPts /= 2)) return (EffEBStoE (lpSrcEBS, lpDstEBS, lpSrcEBS->ulSmpCnt));

    /********************************************************************/
    /* Requires a minimum of four data blocks for normal processing     */
    /********************************************************************/
    if ((lpSrcEBS->ulSmpCnt < TOTBLKCNT * ulBlkPts) && !lpSrcEBS->usEOSCod) 
        return (0L);

    /********************************************************************/
    /* Set user private data pointer                                    */
    /********************************************************************/
    lpFFTBlk->lpUsrDat = lpUsrDat;    

    /********************************************************************/
    /* Convert input in 4L * ulBlkPts FFT sized chunks with 1L sides    */
    /********************************************************************/
    ulDstCnt = EffFFTOvr (lpSrcEBS, lpDstEBS, TOTBLKCNT * ulBlkPts, 
        HDRBLKCNT * ulBlkPts, CTRBLKCNT * ulBlkPts, &lpFFTBlk->obOvrBlk, 
        lpFtrFnc, lpFFTBlk, lpFFTBlk->fpPolPrc, lpFFTBlk->ulPolDat);

    /********************************************************************/
    /* Update destination frequency                                     */
    /********************************************************************/
    lpDstEBS->ulSmpFrq = (DWORD) (lpSrcEBS->ulSmpFrq * lpFFTBlk->flResRat);

    /********************************************************************/
    /********************************************************************/
    return (ulDstCnt);

}

