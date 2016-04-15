/************************************************************************/
/* Effects FFT Overlap Support: FFTOvr.c                V2.00  03/15/95 */
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
DWORD   GetOvrBlk (LPGSMP, float FAR *, DWORD, DWORD, DWORD);
DWORD   FLTtoG (float FAR *, LPGSMP, DWORD, long, long);
DWORD   GtoFLT (LPGSMP, float FAR *, DWORD, long, long);

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL EffFFTOvr (LPEBSB lpSrcEBS, LPEBSB lpDstEBS, DWORD ulTotPts, 
                 DWORD ulHdrPts, DWORD ulCtrPts, LPOVRB lpOvrBlk, 
                 EFFFFTCBK lpFFTFnc, LPVOID lpFFTDat, 
                 EFFPOLPRC fpPolPrc, DWORD ulPolDat)
{
    float FAR * lpflWrkBuf;             /* FFT Blk A, B, C & D (input)  */
    float FAR * lpflOutPtr;             /* Fnc Blk             (output) */
    LPGSMP      lpgsSrcBuf;
    LPGSMP      lpgsDstBuf;
    DWORD       ulSrcPos;
    DWORD       ulFshCnt;               /* # Bytes fresh from source    */
    DWORD       ulSrcCnt;               /* # Bytes not needed again     */
    DWORD       ulDstRem;
    DWORD       ulWrkPts;
    DWORD       ulDstCnt;
    DWORD       ulDstTot;

    /********************************************************************/
    /* Check / Allocate destination buffer                              */
    /********************************************************************/
    if (!(ulDstRem = EBSAloReq (lpDstEBS, lpSrcEBS->usPCMTyp, lpSrcEBS->usAtDRes,
        lpSrcEBS->usChnCnt, lpSrcEBS->ulSmpFrq, EBSCNTDEF))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }

    /********************************************************************/
    /* Initialize buffer pointers                                       */
    /* On failure: release input, leave output                          */
    /********************************************************************/
    if (NULL == (lpgsSrcBuf = GloMemLck (lpSrcEBS->mhBufHdl))) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }
    if (NULL == (lpgsDstBuf = GloMemLck (lpDstEBS->mhBufHdl))) {
        lpSrcEBS->mhBufHdl = GloUnLRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }
    if (NULL == (lpflWrkBuf = GloMemLck (lpOvrBlk->mhWrkHdl))) {
        lpSrcEBS->mhBufHdl = GloUnLRel (lpSrcEBS->mhBufHdl);
        GloMemUnL (lpDstEBS->mhBufHdl);
        lpDstEBS->usEOSCod = FIOEOSERR;
        return ((DWORD) -1L);
    }
    ulWrkPts = GloMemSiz (lpOvrBlk->mhWrkHdl) / sizeof (*lpflWrkBuf);

    /********************************************************************/
    /* Set source offset position past "used" header sample points      */
    /********************************************************************/
    lpOvrBlk->ulSrcOff = min (lpOvrBlk->ulSrcOff, ulHdrPts);   
    ulSrcPos = 0L;
    ulDstTot = 0L;
    ulDstCnt = 0L;

    /********************************************************************/
    /* Show/Hide Progress indicator for time consuming operations       */
    /********************************************************************/
    if (fpPolPrc) fpPolPrc (EFFPOLBEG, 
        lpSrcEBS->ulSmpCnt - min (lpOvrBlk->ulSrcOff, lpSrcEBS->ulSmpCnt), ulPolDat);

    /********************************************************************/
    /* Loop while sufficient for full read (New=ulSmpCnt-ulSrcOff)      */
    /********************************************************************/
    while ((lpSrcEBS->ulSmpCnt >= ulTotPts) || lpSrcEBS->usEOSCod) {
        /****************************************************************/
        /* Restore "A" (Hdr) buffer from previous pass (zeros if first) */
        /* Read new block of data into BC (Ctr) & D (End) buffer        */
        /* ulFshCnt = Available fresh source bytes                      */
        /* ulSrcCnt = (This Hdr) + (Disposable Fresh) - (Next Hdr)      */
        /****************************************************************/
        ulFshCnt = GetOvrBlk (&lpgsSrcBuf[ulSrcPos], lpflWrkBuf, 
            ulHdrPts - lpOvrBlk->ulSrcOff, lpSrcEBS->ulSmpCnt, ulTotPts);
        ulCtrPts = min (ulCtrPts, ulFshCnt - min (ulFshCnt, lpOvrBlk->ulSrcOff));
        ulSrcCnt = lpOvrBlk->ulSrcOff + min (ulFshCnt, ulCtrPts) 
            - min (lpOvrBlk->ulSrcOff + ulCtrPts, ulHdrPts);

        /****************************************************************/
        /* Perform FFT operation on buffers A thru D                    */
        /****************************************************************/
        lpflOutPtr = lpflWrkBuf;
        if (!lpFFTFnc (&lpflOutPtr, &ulDstCnt, ulWrkPts, ulTotPts, 
            ulHdrPts, ulCtrPts, ulDstRem, lpFFTDat)) break;
        if (ulDstCnt > ulDstRem) break;

        /****************************************************************/
        /* Write data to destination buffer                             */
        /* Future: Convert if ebDstEBS is G16 - else leave as float     */
        /****************************************************************/
        FLTtoG (lpflOutPtr, &lpgsDstBuf[lpDstEBS->ulSmpCnt], ulDstCnt, 
            -0x7fff, 0x7fff);

        /****************************************************************/
        /* Update source and destination sample counts                  */
        /****************************************************************/
        lpSrcEBS->ulSmpCnt -= ulSrcCnt;
        ulSrcPos           += ulSrcCnt;
        lpDstEBS->ulSmpCnt += ulDstCnt;
        ulDstTot           += ulDstCnt;
        ulDstRem           -= ulDstCnt;

        /****************************************************************/
        /* Move new source offset pos past "used" header sample points  */
        /****************************************************************/
        lpOvrBlk->ulSrcOff = min (lpOvrBlk->ulSrcOff + ulCtrPts, ulHdrPts);

        /****************************************************************/
        /****************************************************************/
        if (fpPolPrc && !fpPolPrc (EFFPOLCNT, 
          ulSrcPos - min (lpOvrBlk->ulSrcOff, ulSrcPos), ulPolDat)) {
            lpSrcEBS->ulSmpCnt = lpDstEBS->ulSmpCnt = ulDstTot = 0L;
            lpSrcEBS->usEOSCod = FIOEOSCAN;                      
            break;
        }
    }
    if (fpPolPrc) fpPolPrc (EFFPOLEND, 
        ulSrcPos - min (lpOvrBlk->ulSrcOff, ulSrcPos), ulPolDat);

    /********************************************************************/
    /* Release locks and memory buffers                                 */
    /********************************************************************/
    if ((lpSrcEBS->ulSmpCnt > lpOvrBlk->ulSrcOff) || !lpSrcEBS->usEOSCod) { 
        lpSrcEBS->ulSmpCnt += ulSrcPos;         // Restore org cnt for release
        lpDstEBS->usEOSCod = EBSUpdRel (lpSrcEBS, lpgsSrcBuf, ulSrcPos);
    } else {
        lpDstEBS->usEOSCod = lpSrcEBS->usEOSCod;
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
    }
    GloMemUnL (lpOvrBlk->mhWrkHdl);
    GloMemUnL (lpDstEBS->mhBufHdl);
    if (!lpDstEBS->ulSmpCnt) lpDstEBS->mhBufHdl = GloAloRel (lpDstEBS->mhBufHdl);

    /********************************************************************/
    /********************************************************************/
    return (ulDstTot);

}

DWORD   GetOvrBlk (LPGSMP lpgsSrcBuf, float FAR *lpflDstBuf, DWORD ulIniZer,
        DWORD ulSrcCnt, DWORD ulDstCnt)
{
    DWORD       uli;

    /********************************************************************/
    /********************************************************************/
    ulIniZer = min (ulIniZer, ulDstCnt);
    for (uli=0; uli < ulIniZer; uli++) *lpflDstBuf++ = (float) 0;
    ulDstCnt -= ulIniZer;

    /********************************************************************/
    /********************************************************************/
    ulSrcCnt = min (ulSrcCnt, ulDstCnt);
    GtoFLT (lpgsSrcBuf, lpflDstBuf, ulSrcCnt, -0x7fff, 0x7fff);
    for (uli=ulSrcCnt; uli < ulDstCnt; uli++) lpflDstBuf[uli] = (float) 0;

    /********************************************************************/
    /********************************************************************/
    return (ulSrcCnt);

}

/************************************************************************/
/************************************************************************/
DWORD   FLTtoG (float FAR *lpflSrcBuf, LPGSMP lpgsDstBuf, DWORD ulSrcCnt, 
        long lDstMin, long lDstMax)
{
    float   flSrcLoc;    
    DWORD   uli;

    /********************************************************************/
    /* Convert float to short; store short in destination buff          */
    /********************************************************************/
    for (uli=0L; uli<ulSrcCnt; uli++) {
        flSrcLoc = *lpflSrcBuf++;        
        if (flSrcLoc < (float) lDstMin) *lpgsDstBuf++ = (short) lDstMin;
          else if (flSrcLoc > (float) lDstMax) *lpgsDstBuf++ = (short) lDstMax;
            else *lpgsDstBuf++ = (short) flSrcLoc;
    }
    return (ulSrcCnt);

}

DWORD   GtoFLT (LPGSMP lpgsSrcBuf, float FAR *lpflDstBuf, DWORD ulSrcCnt, 
        long lDstMin, long lDstMax)
{
    DWORD   uli;

    /********************************************************************/
    /* Convert short to float; store short in destination buff          */
    /********************************************************************/
    for (uli=0L; uli<ulSrcCnt; uli++) {
        *lpflDstBuf++ = (float) *lpgsSrcBuf++;        
    }
    return (ulSrcCnt);

}
