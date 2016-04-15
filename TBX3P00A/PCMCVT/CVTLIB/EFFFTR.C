/************************************************************************/
/* Filter Effects: EffFtr.c                             V2.00  08/15/95 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#if (defined (W31)) /****************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#include "..\..\os_dev\winmem.h"        /* Generic memory supp defs     */
#include "..\..\os_dev\winmsg.h"        /* User message support defs    */
#endif /*****************************************************************/
#if (defined (DOS)) /****************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "..\..\os_dev\dosmem.h"        /* Generic memory support defs  */
#include "..\..\os_dev\dosmsg.h"        /* User message support defs    */
#endif /*****************************************************************/
#include "..\..\fiodev\filutl.h"        /* File utilities definitions   */
#include "..\..\effdev\geneff.h"        /* Sound Effects definitions    */
#include "gencvt.h"                     /* PCM conversion defs          */
#include "libsup.h"                     /* PCM conversion lib supp      */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */
#include <math.h>                       /* Math library defs            */

/************************************************************************/
/*                          External References                         */
/************************************************************************/
extern  TBXGLO  TBxGlo;

/************************************************************************/
/************************************************************************/
#define NCHFTRMAX   32
typedef struct _NCHFTR {                /* Notch filter frequencies     */
    WORD    usFrqCnt;
    DWORD   ulNchFrq[NCHFTRMAX];
} NCHFTR;   

/************************************************************************/
/************************************************************************/
WORD    FtrFFTAAF (DWORD, DWORD, VISMEMHDL, DWORD);
WORD    FtrFFTDTM (DWORD, VISMEMHDL, DWORD);
void    CpxMul (CPXFLT, CPXFLT, CPXFLT FAR *);

/************************************************************************/
/************************************************************************/
WORD    FFTFtrAlo (CVTBLK FAR *lpCvtBlk, FFTBLK FAR *lpFFTBlk, 
        EFFPOLPRC lpPolPrc, DWORD ulPolDat)
{
    float   flResRat;
    LPCPXF  lpXfrBuf;

    /********************************************************************/
    /********************************************************************/
    _fmemset (lpFFTBlk, 0, sizeof (*lpFFTBlk));

    /********************************************************************/
    /* Any frequency domain filtering requested?                        */
    /********************************************************************/
    if (!(lpCvtBlk->bfFFTDTM || lpCvtBlk->bfFFTAAF || lpCvtBlk->bfFFTFEq 
        || lpCvtBlk->bfFFTRes)) return (0);

    /********************************************************************/
    /* Initialize FFT block                                             */
    /********************************************************************/
    flResRat = !lpCvtBlk->bfFFTRes ? 1 : lpCvtBlk->ulDstFrq / (float) lpCvtBlk->ulSrcFrq;
    if (EffFFTAlo (lpCvtBlk->usFFTOrd, flResRat, lpFFTBlk, lpPolPrc, ulPolDat)) {
        CVTERRMSG ("Error setting FFT filter parameters.\n");
        return ((WORD) -1);
    }

//    /********************************************************************/
//    /* Lock data window memory, initialize								*/
//    /* Note: Data window is twice the FFT point size - overlap saved	*/
//    /********************************************************************/
//    if (NULL != (lpWinBuf = (float FAR *) GloMemLck (m_fbFFTBlk.mhWinHdl))) {
//        EffWinGen (EFFWINTAP, lpWinBuf, 2 * m_fbFFTBlk.ulFFTPts);
//        GloMemUnL (m_fbFFTBlk.mhWinHdl);
//    }

    /********************************************************************/
    /* Initialize equalizer-designed transfer function                  */
    /********************************************************************/
    if (lpCvtBlk->bfFFTFEq &&
      EffEquAlo (lpCvtBlk->ulFEqBot, lpCvtBlk->ulFEqTop, lpCvtBlk->ulSrcFrq, 
      lpCvtBlk->usFEqCnt, lpCvtBlk->flFEqPnt, lpCvtBlk->flFEqGai, lpFFTBlk)) {
        CVTERRMSG ("Error setting equalizer filter parameters.\n");
        return ((WORD) -1);
    }

    /********************************************************************/
    /* Generate Anti-aliasing transfer fucntion                         */
    /********************************************************************/
    if (lpCvtBlk->bfFFTAAF &&
      FtrFFTAAF (lpCvtBlk->ulDstFrq / 2L, lpCvtBlk->ulSrcFrq, lpFFTBlk->mhXfrHdl,
      lpFFTBlk->ulFFTPts)) {
        CVTERRMSG ("Error setting anti-aliasing filter parameters.\n");
        return ((WORD) -1);
    }

    /********************************************************************/
    /* Generate DTMF notch filter transfer function                     */
    /********************************************************************/
    if (lpCvtBlk->bfFFTDTM && FtrFFTDTM (lpCvtBlk->ulSrcFrq, lpFFTBlk->mhXfrHdl,
     lpFFTBlk->ulFFTPts)) {
        CVTERRMSG ("Error setting DTMF notch filter parameters.\n");
        return ((WORD) -1);
    }

    /********************************************************************/
    /* Convert infinite length transfer functions into valid frequency  */
    /* domain filter. Lock FFT filter transfer buffer & window.         */
    /********************************************************************/
    if (NULL != (lpXfrBuf = GloMemLck (lpFFTBlk->mhXfrHdl))) {
        EffWinRsp (EFFWINHAM, lpXfrBuf, lpFFTBlk->ulFFTPts); 
        GloMemUnL (lpFFTBlk->mhXfrHdl);
    }

    /********************************************************************/
    /********************************************************************/
    return (0);

}

WORD    FFTFtrRel (FFTBLK FAR *lpFFTBlk)
{
    /********************************************************************/
    /********************************************************************/
    EffEquRel (lpFFTBlk);
    EffFFTRel (lpFFTBlk);
    return (0);
}

/************************************************************************/
/*                  Anti-aliasing low-pass filter                       */
/************************************************************************/
WORD    FtrFFTAAF (DWORD ulLowPas, DWORD ulSmpFrq, VISMEMHDL mhXfrHdl, 
        DWORD ulFFTPts)
{
    LPCPXF  lpXfrBuf;
    DWORD   ulCutOff;
    DWORD   uli;

    /********************************************************************/
    /* Back frequency off by 90%                                        */    
    /********************************************************************/
    ulLowPas = (DWORD) (ulLowPas * .9);

    /********************************************************************/
    /* Is cut-off frequency at or beyond Nyquist?                       */
    /********************************************************************/
    if (ulLowPas >= ulSmpFrq/2L) return (0);

    /********************************************************************/
    /* Lock FFT filter transfer buffer                                  */
    /********************************************************************/
    if (NULL == (lpXfrBuf = GloMemLck (mhXfrHdl))) {
        return ((WORD) -1);
    }

    /********************************************************************/
    /* Handle DC and N/2 as special case; pack into real FFT format     */
    /* Calculate complex cutoff frequency bin                           */
    /* Zero complex transfer function above cut-off frequency           */
    /********************************************************************/
    lpXfrBuf[0].flRea = lpXfrBuf[0].flImg = 0;
    ulCutOff = (ulFFTPts * ulLowPas) / (ulSmpFrq / 2L);
    for (uli=ulCutOff; uli < ulFFTPts; uli++) {
        lpXfrBuf[uli].flRea = 0;       
        lpXfrBuf[uli].flImg = 0;
    }

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (mhXfrHdl);
    return (0);

}

/************************************************************************/
/*                      DTMF tone notch filter                          */
/************************************************************************/
WORD    FtrFFTDTM (DWORD ulSmpFrq, VISMEMHDL mhXfrHdl, DWORD ulFFTPts)
{
    #define HAMWIN(s,p) (0.54-0.46*cos((double)(p*2)*dbPI/((double)s-1.0)))
    NCHFTR  nfDTMF = {8, 697, 770, 852, 941, 1209, 1336, 1477, 1633};
    LPCPXF  lpXfrBuf;
    WORD    usi;
    
    /********************************************************************/
    /* Lock FFT filter transfer buffer                                  */
    /********************************************************************/
    if (NULL == (lpXfrBuf = GloMemLck (mhXfrHdl))) {
        return ((WORD) -1);
    }

    /********************************************************************/
    /* Zero complex transfer function at notch frequency                */
    /********************************************************************/
    for (usi = 0; usi < nfDTMF.usFrqCnt; usi++) {
        if (nfDTMF.ulNchFrq[usi] < ulSmpFrq/2L) {
            DWORD ulFrqBin = (ulFFTPts * nfDTMF.ulNchFrq[usi]) / (ulSmpFrq/2L);
            if (ulFrqBin >= 2) {
                lpXfrBuf[ulFrqBin-2].flRea *= (float) (1 - HAMWIN (7, 1));
                lpXfrBuf[ulFrqBin-2].flImg *= (float) (1 - HAMWIN (7, 1));
            }
            if (ulFrqBin >= 1) {
                lpXfrBuf[ulFrqBin-1].flRea *= (float) (1 - HAMWIN (7, 2));
                lpXfrBuf[ulFrqBin-1].flImg *= (float) (1 - HAMWIN (7, 2));
            }
            lpXfrBuf[ulFrqBin].flRea = 0;
            lpXfrBuf[ulFrqBin].flImg = 0;
            if ((ulFrqBin+1) < ulFFTPts) {
                lpXfrBuf[ulFrqBin+1].flRea *= (float) (1 - HAMWIN (7, 4));
                lpXfrBuf[ulFrqBin+1].flImg *= (float) (1 - HAMWIN (7, 4));
            }
            if ((ulFrqBin+2) < ulFFTPts) {
                lpXfrBuf[ulFrqBin+2].flRea *= (float) (1 - HAMWIN (7, 5));
                lpXfrBuf[ulFrqBin+2].flImg *= (float) (1 - HAMWIN (7, 5));
            }
        }
    }

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (mhXfrHdl);
    return (0);

}

/////////////////////////////////////////////////////////////////////////////
//	Note: MSC 8.0 bug when compiling with Compiler Optimizations = Max Speed
//	Equalizer generates zeros for output data. Is this related
//	to ToolBox 2.00d EffFtr.c MSC 8.0 -Ocglt bug?
/////////////////////////////////////////////////////////////////////////////
#pragma optimize("", off)
/************************************************************************/
/*                  Frequency Domain Filter Routine                     */
/************************************************************************/
WORD FAR PASCAL FFTFtrFnc (float FAR * FAR *ppflSrcBuf, DWORD FAR *lpulDstCnt,
                DWORD ulBufPts, DWORD ulTotPts, DWORD ulHdrPts, DWORD ulCtrPts, 
                DWORD ulDstRem, LPFFTB lpFFTBlk)
{
    CVTBLK  FAR *lpCvtBlk = lpFFTBlk->lpUsrDat;   
    CPXFLT  FAR *lpFFTCpx = (CPXFLT FAR *)(*ppflSrcBuf);
    CPXFLT  FAR *lpXfrBuf;
    DWORD       uli;

    /********************************************************************/
    /* The EffFFTFtr (caller) delivers a set of data points where       */
    /* ulCtrPts at offset ulHdrPts have been isolated to implement      */
    /* the overlap-save method of FFT corruption prevention.            */
    /********************************************************************/
    
    /********************************************************************/
    /* Default returned data count to zero                              */
    /********************************************************************/
    *lpulDstCnt = 0L;

    /********************************************************************/
    /* Completion pass? Indicate done processing...                     */
    /********************************************************************/
    if (!ulCtrPts) return (FALSE);

    /********************************************************************/
    /* Space available in output? Enough room for resampling?           */
    /********************************************************************/
    if (ulDstRem < ulCtrPts * lpFFTBlk->flResRat) return (FALSE);
    if (ulBufPts < ulTotPts * lpFFTBlk->flResRat) return (FALSE);

//    /********************************************************************/
//    /* Window incoming data                                             */
//    /********************************************************************/
//    if (NULL != (lpWinBuf = (float FAR *) GloMemLck (lpFFTBlk->mhWinHdl))) {
//      for (uli=0; uli < ulTotPts; uli++) (*ppflSrcBuf)[uli] *= lpWinBuf[uli];
//      GloMemUnL (lpFFTBlk->mhWinHdl);
//    }

    /********************************************************************/
    /* Transform to frequency domain                                    */
    /********************************************************************/
    EffFFTRea (*ppflSrcBuf, (WORD) (ulTotPts / 2L), EFFFFTFWD);

    /********************************************************************/
    /* Lock FFT filter transfer buffer                                  */
    /********************************************************************/
    if (NULL == (lpXfrBuf = (CPXFLT FAR *) GloMemLck (lpFFTBlk->mhXfrHdl))) 
        return (FALSE);

    /********************************************************************/
    /* Handle DC & Nyquist as special case; pack into real data FFT fmt */
    /********************************************************************/
    lpFFTCpx[0].flRea *= lpXfrBuf[0].flRea;
    lpFFTCpx[0].flImg *= lpXfrBuf[0].flImg;

    /********************************************************************/
    /* Apply filter function, unlock filter transfer buffer when done   */
    /********************************************************************/
    for (uli = 1L; uli < ulTotPts / 2L; uli++) {
        CpxMul (lpFFTCpx[uli], lpXfrBuf[uli], &lpFFTCpx[uli]);
    }
    GloMemUnL (lpFFTBlk->mhXfrHdl);

    /********************************************************************/
    /* Pad with zero if required; transform back to time domain         */
    /********************************************************************/
    for (uli=ulTotPts; uli < ulBufPts; uli++) (*ppflSrcBuf)[uli] = (float) 0;
    EffFFTRea (*ppflSrcBuf, (WORD) ((ulTotPts * lpFFTBlk->flResRat) / 2), EFFFFTINV);
    //
    //  Note: MSC 8.0 bug when compiling DLL with -Ocglt: 
    //  #pragma optimize("", off) has no effect
    //  0 == (WORD) ((ulTotPts * lpFFTBlk->flResRat) / 2)
    //  0 == (WORD) ((lpFFTBlk->flResRat * ulTotPts) / 2)
    //  0 == (WORD) (ulTotPts * (lpFFTBlk->flResRat / 2.))
    //  0 == (WORD) ((double) ulTotPts * ((double) lpFFTBlk->flResRat / 2.))
    //  WORD usTmp = (WORD) ((lpFFTBlk->flResRat * ulTotPts) / 2); // == 0
    //  Note: Appears to only effect the DLL version, not static lib compile
    //

    /********************************************************************/
    /* Set Output buffer to point to good samples (past header)         */
    /********************************************************************/
    *ppflSrcBuf = &(*ppflSrcBuf)[(DWORD) (ulHdrPts * lpFFTBlk->flResRat)];

    /********************************************************************/
    /* Indicate number of valid output samples and continue processing  */
    /********************************************************************/
    *lpulDstCnt = (DWORD) (ulCtrPts * lpFFTBlk->flResRat);

//    /********************************************************************/
//    /* Correct energy level for re-sample                               */
//    /********************************************************************/
//    if (1 != lpFFTBlk->flResRat) for (uli=0; uli < ulCtrPts * lpFFTBlk->flResRat; uli++) 
//        (*ppflSrcBuf)[uli] *= lpFFTBlk->flResRat;

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);

}

/////////////////////////////////////////////////////////////////////////////
//	End MSC 8.0 bug
/////////////////////////////////////////////////////////////////////////////
#pragma optimize("", on)

void CpxMul (CPXFLT a, CPXFLT b, CPXFLT FAR *pc)
{
    pc->flRea = a.flRea*b.flRea - a.flImg*b.flImg;
    pc->flImg = a.flImg*b.flRea + a.flRea*b.flImg;
}

