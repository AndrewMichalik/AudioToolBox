/************************************************************************/
/* Effects Freq shift Osc Bank Syn: EffOBS.c            V2.00  03/15/95 */
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
#define OBSPRVPTS(x)    ((x/2L)+1L)     /* OBS previous mag/amp cnt     */      
#define OBSIDXPTS(x)    (x+1L)          /* OBS previous index count     */      
#define OBSCOSTAB       8192L           /* OBS cosine table length      */      

/************************************************************************/
/*          Oscillator Bank Synthesis allocation functions              */
/************************************************************************/
WORD    VocOBSAlo (LPOBSB pobOBSBlk, DWORD ulFFTPts, DWORD ulWinPts, 
        DWORD ulInpNum, DWORD ulOutDen, DWORD ulSmpFrq)
{
    MGAFLT FAR *lpmaPrvBuf;
    float  FAR *lpflCosBuf;
    float  FAR *lpflIdxBuf;
    float  FAR *lpflWrkBuf;
    DWORD       ulCpxPts = ulFFTPts >> 1;                
    DWORD       uli;

    /********************************************************************/
    /* Check / Allocate Previous Mag / Ang buffer                       */
    /********************************************************************/
    if (NULL == (lpmaPrvBuf = GloAloLck (GMEM_MOVEABLE, &pobOBSBlk->mhPrvBuf, 
      EBSSmp2Bh (CPXPCM064, 1, OBSPRVPTS(ulFFTPts))))) {
        return ((WORD) -1);
    }
    for (uli=0L; uli < OBSPRVPTS(ulFFTPts); uli++) 
        lpmaPrvBuf[uli].flMag = lpmaPrvBuf[uli].flAng  = 0;
    GloMemUnL (pobOBSBlk->mhPrvBuf);

    /********************************************************************/
    /* Check / Allocate Cosine table buffer                             */
    /********************************************************************/
    if (NULL == (lpflCosBuf = GloAloLck (GMEM_MOVEABLE, &pobOBSBlk->mhCosBuf, 
      EBSSmp2Bh (FLTPCM032, 1, OBSCOSTAB)))) {
        pobOBSBlk->mhPrvBuf = GloAloRel (pobOBSBlk->mhPrvBuf);
        return ((WORD) -1);
    }
    for (uli=0L; uli < OBSCOSTAB; uli++) 
        lpflCosBuf[uli] = (float) cos (db2PI * uli / (float) OBSCOSTAB);
    GloMemUnL (pobOBSBlk->mhCosBuf);

    pobOBSBlk->flTabInc = OBSCOSTAB / (float) ulSmpFrq;

    /********************************************************************/
    /* Check / Allocate Index previous address buffer                   */
    /********************************************************************/
    if (NULL == (lpflIdxBuf = GloAloLck (GMEM_MOVEABLE, &pobOBSBlk->mhIdxBuf, 
      EBSSmp2Bh (FLTPCM032, 1, OBSIDXPTS(ulFFTPts))))) {
        pobOBSBlk->mhPrvBuf = GloAloRel (pobOBSBlk->mhPrvBuf);
        pobOBSBlk->mhCosBuf = GloAloRel (pobOBSBlk->mhCosBuf);
        return ((WORD) -1);
    }
    for (uli=0L; uli < OBSIDXPTS(ulFFTPts); uli++) lpflIdxBuf[uli] = 0;        
    GloMemUnL (pobOBSBlk->mhIdxBuf);

    /********************************************************************/
    /********************************************************************/
    if (!(lpflWrkBuf = GloAloLck (GMEM_MOVEABLE, &pobOBSBlk->mhWrkBuf, 
      EBSSmp2Bh (FLTPCM032, 1, ulWinPts)))) {
        pobOBSBlk->mhPrvBuf = GloAloRel (pobOBSBlk->mhPrvBuf);
        pobOBSBlk->mhCosBuf = GloAloRel (pobOBSBlk->mhCosBuf);
        pobOBSBlk->mhIdxBuf = GloAloRel (pobOBSBlk->mhIdxBuf);
        return ((WORD) -1);
    }
    for (uli=0L; uli < ulWinPts; uli++) lpflWrkBuf[uli] = 0;        
    GloMemUnL (pobOBSBlk->mhWrkBuf);

    /********************************************************************/
    /********************************************************************/
    return (0);
}

WORD    VocOBSRel (LPOBSB pobOBSBlk)
{
    pobOBSBlk->mhPrvBuf = GloAloRel (pobOBSBlk->mhPrvBuf);
    pobOBSBlk->mhCosBuf = GloAloRel (pobOBSBlk->mhCosBuf);
    pobOBSBlk->mhWrkBuf = GloAloRel (pobOBSBlk->mhWrkBuf);
    return (0);
}

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL OBSVocSyn (float FAR *lpflSrcBuf, DWORD ulOutDen, 
                 long lSmpTim, DWORD ulWinPts, DWORD ulFFTPts, 
                 float flPchShf, float flSynThr, LPOBSB pobOBSBlk)
{
    MGAFLT FAR *lpmaMGABuf;
    MGAFLT FAR *lpmaPrvBuf;
    float  FAR *lpflCosBuf;
    float  FAR *lpflIdxBuf;
    float  FAR *lpflOutBuf;
    DWORD       ulOscCnt;
    float       flPchInc;
    float       flFrqInc;
    float       flAmpInc;
    float       flCurAmp;
    float       flCurFrq;
    float       flCurIdx;
    DWORD       ulCpxPts = ulFFTPts >> 1;                
    DWORD       uli;
    DWORD       ulj;

    /********************************************************************/
    /********************************************************************/
    ulOscCnt = (DWORD) ((flPchShf > 1) ? ulCpxPts / flPchShf : ulCpxPts);
    flPchInc =  flPchShf * pobOBSBlk->flTabInc;

    /********************************************************************/
    /********************************************************************/
    if (!(lpmaPrvBuf = GloMemLck (pobOBSBlk->mhPrvBuf))) {
        return (0L);
    }
    if (!(lpflCosBuf = GloMemLck (pobOBSBlk->mhCosBuf))) {
        GloMemUnL (pobOBSBlk->mhPrvBuf);
        return (0L);
    }
    if (!(lpflIdxBuf = GloMemLck (pobOBSBlk->mhIdxBuf))) {
        GloMemUnL (pobOBSBlk->mhPrvBuf);
        GloMemUnL (pobOBSBlk->mhCosBuf);
        return (0L);
    }
    if (!(lpflOutBuf = GloMemLck (pobOBSBlk->mhWrkBuf))) {
        GloMemUnL (pobOBSBlk->mhPrvBuf);
        GloMemUnL (pobOBSBlk->mhCosBuf);
        GloMemUnL (pobOBSBlk->mhIdxBuf);
        return (0L);
    }
    lpmaMGABuf = (LPVOID) lpflSrcBuf;

    /********************************************************************/
    /* Generate I output samples from input Mag / Ang buffer            */
    /********************************************************************/
    for (uli=0L; uli < ulOscCnt; uli++) {
        lpmaMGABuf[uli].flAng *= flPchInc;
        flCurFrq = lpmaPrvBuf[uli].flAng;
        flFrqInc = (lpmaMGABuf[uli].flAng - flCurFrq) / (float) ulOutDen;
        flCurIdx = lpflIdxBuf[uli];

        /****************************************************************/
        /****************************************************************/
        if (lpmaMGABuf[uli].flMag < flSynThr) lpmaMGABuf[uli].flMag = 0;
        flCurAmp = lpmaPrvBuf[uli].flMag;
        flAmpInc = (lpmaMGABuf[uli].flMag - flCurAmp) / (float) ulOutDen;

        /****************************************************************/
        /* Inc Frq / Amp from beginning to end of output                */
        /****************************************************************/
        if (flAmpInc || flCurAmp) for (ulj=0; ulj < ulOutDen; ulj++) {
            // lpflOutBuf[ulj] += flCurAmp * cos (db2PI * flCurIdx / OBSCOSTAB);           
            lpflOutBuf[ulj] += flCurAmp * lpflCosBuf[(DWORD) flCurIdx];           
            flCurAmp += flAmpInc;
            flCurIdx += (flCurFrq + ulj * flFrqInc);
            while (flCurIdx >= OBSCOSTAB) flCurIdx -= (float) OBSCOSTAB;
            while (flCurIdx <          0) flCurIdx += (float) OBSCOSTAB;
        }
        lpmaPrvBuf[uli] = lpmaMGABuf[uli];
        lpflIdxBuf[uli] = flCurIdx;

    }

    /********************************************************************/
    /* If output time >= 0, output first batch & shift remaining left   */
    /********************************************************************/
    if (lSmpTim) _fmemcpy (lpflSrcBuf, lpflOutBuf, 
        (WORD) ulOutDen * sizeof (*lpflOutBuf));
    _fmemmove (lpflOutBuf, &lpflOutBuf[ulOutDen], 
        (WORD) (ulWinPts - ulOutDen) * sizeof (*lpflOutBuf));          
    for (uli=ulWinPts - ulOutDen; uli<ulWinPts; uli++) lpflOutBuf[uli] = 0;        

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (pobOBSBlk->mhPrvBuf);
    GloMemUnL (pobOBSBlk->mhCosBuf);
    GloMemUnL (pobOBSBlk->mhIdxBuf);
    GloMemUnL (pobOBSBlk->mhWrkBuf); 

    /********************************************************************/
    /********************************************************************/
    return ((lSmpTim >= 0L) ? ulOutDen : 0L);

}
