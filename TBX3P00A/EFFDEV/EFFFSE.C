/************************************************************************/
/* Effects File Sound Event Support: EffFSE.c           V2.00  03/15/95 */
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
#include "..\fiodev\filutl.h"           /* File utilities definitions   */
#include "geneff.h"                     /* Sound Effects definitions    */
#include "effsup.h"                     /* Sound Effects definitions    */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation          */
#include <stdio.h>                      /* Standard I/O                 */
#include <math.h>                       /* Math library defs            */

/************************************************************************/
/************************************************************************/
FSEEVT  ScnSndEvt (LPGSMP, DWORD FAR *, WORD FAR *, DWORD FAR *, LPFSEB);
DWORD   ScnFrmBuf (WORD FAR *, DWORD, DWORD, BOOL, DWORD); 

WORD    EffF2PAloHghPas (DWORD, DWORD, BOOL, struct _F2PBLK FAR *);
DWORD   FtrF2PBlkHghPas (LPGSMP, DWORD, struct _F2PBLK FAR *);

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL EffFSEAlo (float flFrmTim, float flResTim, float flSndThr, 
                float flAtkRat, float flDcyRat, float flAtkGrd, float flDcyGrd,
                DWORD ulHghPas, PCMTYP usPCMTyp, WORD usChnCnt, WORD usBIOEnc, 
                DWORD ulSmpFrq, LPFSEB lpFSEBlk) 
{
    LPGSMP  lpFrmBuf;
    #define SILFRMMAX   16384L
    #define SILRESMAX   65535L

    /********************************************************************/
    /********************************************************************/
    if (!flResTim) return ((WORD) -1);

    /********************************************************************/
    /* Initialize silence scan variables                                */
    /********************************************************************/
    lpFSEBlk->usPCMTyp = usPCMTyp;
    lpFSEBlk->usChnCnt = usChnCnt;
    lpFSEBlk->usBIOEnc = usBIOEnc;
    lpFSEBlk->ulSmpFrq = ulSmpFrq;
    lpFSEBlk->ulFrmLen = (DWORD) (flFrmTim / flResTim);
    lpFSEBlk->ulBinPos = 0L;
    lpFSEBlk->ulBinOff = 0L;
    lpFSEBlk->ulResCnt = (DWORD) (flResTim * ulSmpFrq);
    lpFSEBlk->ulAtkCnt = (DWORD) (flFrmTim * flAtkRat * ulSmpFrq);
    lpFSEBlk->ulDcyCnt = (DWORD) (flFrmTim * flDcyRat * ulSmpFrq);
    lpFSEBlk->ulAtkGrd = (DWORD) (flAtkGrd * ulSmpFrq);
    lpFSEBlk->ulDcyGrd = (DWORD) (flDcyGrd * ulSmpFrq);
    lpFSEBlk->ulSndThr = (DWORD) (flSndThr * pow (2, EBSAtDRes (usPCMTyp)) / 2);
    lpFSEBlk->bfPrvSnd = FALSE; 
    EffF2PAloHghPas (ulHghPas, ulSmpFrq, TRUE, (struct _F2PBLK FAR *) &lpFSEBlk->tfHghPas);
    if (!lpFSEBlk->ulFrmLen || !lpFSEBlk->ulResCnt)
        return ((WORD) -1);
    if ((lpFSEBlk->ulFrmLen > SILFRMMAX) || (lpFSEBlk->ulResCnt > SILRESMAX))
        return ((WORD) -1);

    /********************************************************************/
    /* Allocate silence scan frame buffer                               */
    /********************************************************************/
    if (NULL == (lpFrmBuf = GloAloLck (GMEM_MOVEABLE, &lpFSEBlk->mhFrmHdl, 
      lpFSEBlk->ulFrmLen * sizeof (WORD)))) {
        return ((WORD) -1);
    }
    _fmemset (lpFrmBuf, 0, (WORD) lpFSEBlk->ulFrmLen * sizeof (WORD));

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (lpFSEBlk->mhFrmHdl);
    return (0);

}
WORD FAR PASCAL EffFSERel (LPFSEB lpFSEBlk)
{
    /********************************************************************/
    /* Insure safe multiple release calls                               */
    /********************************************************************/
    lpFSEBlk->mhFrmHdl = GloAloRel (lpFSEBlk->mhFrmHdl);
    return (0);
}

WORD FAR PASCAL EffFSEEnu (short sFilHdl, DWORD ulBytRem, LPFSEB lpFSEBlk, 
                LPITCB lpITCBlk, EFFFSECBK fpFSEPrc, DWORD ulFSEDat,
                EBSFRDPRC fpPCMRd_G16)
{
    VISMEMHDL   mhWrkHdl;
    LPGSMP      lpWrkBuf;
    LPVOID      lpFrmBuf;
    DWORD       ulCurCnt;
    DWORD       ulRemCnt;
    DWORD       ulFilByt;
    DWORD       ulFilPos = FilGetPos (sFilHdl);
    ITCBLK      ibITCBlk = *lpITCBlk;
    DWORD       ulBytTot = ulBytRem;
    DWORD       ulEvtOff = 0L;
    DWORD       ulRemSmp = 0L;
    DWORD       ulSmpCnt = 0L;
    DWORD       ulExtTot = lpFSEBlk->ulFrmLen * lpFSEBlk->ulResCnt; 
    DWORD       ulExtCnt = 0L; 
    FSEEVT      feFSEEvt;
    WORD        usRetCod;

    /********************************************************************/
    /* Allocate file I/O and lock frame memory buffers                  */
    /********************************************************************/
    if (NULL == (lpWrkBuf = GloAloLck (GMEM_MOVEABLE, &mhWrkHdl, 
      EBSCNTDEF * sizeof (GENSMP)))) {
        return ((WORD) -1);
    }
    if (NULL == (lpFrmBuf = GloMemLck (lpFSEBlk->mhFrmHdl))) {
        GloUnLRel (mhWrkHdl);
        return ((WORD) -1);
    }

    /********************************************************************/
    /* Initialize callback procedure                                    */
    /********************************************************************/
    fpFSEPrc (EFFFSEINI, ulBytRem, ulFSEDat);

    /********************************************************************/
    /* Skip leading silence; scan file for next sound section           */
    /********************************************************************/
    while (TRUE) {
        ulCurCnt = fpPCMRd_G16 (sFilHdl, min (ulBytRem, BYTMAXFIO), lpWrkBuf, 
            EBSCNTDEF, lpFSEBlk->usPCMTyp, lpFSEBlk->usChnCnt, lpFSEBlk->usBIOEnc, 
            &ibITCBlk, &ulFilByt, NULL, NULL, 0L);
        if (-1L == ulCurCnt) {
            usRetCod = (WORD) -1;
            break;
        }

        /****************************************************************/
        /* 2 pole high pass DC filter                                   */
        /****************************************************************/
        FtrF2PBlkHghPas (lpWrkBuf, ulCurCnt, (struct _F2PBLK FAR *) &lpFSEBlk->tfHghPas);

        /****************************************************************/
        /* Extend silence past end of file (if required)                */
        /****************************************************************/
        if (!ulCurCnt) {
            if (!ulExtTot) break;
            ulExtCnt = min (ulExtTot, EBSCNTDEF);
            _fmemset (lpWrkBuf, 0, (WORD) ulExtCnt * sizeof (GENSMP));
            ulCurCnt += ulExtCnt;
            ulExtTot -= ulExtCnt;
        }

        /****************************************************************/
        /* Transition from silence or sound?                            */
        /* Adjust offset for guard sample count; limit to total bytes.  */
        /****************************************************************/
        ulRemCnt = ulCurCnt;
        while (feFSEEvt = ScnSndEvt (&lpWrkBuf[ulCurCnt-ulRemCnt], &ulRemCnt, 
          lpFrmBuf, &ulEvtOff, lpFSEBlk)) {
            if (EFFFSESND == feFSEEvt) ulEvtOff -= min (ulEvtOff, lpFSEBlk->ulAtkGrd);
            if (EFFFSESIL == feFSEEvt) ulEvtOff += lpFSEBlk->ulDcyGrd;
            ulEvtOff = min (ulEvtOff, EBSByt2Sl (lpFSEBlk->usPCMTyp, 
                lpFSEBlk->usChnCnt, ulBytTot));
            if (!fpFSEPrc (feFSEEvt, ulEvtOff, ulFSEDat)) {
                usRetCod = (WORD) -1;    
                break;
            }
        }

        /****************************************************************/
        /****************************************************************/
        if (!fpFSEPrc (EFFFSEPOL, ulBytTot - ulBytRem, ulFSEDat)) {
            usRetCod = (WORD) -1;    
            break;
        }
        ulBytRem -= ulFilByt;
        ulSmpCnt += ulCurCnt;

    }
    fpFSEPrc (EFFFSETRM, ulBytTot, ulFSEDat);

    /********************************************************************/
    /********************************************************************/
    GloUnLRel (mhWrkHdl);
    GloMemUnL (lpFSEBlk->mhFrmHdl);
    FilSetPos (sFilHdl, ulFilPos, SEEK_SET);

    /********************************************************************/
    /********************************************************************/
    return (usRetCod);

}

FSEEVT  ScnSndEvt (LPGSMP lpSmpBuf, DWORD FAR *pulSmpCnt, WORD FAR *lpFrmBuf, 
        DWORD FAR *pulEvtOff, LPFSEB lpFSEBlk)
{
    DWORD   ulRemSmp = lpFSEBlk->ulBinOff % lpFSEBlk->ulResCnt;
    DWORD   ulSndTot;
    DWORD   ulCurA_D;                       /* Current attack/decay cnt */
    DWORD   ulFrmOff;
    DWORD   uli;                                                            

    /********************************************************************/
    /********************************************************************/
    for (ulSndTot=0L, uli=0L; uli < lpFSEBlk->ulFrmLen; uli++) 
        ulSndTot += lpFrmBuf[uli];
    ulCurA_D = lpFSEBlk->bfPrvSnd ? lpFSEBlk->ulDcyCnt : lpFSEBlk->ulAtkCnt;

    /********************************************************************/
    /* Increment each frame bin based upon signal level                 */
    /********************************************************************/
    for (uli=1L; uli <= *pulSmpCnt; uli++) {
        if ((DWORD) abs (*lpSmpBuf++) > lpFSEBlk->ulSndThr) 
            lpFrmBuf[lpFSEBlk->ulBinPos]++;
        /****************************************************************/
        /****************************************************************/
        if (!((ulRemSmp + uli) % lpFSEBlk->ulResCnt)) {
            /************************************************************/
            /* Drop oldest bin, adjust totals, init new bin             */
            /************************************************************/
            ulSndTot += lpFrmBuf[lpFSEBlk->ulBinPos];            
            lpFSEBlk->ulBinPos = (lpFSEBlk->ulBinPos+1L) % lpFSEBlk->ulFrmLen;
            ulSndTot -= lpFrmBuf[lpFSEBlk->ulBinPos];            
            lpFrmBuf[lpFSEBlk->ulBinPos] = 0L;
            /************************************************************/
            /* Transition from silence or sound?                        */
            /************************************************************/
            if (lpFSEBlk->bfPrvSnd != (ulSndTot > ulCurA_D)) {
                /********************************************************/
                /* Scan frame for first snd/sil bin, skip recent 0      */
                /* Always finds (sound < ulFrmLen), thats how got here! */
                /********************************************************/
                ulFrmOff = ScnFrmBuf (lpFrmBuf, 
                    (lpFSEBlk->ulBinPos + 1L) % lpFSEBlk->ulFrmLen, 
                    lpFSEBlk->ulFrmLen, ulSndTot > ulCurA_D, 
                    ulCurA_D / lpFSEBlk->ulFrmLen); 
                /********************************************************/
                /********************************************************/
                lpFSEBlk->bfPrvSnd = !lpFSEBlk->bfPrvSnd;
                lpFSEBlk->ulBinOff += uli;
                *pulEvtOff  = lpFSEBlk->ulBinOff - (ulFrmOff * lpFSEBlk->ulResCnt);
                *pulSmpCnt -= uli;
                /********************************************************/
                /********************************************************/
                return (lpFSEBlk->bfPrvSnd ? EFFFSESND : EFFFSESIL);
            }
        }
    }
    lpFSEBlk->ulBinOff += *pulSmpCnt;
    *pulSmpCnt = 0L;

    /********************************************************************/
    /********************************************************************/
    return (EFFFSENON);

}

DWORD   ScnFrmBuf (WORD FAR *lpFrmBuf, DWORD ulBinPos, DWORD ulFrmLen, 
        BOOL bfSndSel, DWORD ulThrCnt) 
{
    DWORD   uli;    

    /********************************************************************/
    /* Scan frame for first sound / silence bin                         */
    /* Return offset from final (most recent) bin.                      */
    /********************************************************************/
    if (bfSndSel) for (uli=0L; uli < ulFrmLen; uli++) {
        /****************************************************************/
        /* Scan forward for sound; return first sound bin               */
        /****************************************************************/
        if (lpFrmBuf[(ulBinPos + uli) % ulFrmLen]  > ulThrCnt) 
            return ((ulFrmLen - 1L) - uli);                 /* 0 Offset */
    } 
    else for (uli=ulFrmLen-1L; uli > 0L; uli--) {
        /****************************************************************/
        /* Scan backwards for non-sound; return first silence bin       */
        /****************************************************************/
        if (lpFrmBuf[(ulBinPos + uli) % ulFrmLen] > ulThrCnt) 
            return ((ulFrmLen - 1L) - uli);                 /* 0 Offset */
    }
    /********************************************************************/
    /********************************************************************/
    return ((uli >= ulFrmLen) ? 0 : ulFrmLen - 1L); /* Should not reach */    
}

DWORD FAR PASCAL EffFSEChp (LPEBSB lpSrcEBS, LPEBSB lpDstEBS, WORD usFSECnt, 
                 LPFSER lpFSERec)
{

    LPGSMP  lpSrcBuf;
    DWORD   ulDstRem;
    DWORD   ulInpSmp;

    /********************************************************************/
    /********************************************************************/
    if (!lpSrcEBS->mhBufHdl || !lpSrcEBS->ulSmpCnt) 
        return (EffEBStoE (lpSrcEBS, lpDstEBS, 0L));
    if (!usFSECnt) 
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
    /* Discard any data until beginning position is satisfied           */
    /* Note: EBSUpdRel() unlocks & frees (if empty)                     */
    /********************************************************************/
    if (lpFSERec->ulBegPos > 0L) {
        DWORD   ulRelSmp = min (lpFSERec->ulBegPos, lpSrcEBS->ulSmpCnt);
        lpDstEBS->usEOSCod  = EBSUpdRel (lpSrcEBS, lpSrcBuf, ulRelSmp);
        lpFSERec->ulBegPos -= ulRelSmp;
        lpFSERec->ulEndPos -= min (lpFSERec->ulEndPos, ulRelSmp);
    }
    else GloMemUnL (lpSrcEBS->mhBufHdl);

    /********************************************************************/
    /* Free source if at end of sound sequence                          */
    /********************************************************************/
    if (!lpFSERec->ulEndPos) {
        lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
        lpDstEBS->usEOSCod = lpSrcEBS->usEOSCod;
        return (0L);
    }

    /********************************************************************/
    /* Only try to keep as many samples as will fit in destination      */
    /********************************************************************/
    ulInpSmp = min (lpFSERec->ulEndPos, min (lpSrcEBS->ulSmpCnt, ulDstRem));
    lpFSERec->ulEndPos -= ulInpSmp;
    return (EffEBStoE (lpSrcEBS, lpDstEBS, ulInpSmp));

}

/************************************************************************/
/*          DC Filter (Future: replace with general purpose             */
/************************************************************************/
typedef struct _F2PBLK {                /* Two pole filter block        */
    float       flCtrPer; 
    LNGFRA      lfCtrPer;
    union {
        float   flP0;                   
        LNGFRA  lfP0;                   
    };
    union {
        float   flP1;
        LNGFRA  lfP1;
    };
    BYTE        ucPad[8];
} F2PBLK;
typedef F2PBLK FAR * LPF2PB;

/************************************************************************/
/************************************************************************/
WORD EffF2PAloHghPas (DWORD ulHghPas, DWORD ulSmpFrq, BOOL bfFstFlg, LPF2PB lpF2PBlk)
{
  
    /********************************************************************/
    /********************************************************************/
    _fmemset (lpF2PBlk, 0, sizeof (*lpF2PBlk));         /* Initialize   */

    /********************************************************************/
    /* Calculate cutoff period                                          */
    /********************************************************************/
    if (ulHghPas < ulSmpFrq) {
        if (!bfFstFlg) 
            lpF2PBlk->flCtrPer = (float) FRQTOFCON (ulHghPas, ulSmpFrq);
        else 
            lpF2PBlk->lfCtrPer = (long) (LNGFRAONE * FRQTOFCON (ulHghPas / 2L, ulSmpFrq));
    }

    /********************************************************************/
    /********************************************************************/
    return (0);

}

DWORD   FtrF2PBlkHghPas (LPGSMP lpDstBuf, DWORD ulSmpCnt, LPF2PB lpF2PBlk)
{
    F2PBLK  fbFtr = *lpF2PBlk;          /* Local for faster access      */
    LNGFRA  lfCurWav;
    float   flCurWav;
    DWORD   uli;

    /********************************************************************/
    /* Note: flCtrPer must be non-zero                                  */
    /********************************************************************/
    if (lpF2PBlk->flCtrPer) for (uli = 0; uli < ulSmpCnt; uli++) {
        /****************************************************************/
        /* Two pole low pass filter (floating point)                    */
        /****************************************************************/
        flCurWav = (float) lpDstBuf[uli];
        fbFtr.flP0 += (flCurWav   - fbFtr.flP0) * fbFtr.flCtrPer;
        fbFtr.flP1 += (fbFtr.flP0 - fbFtr.flP1) * fbFtr.flCtrPer;
        lpDstBuf[uli] -= (short) fbFtr.flP1;
    }
    /********************************************************************/
    /* Note: lfCtrPer must be non-zero                                  */
    /********************************************************************/
    else if (lpF2PBlk->lfCtrPer) for (uli = 0; uli < ulSmpCnt; uli++) {
        /****************************************************************/
        /* Two pole low pass filter ("long fraction")                   */
        /****************************************************************/
        lfCurWav = (LNGFRA) lpDstBuf[uli] << LNGFRANRM;
        fbFtr.lfP0 += LNGFRAMUL ((lfCurWav   - fbFtr.lfP0), fbFtr.lfCtrPer);
        fbFtr.lfP1 += LNGFRAMUL ((fbFtr.lfP0 - fbFtr.lfP1), fbFtr.lfCtrPer);
        lpDstBuf[uli] -= (short) (fbFtr.lfP1 >> LNGFRANRM);
    }

    /********************************************************************/
    /********************************************************************/
    *lpF2PBlk = fbFtr;
    return (ulSmpCnt);

}


