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
#include "..\pcmdev\genpcm.h"           /* PCM/APCM conv routine defs   */
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
void    FAR PASCAL  InvEffFFTRea (float FAR *, WORD, short);

/************************************************************************/
/*          Modified short-term transform allocation functions          */
/************************************************************************/
WORD    VocMSTAlo (LPMSTB pmbMSTBlk, DWORD ulFFTPts, DWORD ulWinPts, 
        DWORD ulInpNum, DWORD ulOutDen, DWORD ulSmpFrq, VOCTYP usVocTyp)
{
    float FAR  *lpflWinBuf; 
    float FAR  *lpflPhsBuf; 
    float FAR  *lpflWrkBuf; 
    float       flWinSum = 0;
    DWORD       uli;

    /********************************************************************/
    /* Check / Allocate Data Window buffer                              */
    /********************************************************************/
    if (NULL == (lpflWinBuf = GloAloLck (GMEM_MOVEABLE, &pmbMSTBlk->mhWinBuf, 
      EBSSmp2Bh (FLTPCM032, 1, ulWinPts)))) {
        return ((WORD) -1);
    }

    /********************************************************************/
    /* Calculate Hamming Window                                         */
    /********************************************************************/
    for (uli=0L; uli < ulWinPts; uli++) flWinSum += (lpflWinBuf[uli] = 
        (float) (.54 - .46 * cos (db2PI * uli / (float) (ulWinPts - 1L))));        

    /********************************************************************/
    /* As per Portnoff, et al: if ulWinPts > ulFFTPts, apply sinc win   */
    /********************************************************************/
    if (ulWinPts > ulFFTPts) {
        float flTmp = - (float) (ulWinPts - 1L) / (float) 2;
        for (uli=0L; uli < ulWinPts; uli++, flTmp+=1) if (flTmp != 0) 
            lpflWinBuf[uli] *= (float) ((VOCMSTANA == usVocTyp) 
            ? ulFFTPts * sin (flTmp * dbPI / ulFFTPts) / (flTmp * dbPI)
            : ulOutDen * sin (flTmp * dbPI / ulOutDen) / (flTmp * dbPI));
    }

//flWinSum = 0;
//for (uli=0L; uli < ulWinPts; uli++) flWinSum +=  
//    (float) (.54 - .46 * cos (db2PI * uli / (float) (ulWinPts - 1L)));        
//if (ulWinPts > ulFFTPts) {
//    float flTmp = - (float) (ulWinPts - 1L) / (float) 2;
//    for (uli=0L; uli < ulWinPts; uli++, flTmp+=1) flWinSum += 
//        (float) (.54 - .46 * cos (db2PI * uli / (float) (ulWinPts - 1L)))        
//        * (float) (ulFFTPts * sin (flTmp * dbPI / ulFFTPts) / (flTmp * dbPI));
//}

    /********************************************************************/
    /* Normalize for unity gain; unlock                                 */
    /********************************************************************/
    if (VOCMSTANA == usVocTyp) flWinSum = 1 / flWinSum;
    if ((VOCMSTSYN == usVocTyp) && (ulWinPts <= ulFFTPts)) {
        if (ulOutDen >= ulInpNum) flWinSum /= (float) pow (4, ulInpNum / (float) ulOutDen);
        else flWinSum /= 4 * (float) sqrt (ulInpNum / (float) ulOutDen);
    }
    for (uli=0L; uli < ulWinPts; uli++) lpflWinBuf[uli] *= flWinSum;

//    if ((VOCMSTSYN == usVocTyp) && (ulWinPts <= ulFFTPts)) {
//        flWinSum = 0;
//        for (uli=0L; uli < ulWinPts; uli+=ulOutDen) 
//            flWinSum += lpflWinBuf[uli] * lpflWinBuf[uli];
//        for (uli=0L; uli < ulWinPts; uli++) lpflWinBuf[uli] /= flWinSum;
//    }
    GloMemUnL (pmbMSTBlk->mhWinBuf);

    /********************************************************************/
    /* Check / Allocate last phase diff buffer (for both FWD and INV)   */
    /* Initialize phase difference buffer to zero; unlock               */
    /********************************************************************/
    if (NULL == (lpflPhsBuf = GloAloLck (GMEM_MOVEABLE, &pmbMSTBlk->mhPhsBuf, 
      EBSSmp2Bh (FLTPCM032, 1, MSTDIFPTS(ulFFTPts))))) {
        pmbMSTBlk->mhWinBuf = GloAloRel (pmbMSTBlk->mhWinBuf);
        return ((WORD) -1);
    }
    for (uli=0L; uli < MSTDIFPTS(ulFFTPts); uli++) lpflPhsBuf[uli] = 0;        
    GloMemUnL (pmbMSTBlk->mhPhsBuf);

    /********************************************************************/
    /* Initialize MSTBlk time and phase diff factor parameters          */
    /* Allocate input rotation / output shift buffers                   */
    /********************************************************************/
    if (VOCMSTANA == usVocTyp) {
        pmbMSTBlk->flDifFac = (float) ulSmpFrq / (float) (ulInpNum * db2PI);           
        if (!(pmbMSTBlk->mhWrkBuf = GloAloMem (GMEM_MOVEABLE, 
          EBSSmp2Bh (FLTPCM032, 1, ulFFTPts)))) {
            pmbMSTBlk->mhWinBuf = GloAloRel (pmbMSTBlk->mhWinBuf);
            pmbMSTBlk->mhPhsBuf = GloAloRel (pmbMSTBlk->mhPhsBuf);
            return ((WORD) -1);
        }
    } 
    else {
        pmbMSTBlk->flDifFac = (float) (ulOutDen * db2PI) / (float) ulSmpFrq;           
        if (!(lpflWrkBuf = GloAloLck (GMEM_MOVEABLE, &pmbMSTBlk->mhWrkBuf, 
          EBSSmp2Bh (FLTPCM032, 1, ulWinPts)))) {
            pmbMSTBlk->mhWinBuf = GloAloRel (pmbMSTBlk->mhWinBuf);
            pmbMSTBlk->mhPhsBuf = GloAloRel (pmbMSTBlk->mhPhsBuf);
            return ((WORD) -1);
        }
        for (uli=0L; uli < ulWinPts; uli++) lpflWrkBuf[uli] = 0;        
        GloMemUnL (pmbMSTBlk->mhWrkBuf);
    }
    
    /********************************************************************/
    /********************************************************************/
    return (0);

}

WORD    VocMSTRel (LPMSTB pmbMSTBlk)
{
    /********************************************************************/
    /* Release memory buffers                                           */
    /********************************************************************/
    pmbMSTBlk->mhWinBuf = GloAloRel (pmbMSTBlk->mhWinBuf);
    pmbMSTBlk->mhPhsBuf = GloAloRel (pmbMSTBlk->mhPhsBuf);
    pmbMSTBlk->mhWrkBuf = GloAloRel (pmbMSTBlk->mhWrkBuf);
    return (0);
}

DWORD FAR PASCAL MSTVocAna (float FAR *lpflSrcBuf, DWORD ulInpNum, 
                 long lSmpTim, DWORD ulWinPts, DWORD ulFFTPts, 
                 float flBinFrq, LPMSTB pmbMSTBlk)
{
    float  FAR *lpflWinBuf;
    CPXFLT FAR *lpcxFFTBuf;
    MGAFLT FAR *lpmaMGABuf;
    float  FAR *lpflLstPhs;
    float  FAR *lpflRotBuf;
    DWORD       ulCpxPts = ulFFTPts >> 1;                
    DWORD       uli;                

    /********************************************************************/
    /* Initialize window & rotation buffers & pointers                  */
    /********************************************************************/
    if (!(lpflWinBuf = GloMemLck (pmbMSTBlk->mhWinBuf))) return (0L);
    if (!(lpflRotBuf = GloMemLck (pmbMSTBlk->mhWrkBuf))) {
        GloMemUnL (pmbMSTBlk->mhWinBuf); 
        return (0L);
    }
    for (uli=0; uli < ulFFTPts; uli++) lpflRotBuf[uli] = 0;

    /********************************************************************/
    /* As per Portnoff, et al: window, fold & rotate                    */
    /********************************************************************/
    while (lSmpTim < 0) lSmpTim += ulFFTPts;
    lSmpTim %= ulFFTPts;
    for (uli=0; uli < ulWinPts; uli++) {
        lpflRotBuf[lSmpTim++] += lpflSrcBuf[uli] * lpflWinBuf[uli];
        lSmpTim %= ulFFTPts;
    }
    _fmemcpy (lpflSrcBuf, lpflRotBuf, (WORD) ulFFTPts * sizeof (*lpflRotBuf));
    GloMemUnL (pmbMSTBlk->mhWinBuf); 
    GloMemUnL (pmbMSTBlk->mhWrkBuf);

    /********************************************************************/
    /* Initialize FFT and Phase difference variables                    */
    /********************************************************************/
    if (!(lpflLstPhs = GloMemLck (pmbMSTBlk->mhPhsBuf))) return (0L);
    lpcxFFTBuf = (LPVOID) lpflSrcBuf;
    lpmaMGABuf = (LPVOID) lpflSrcBuf;

    /********************************************************************/
    /* Real FFT size is half of complex.                                */
    /********************************************************************/
//    EffFFTRea (lpflSrcBuf, (WORD) ulCpxPts, EFFFFTFWD);
// Perfect for OBS -2, -2, 1.0 with book shift time
// No effect for OBS -2, -2, 2.0 with book shift time
// Slight improvement(?) or no change for all other cases
InvEffFFTRea (lpflSrcBuf, (WORD) ulCpxPts, EFFFFTFWD);

/********************************************************************/
/********************************************************************/
lpcxFFTBuf[0].flRea = lpcxFFTBuf[0].flImg = 0;

    /********************************************************************/
    /* Unravel rfft-format spectrum; (ulFFTPts / 2) + 1 pairs produced  */
    /********************************************************************/
    for (uli=0; uli <= ulCpxPts; uli++) {
        float   flEnd = lpcxFFTBuf[0].flImg;
        float   flRea;
        float   flImg; 
        float   flDif;
        float   flPhs;

        /****************************************************************/
        /****************************************************************/
        flRea = (ulCpxPts == uli) ? flEnd : lpcxFFTBuf[uli].flRea;       
        flImg = (!uli || (ulCpxPts == uli)) ? 0 : lpcxFFTBuf[uli].flImg;       

        /****************************************************************/
        /* Compute phase & phase 1st order est from real and imag parts */
        /****************************************************************/
        if (0 == (lpmaMGABuf[uli].flMag = (float) _hypot (flRea, flImg))) 
            flDif = 0;
        else {
            // OK for all
            flDif = (flPhs = (float) -atan2 (flImg, flRea)) - lpflLstPhs[uli];
            // OK most, NG for -2, -2, 2.0: 
            // flDif = (flPhs = (float) atan2 (flImg, flRea)) - lpflLstPhs[uli];
            // OK for all
            // flDif = lpflLstPhs[uli] - (flPhs = (float) atan2 (flImg, flRea));
            lpflLstPhs[uli] = flPhs;
            while (flDif >  dbPI) flDif -= (float) db2PI;
            while (flDif < -dbPI) flDif += (float) db2PI;
        }    

        /****************************************************************/
        /* Convert to hertz                                             */
        /****************************************************************/
        lpmaMGABuf[uli].flAng = flDif * pmbMSTBlk->flDifFac + uli * flBinFrq;

    }
    GloMemUnL (pmbMSTBlk->mhPhsBuf); 

    /********************************************************************/
    /********************************************************************/
    return (ulCpxPts + 1L);

}


DWORD FAR PASCAL MSTVocSyn (float FAR *lpflSrcBuf, DWORD ulOutDen,
                 long lSmpTim, DWORD ulWinPts, DWORD ulFFTPts, 
                 float flBinFrq, LPMSTB pmbMSTBlk)
{
    float  FAR *lpflLstPhs;
    CPXFLT FAR *lpcxFFTBuf;
    MGAFLT FAR *lpmaMGABuf;
    float  FAR *lpflWinBuf;
    float  FAR *lpflOutBuf;
    DWORD       ulCpxPts = ulFFTPts >> 1;                
    DWORD       ulNxtOut = (lSmpTim >= 0L) ? ulOutDen : 0L;
    DWORD       uli;                

    /********************************************************************/
    /* Initialize FFT and Phase difference variables                    */
    /********************************************************************/
    if (!(lpflLstPhs = GloMemLck (pmbMSTBlk->mhPhsBuf))) return (0L);
    lpcxFFTBuf = (LPVOID) lpflSrcBuf;
    lpmaMGABuf = (LPVOID) lpflSrcBuf;

    /********************************************************************/
    /* Subtract out frequencies for each channel; convert to complex.   */
    /* Note: Write imaginary before real for shared Cpx/MGA buffer      */
    /********************************************************************/
    for (uli=0; uli <= ulCpxPts; uli++) {
        float   flPhs;
        lpflLstPhs[uli] += lpmaMGABuf[uli].flAng - uli * flBinFrq;         
        flPhs = lpflLstPhs[uli] * pmbMSTBlk->flDifFac;
//}
//PreWrpSpc (lpflSrcBuf, ulCpxPts, 1, mhWrpBuf);
//for (uli=0; uli <= ulCpxPts; uli++) {
//float flPhs;
//flPhs = lpflLstPhs[uli] * pmbMSTBlk->flDifFac;
        if (ulCpxPts != uli) {
            lpcxFFTBuf[uli].flImg = -lpmaMGABuf[uli].flMag * (float) sin (flPhs);
            lpcxFFTBuf[uli].flRea = +lpmaMGABuf[uli].flMag * (float) cos (flPhs);
        }
        else lpcxFFTBuf[0].flImg  = +lpmaMGABuf[uli].flMag * (float) cos (flPhs);
    }
    GloMemUnL (pmbMSTBlk->mhPhsBuf); 

//lpcxFFTBuf[0].flImg = lpcxFFTBuf[0].flImg = 0;
//lpcxFFTBuf[ulCpxPts].flImg = lpcxFFTBuf[ulCpxPts].flImg = 0;

    /********************************************************************/
    /* Real FFT size is half of complex.                                */
    /********************************************************************/
    InvEffFFTRea (lpflSrcBuf, (WORD) ulCpxPts, EFFFFTINV);

    /********************************************************************/
    /* Initialize destination and data window variables                 */
    /********************************************************************/
    if (!(lpflWinBuf = GloMemLck (pmbMSTBlk->mhWinBuf))) return (0L);
    if (!(lpflOutBuf = GloMemLck (pmbMSTBlk->mhWrkBuf))) {
        GloMemUnL (pmbMSTBlk->mhWinBuf); 
        return (0L);
    }
    
    /********************************************************************/
    /* As per Portnoff, et al: un-window, un-fold & un-rotate           */
    /********************************************************************/
    while (lSmpTim < 0) lSmpTim += ulFFTPts;
    lSmpTim %= ulFFTPts;
    for (uli=0L; uli < ulWinPts; uli++) {
        lpflOutBuf[uli] += lpflSrcBuf[lSmpTim++] * lpflWinBuf[uli];
        lSmpTim %= ulFFTPts;
    }
    GloMemUnL (pmbMSTBlk->mhWinBuf); 

    /********************************************************************/
    /* If output time >= 0, output first batch & shift remaining left   */
    /********************************************************************/
    if (ulNxtOut) _fmemcpy (lpflSrcBuf, lpflOutBuf, 
        (WORD) ulOutDen * sizeof (*lpflOutBuf));
    _fmemmove (lpflOutBuf, &lpflOutBuf[ulOutDen], 
        (WORD) (ulWinPts - ulOutDen) * sizeof (*lpflOutBuf));          
    for (uli=ulWinPts - ulOutDen; uli<ulWinPts; uli++) lpflOutBuf[uli] = 0;        
    GloMemUnL (pmbMSTBlk->mhWrkBuf); 

    /********************************************************************/
    /********************************************************************/
    return (ulNxtOut);

}

/************************************************************************/
/************************************************************************/
WORD PreWrpSpc(float FAR *, long, float, VISMEMHDL);
void PhsToFrq (float FAR *buf, long size, float incr, float sampRate);
void FrqToPhs (float FAR *buf, long size, float incr, float sampRate);

#define MMmaskPhs(p,q,r,s) /* p is pha, q local, r is PI, s is 1/PI */  \
    q = (int)(s*p);                                                     \
    p -= r*(float)((int)((q+((q>=0)?(q&1):-(q&1) ))));                  \

#define actual(s)    ((s-1L)*2L)
    /* if callers are passing whole frame size, make this (s) */
    /* where callers pass s/2+1, this recreates the parent fft frame size */

/************************************************************************/
/* spectral envelope detection: this is a crude peak picking algorithm  */
/*    which is used to detect and pre-warp the spectral envelope so that  */
/*    pitch transposition can be performed without altering timbre.       */
/*    The basic idea is to disallow large negative slopes between         */
/*    successive values of magnitude vs. frequency.                       */
/************************************************************************/
//WORD PreWrpSpc (float FAR *lpflSrcBuf, long lCpxPts, float flWrpFac, VISMEMHDL mhWrkBuf)
//{
////  lpflSrcBuf;             /* spectrum as magnitude, phase             */
////  lCpxPts;                /* full frame size, tho' we only use n/2+1  */
////  flWrpFac;               /* How much pitches are being multd by      */
//
//    float FAR  *lpflEnvBuf;
//    float       eps, slope;
//    float       flCurMag, flLstMag, flNxtMag, flOldMax;
//    long        lMaxCnt, li, lj;
//
//    /********************************************************************/
//    /********************************************************************/
//    if (!(lpflEnvBuf = GloMemLck (mhWrkBuf))) return ((WORD) -1);
//
//    /********************************************************************/
//    /********************************************************************/
//    eps = -64 / (float) lCpxPts;    /* for spectral envelope estimation */
//    flLstMag = *lpflSrcBuf;
//    flCurMag = lpflSrcBuf[2*1];
//    flOldMax = flLstMag;
//    *lpflEnvBuf = flOldMax;
//    lMaxCnt = 1;
//  
//    /********************************************************************/
//    /* Step thru spectrum                                               */
//    /********************************************************************/
//    for (li=1; li < lCpxPts; li++) 
//    {
//        if (li < lCpxPts-1) flNxtMag = lpflSrcBuf[2*(li+1)];
//        else flNxtMag = 0;
//  
//        if (flOldMax != 0) 
//             slope = (flCurMag - flOldMax) / (flOldMax * lMaxCnt);
//        else slope = -10;
//  
//        /****************************************************************/
//        /* Look for peaks                                               */
//        /****************************************************************/
//        if ((flCurMag>=flLstMag)&&(flCurMag>flNxtMag)&&(slope>eps))
//        {
//            lpflEnvBuf[li] = flCurMag;
//            lMaxCnt--;
//            for (lj = 1; lj <= lMaxCnt; lj++) {
//                lpflEnvBuf[li - lMaxCnt + lj - 1] = flOldMax * (1 + slope * lj);
//            }
//            flOldMax = flCurMag;
//            lMaxCnt = 1;
//        }
//        else lMaxCnt++;                             /* not a peak       */
//  
//        flLstMag = flCurMag;
//        flCurMag = flNxtMag;
//    }
//  
//    /********************************************************************/
//    /* Get final peak                                                   */
//    /********************************************************************/
//    if (lMaxCnt > 1) {
//        flCurMag = lpflSrcBuf[2 * (lCpxPts / 2)];
//        slope = (flCurMag - flOldMax) / lMaxCnt;
//        lpflEnvBuf[lCpxPts / 2] = flCurMag;
//        lMaxCnt--;
//        for (lj=1; lj <= lMaxCnt; lj++) {
//            lpflEnvBuf[lCpxPts/2 - lMaxCnt + lj - 1] = flOldMax + slope * lj;
//        }
//    }
//  
//    /********************************************************************/
//    /* Warp spectral envelope                                           */
//    /********************************************************************/
//    for (li = 0; li < lCpxPts; li++) { 
//        lj = (long) (li * flWrpFac);
//        flCurMag = lpflSrcBuf[2*li];
//        if ((lj < lCpxPts) && (lpflEnvBuf[li] != 0.))
//             lpflSrcBuf[2 * li] *= lpflEnvBuf[lj]/lpflEnvBuf[li];
//        else lpflSrcBuf[2 * li] = 0;
//    }
//
//    /********************************************************************/
//    /********************************************************************/
//    GloMemUnL (mhWrkBuf); 
//
//    /********************************************************************/
//    /********************************************************************/
//    return (0);
//}
//
///* Convert a batch of phase differences to actual target freqs */
//void PhsToFrq (float FAR *buf, long size, float incr, float sampRate)
//{
//    long    i;
//    float FAR *pha, p, q, oneOnPi;
//    int     z;
//    float   flDifFac, flCtrFrq, flBinFrq;
//    float   flExpDltPhs, flDltPhsInc;
//
//    pha = buf + 1;
//    flDifFac = sampRate/(2*(float)dbPI*incr);
//    flBinFrq = sampRate/((float)actual(size));
//    flCtrFrq = 0;
//    /********************************************************************/
//    /* We get some phase shift with spot-on frq coz time shift          */
//    /********************************************************************/
//    flExpDltPhs = 0;
//    flDltPhsInc = (float) db2PI * incr / ((float)actual(size));
//    oneOnPi = 1/(float)dbPI;
//    for(i=0; i<size; ++i) {
//        q = p = pha[2L*i] - flExpDltPhs;
//        MMmaskPhs (p, z, (float)dbPI, oneOnPi);
//        pha[2L*i] = p;
//        pha[2L*i] *= flDifFac;
//        pha[2L*i] += flCtrFrq;
//
//        flExpDltPhs += flDltPhsInc;
//        flExpDltPhs -= (float)db2PI*(float)((int)(flExpDltPhs*oneOnPi)); 
//        flCtrFrq += flBinFrq;
//    }
//}
//
///* Undo a pile of frequencies back into phase differences */
//void FrqToPhs (float FAR *buf, long size, float incr, float sampRate)
//{
//    float   fixUp = 0;
//    float FAR *pha;
//    float   twoPiOnSr, flCtrFrq, flBinFrq;
//    float   flExpDltPhs,flDltPhsInc;
//    register float  p;
//    register long   i;
//    register int    j;
//    float   oneOnPi;
//
//    oneOnPi = 1/(float)dbPI;
//    pha = buf + 1;
//    twoPiOnSr = (float)db2PI*((float)incr)/sampRate;
//    flBinFrq = sampRate/((float)actual(size));
//    flCtrFrq = 0;
//    /********************************************************************/
//    /* We get some phase shift with spot-on frq coz time shift          */
//    /********************************************************************/
//    flExpDltPhs = 0;
//    flDltPhsInc = 2*(float)dbPI*((incr)/((float)actual(size)) + fixUp);
//    for(i=0; i<size; ++i)
//    {
//    p = pha[2L*i];
//    p -= flCtrFrq;
//    p *= twoPiOnSr;
//    p += flExpDltPhs;
//        MMmaskPhs(p,j,(float)dbPI,oneOnPi);
//    /* MmaskPhs(p);    */
//    pha[2L*i] = p;
//    flExpDltPhs += flDltPhsInc;
//    flExpDltPhs -= (float)db2PI*(float)((int)(flExpDltPhs*oneOnPi)); 
//    flCtrFrq += flBinFrq;
//    }
//}

/************************************************************************/
/************************************************************************/
void    FAR PASCAL  InvEffFFTRea (float FAR *lpflSrcBuf, WORD usCpxPts,  short sSgn)
{
    WORD        usi;
    CPXFLT FAR *lpcxSrcBuf = (CPXFLT FAR *) lpflSrcBuf;

    // Perfect for OBS -2, -2, 1.0 with book shift time
    // No effect for OBS -2, -2, 2.0 with book shift time
    // Slight improvement(?) or no change for all other cases
    /********************************************************************/
    /* Error in FFT algorithms? Sign inversion occuring?                */
    /********************************************************************/
    if (EFFFFTFWD == sSgn) EffFFTRea (lpflSrcBuf, usCpxPts, EFFFFTFWD);
    for (usi=1; usi <= usCpxPts; usi+=2) {                  /* Good!    */
        lpcxSrcBuf[usi].flRea = -lpcxSrcBuf[usi].flRea;
        lpcxSrcBuf[usi].flImg = -lpcxSrcBuf[usi].flImg;
    }
    if (EFFFFTINV == sSgn) EffFFTRea (lpflSrcBuf, usCpxPts, EFFFFTINV);

}

