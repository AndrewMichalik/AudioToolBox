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

/************************************************************************/
/************************************************************************/
DWORD   FAR PASCAL  FtrIIRFlt (LPGSMP, DWORD, LPTDOF, WORD FAR *, EFFPOLPRC, DWORD);
DWORD   FAR PASCAL  FtrIIRLFA (LPGSMP, DWORD, LPTDOF, WORD FAR *, EFFPOLPRC, DWORD);
DWORD   FAR PASCAL  FtrIIRLFA221 (LPGSMP, DWORD, LPTDOF, WORD FAR *, EFFPOLPRC, DWORD);

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL EffIIRAlo (FTRTYP usFtrTyp, PASTYP usPasTyp, WORD usSecCnt, 
                WORD usSecOrd, DWORD ulStpFrq, DWORD ulRsv001, float flPasFac, 
                float flStpAtt, DWORD ulSmpFrq, BOOL bfFstFlg, LPTDOF lpTDoFtr)
{

WORD usSecLen = usSecOrd+1;

    /********************************************************************/
    /********************************************************************/
    _fmemset (lpTDoFtr, 0, sizeof (*lpTDoFtr));         /* Initialize   */
    if (ulStpFrq >= ulSmpFrq/2L) return (0);

    /********************************************************************/
    /********************************************************************/
    return (EffIIROpt (usFtrTyp, usPasTyp, usSecCnt, usSecLen, ulStpFrq, 
        flPasFac, flStpAtt, ulSmpFrq, bfFstFlg, lpTDoFtr));
}

WORD    FAR PASCAL  EffIIRRel (LPTDOF lpTDoFtr)
{
    return (0);
}

DWORD FAR PASCAL EffIIRFtr (LPEBSB lpSrcEBS, LPEBSB lpDstEBS, LPTDOF lpTDoFtr)
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
        /****************************************************************/
        /* Filter a 2 section, 2 zero, 1 pole IIR filter                */
        /****************************************************************/
        if ((2 == lpTDoFtr->usSec) && (2 == lpTDoFtr->tbZer.usLen) &&
            (1 == lpTDoFtr->tbPol.usLen)) 
            ulInpSmp = FtrIIRLFA221 (lpSrcBuf, min (lpSrcEBS->ulSmpCnt, ulDstRem), 
                lpTDoFtr, NULL, NULL, 0L);
        else
            ulInpSmp = FtrIIRLFA (lpSrcBuf, min (lpSrcEBS->ulSmpCnt, ulDstRem), 
                lpTDoFtr, NULL, NULL, 0L);
    }
    else {
        ulInpSmp = FtrIIRFlt (lpSrcBuf, min (lpSrcEBS->ulSmpCnt, ulDstRem), 
            lpTDoFtr, NULL, NULL, 0L);
    }

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (lpSrcEBS->mhBufHdl);

    /********************************************************************/
    /********************************************************************/
    return (EffEBStoE (lpSrcEBS, lpDstEBS, ulInpSmp));

}

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL FtrIIRFlt (LPGSMP lpSrcBuf, DWORD ulSmpCnt, LPTDOF lpTDoFtr,
                 WORD FAR *lpErrCod, EFFPOLPRC fpPolPrc, DWORD ulPolDat)
{
    TDFBLK  tbZer = lpTDoFtr->tbZer;
    TDFBLK  tbPol = lpTDoFtr->tbPol;
    DWORD   uli = ulSmpCnt;

//    /********************************************************************/
//    /********************************************************************/
//    if ((tbZer.usLen <= 1) || (tbPol.usLen != (tbZer.usLen - 1))) 
//        return ((WORD) -1);

    /********************************************************************/
    /* Show/Hide Progress indicator for time consuming operations       */
    /********************************************************************/
    if (fpPolPrc) fpPolPrc (EFFPOLBEG, ulSmpCnt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    while (uli--) {
        WORD    usi;
        WORD    usSec;
        float   flx =  *lpSrcBuf;
        /****************************************************************/
        /* Filter each section of a cascaded filter.                    */
        /* Note zeros range from 0 to usLen, poles from 0 to usLen - 1  */
        /****************************************************************/
        for (usSec = 0; usSec < lpTDoFtr->usSec; usSec++) {
            float FAR * lpZerCoe = &(tbZer.flCoe[usSec*tbZer.usLen]);
            float FAR * lpZerHis = &(tbZer.flHis[usSec*tbZer.usLen]);
            float FAR * lpPolCoe = &(tbPol.flCoe[usSec*tbPol.usLen]);
            float FAR * lpPolHis = &(tbPol.flHis[usSec*tbPol.usLen]);
            /************************************************************/
            /* Compute output for this section of filter                */
            /************************************************************/
            lpZerHis[0] = flx;             
            flx = lpZerCoe[0] * lpZerHis[0];
            for (usi = 1; usi < tbZer.usLen; usi++) 
                flx += (lpZerCoe[usi]   * lpZerHis[usi])
                     - (lpPolCoe[usi-1] * lpPolHis[usi-1]);
            /************************************************************/
            /* Shift input and output histories                         */
            /************************************************************/
            _fmemmove (&(lpZerHis[1]), &(lpZerHis[0]), 
                (tbZer.usLen - 1) * sizeof (*lpZerHis));
            _fmemmove (&(lpPolHis[1]), &(lpPolHis[0]), 
                (tbPol.usLen - 1) * sizeof (*lpPolHis));
            lpPolHis[0] = flx;       
        }
//        flx = min (lfx, ONE * + ATDMAXDEF);  
//        flx = max (lfx, ONE * - ATDMAXDEF);  
        *lpSrcBuf++ = (short) flx;

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
    lpTDoFtr->tbPol = tbPol;
    return (ulSmpCnt);
}

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL FtrIIRLFA (LPGSMP lpSrcBuf, DWORD ulSmpCnt, LPTDOF lpTDoFtr,
                 WORD FAR *lpErrCod, EFFPOLPRC fpPolPrc, DWORD ulPolDat)
{
    TDFBLK  tbZer = lpTDoFtr->tbZer;
    TDFBLK  tbPol = lpTDoFtr->tbPol;
    DWORD   uli = ulSmpCnt;

//    /********************************************************************/
//    /********************************************************************/
//    if ((tbZer.usLen <= 1) || (tbPol.usLen != (tbZer.usLen - 1))) 
//        return ((WORD) -1);

    /********************************************************************/
    /* Show/Hide Progress indicator for time consuming operations       */
    /********************************************************************/
    if (fpPolPrc) fpPolPrc (EFFPOLBEG, ulSmpCnt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    while (uli--) {
        WORD    usi;
        WORD    usSec;
        LNGFRA  lfx =  LNGFRAONE * *lpSrcBuf;
        /****************************************************************/
        /* Filter each section of a cascaded filter.                    */
        /* Note zeros range from 0 to usLen, poles from 0 to usLen - 1  */
        /****************************************************************/
        for (usSec = 0; usSec < lpTDoFtr->usSec; usSec++) {
            LNGFRA FAR * lpZerCoe = &(tbZer.lfCoe[usSec*tbZer.usLen]);
            LNGFRA FAR * lpZerHis = &(tbZer.lfHis[usSec*tbZer.usLen]);
            LNGFRA FAR * lpPolCoe = &(tbPol.lfCoe[usSec*tbPol.usLen]);
            LNGFRA FAR * lpPolHis = &(tbPol.lfHis[usSec*tbPol.usLen]);
            /************************************************************/
            /* Compute output for this section of filter                */
            /************************************************************/
            lpZerHis[0] = lfx;             
            lfx = LNGFRAMUL (lpZerCoe[0], lpZerHis[0]);
            for (usi = 1; usi < tbZer.usLen; usi++) 
                lfx += LNGFRAMUL (lpZerCoe[usi],   lpZerHis[usi])
                     - LNGFRAMUL (lpPolCoe[usi-1], lpPolHis[usi-1]);
            /************************************************************/
            /* Shift input and output histories                         */
            /************************************************************/
            _fmemmove (&(lpZerHis[1]), &(lpZerHis[0]), 
                (tbZer.usLen - 1) * sizeof (*lpZerHis));
            _fmemmove (&(lpPolHis[1]), &(lpPolHis[0]), 
                (tbPol.usLen - 1) * sizeof (*lpPolHis));
            lpPolHis[0] = lfx;       

        }
        /****************************************************************/
        /****************************************************************/
//        lfx = min (lfx, LNGFRAONE * + ATDMAXDEF);  
//        lfx = max (lfx, LNGFRAONE * - ATDMAXDEF);  
        *lpSrcBuf++ = (short) (lfx >> LNGFRANRM);

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
    lpTDoFtr->tbPol = tbPol;
    return (ulSmpCnt);
}

DWORD FAR PASCAL FtrIIRLFA221 (LPGSMP lpSrcBuf, DWORD ulSmpCnt, LPTDOF lpTDoFtr,
                 WORD FAR *lpErrCod, EFFPOLPRC fpPolPrc, DWORD ulPolDat)
{
    LNGFRA  lfZerCoeA0 = lpTDoFtr->tbZer.lfCoe[0];
    LNGFRA  lfZerCoeA1 = lpTDoFtr->tbZer.lfCoe[1];
    LNGFRA  lfZerHisA0 = lpTDoFtr->tbZer.lfHis[0];
    LNGFRA  lfZerHisA1 = lpTDoFtr->tbZer.lfHis[1];
    LNGFRA  lfPolCoeA0 = lpTDoFtr->tbPol.lfCoe[0];
    LNGFRA  lfPolHisA0 = lpTDoFtr->tbPol.lfHis[0];
    LNGFRA  lfZerCoeB0 = (&(lpTDoFtr->tbZer.lfCoe[lpTDoFtr->tbZer.usLen]))[0];
    LNGFRA  lfZerCoeB1 = (&(lpTDoFtr->tbZer.lfCoe[lpTDoFtr->tbZer.usLen]))[1];
    LNGFRA  lfZerHisB0 = (&(lpTDoFtr->tbZer.lfHis[lpTDoFtr->tbZer.usLen]))[0];
    LNGFRA  lfZerHisB1 = (&(lpTDoFtr->tbZer.lfHis[lpTDoFtr->tbZer.usLen]))[1];
    LNGFRA  lfPolCoeB0 = (&(lpTDoFtr->tbPol.lfCoe[lpTDoFtr->tbPol.usLen]))[0];
    LNGFRA  lfPolHisB0 = (&(lpTDoFtr->tbPol.lfHis[lpTDoFtr->tbPol.usLen]))[0];
    DWORD   uli = ulSmpCnt;

//    /********************************************************************/
//    /* Filter a 2 section, 2 zero, 1 pole IIR filter                    */
//    /********************************************************************/
//    if ((lpTDoFtr->usSec != 2) || (lpTDoFtr->tbPol.usLen != 2) ||
//        (lpTDoFtr->tbZer.usLen != 1)) return ((WORD) -1);

    /********************************************************************/
    /* Show/Hide Progress indicator for time consuming operations       */
    /********************************************************************/
    if (fpPolPrc) fpPolPrc (EFFPOLBEG, ulSmpCnt, ulPolDat);

    /********************************************************************/
    /* Apply all sections to each sample (cascaded implementation)      */
    /********************************************************************/
    while (uli--) {
        /****************************************************************/
        /* Compute output for first section (A) of filter               */
        /****************************************************************/
        lfZerHisA0 = LNGFRAONE * *lpSrcBuf;             
        lfPolHisA0 = LNGFRAACC (LNGFRAMAC (lfZerCoeA0, lfZerHisA0)
                        + LNGFRAMAC (lfZerCoeA1, lfZerHisA1)
                        - LNGFRAMAC (lfPolCoeA0, lfPolHisA0));
        lfZerHisA1 = lfZerHisA0;

        /****************************************************************/
        /* Compute output for second section (B) of filter              */
        /****************************************************************/
        lfZerHisB0 = lfPolHisA0;
        lfPolHisB0 = LNGFRAACC (LNGFRAMAC (lfZerCoeB0, lfZerHisB0)
                        + LNGFRAMAC (lfZerCoeB1, lfZerHisB1)
                        - LNGFRAMAC (lfPolCoeB0, lfPolHisB0));
        lfZerHisB1 = lfZerHisB0;

        /****************************************************************/
        /****************************************************************/
        *lpSrcBuf++ = (short) (lfPolHisB0 >> LNGFRANRM);

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
    lpTDoFtr->tbZer.lfCoe[0] = lfZerCoeA0;
    lpTDoFtr->tbZer.lfCoe[1] = lfZerCoeA1;
    lpTDoFtr->tbZer.lfHis[0] = lfZerHisA0;
    lpTDoFtr->tbZer.lfHis[1] = lfZerHisA1;
    lpTDoFtr->tbPol.lfCoe[0] = lfPolCoeA0;
    lpTDoFtr->tbPol.lfHis[0] = lfPolHisA0;
    (&(lpTDoFtr->tbZer.lfCoe[lpTDoFtr->tbZer.usLen]))[0] = lfZerCoeB0;
    (&(lpTDoFtr->tbZer.lfCoe[lpTDoFtr->tbZer.usLen]))[1] = lfZerCoeB1;
    (&(lpTDoFtr->tbZer.lfHis[lpTDoFtr->tbZer.usLen]))[0] = lfZerHisB0;
    (&(lpTDoFtr->tbZer.lfHis[lpTDoFtr->tbZer.usLen]))[1] = lfZerHisB1;
    (&(lpTDoFtr->tbPol.lfCoe[lpTDoFtr->tbPol.usLen]))[0] = lfPolCoeB0;
    (&(lpTDoFtr->tbPol.lfHis[lpTDoFtr->tbPol.usLen]))[0] = lfPolHisB0;
    
    /********************************************************************/
    /********************************************************************/
    return (ulSmpCnt);
}

//DWORD    FtrIIRLFA321 (LPGSMP, WORD, CVSFTR FAR *);
//DWORD    FtrIIRLFA321 (LPGSMP lpSrcBuf, DWORD ulSmpCnt, CVSFTR FAR *lpTDoFtr)
//{
//    LNGFRA  lfZerCoeA0 = lpTDoFtr->tbZer.lfCoe[0];
//    LNGFRA  lfZerCoeA1 = lpTDoFtr->tbZer.lfCoe[1];
//    LNGFRA  lfZerHisA0 = lpTDoFtr->tbZer.lfHis[0];
//    LNGFRA  lfZerHisA1 = lpTDoFtr->tbZer.lfHis[1];
//    LNGFRA  lfPolCoeA0 = lpTDoFtr->tbPol.lfCoe[0];
//    LNGFRA  lfPolHisA0 = lpTDoFtr->tbPol.lfHis[0];
//    LNGFRA  lfZerCoeB0 = (&(lpTDoFtr->tbZer.lfCoe[lpTDoFtr->tbZer.usLen]))[0];
//    LNGFRA  lfZerCoeB1 = (&(lpTDoFtr->tbZer.lfCoe[lpTDoFtr->tbZer.usLen]))[1];
//    LNGFRA  lfZerHisB0 = (&(lpTDoFtr->tbZer.lfHis[lpTDoFtr->tbZer.usLen]))[0];
//    LNGFRA  lfZerHisB1 = (&(lpTDoFtr->tbZer.lfHis[lpTDoFtr->tbZer.usLen]))[1];
//    LNGFRA  lfPolCoeB0 = (&(lpTDoFtr->tbPol.lfCoe[lpTDoFtr->tbPol.usLen]))[0];
//    LNGFRA  lfPolHisB0 = (&(lpTDoFtr->tbPol.lfHis[lpTDoFtr->tbPol.usLen]))[0];
//    LNGFRA  lfZerCoeC0 = (&(lpTDoFtr->tbZer.lfCoe[2 * lpTDoFtr->tbZer.usLen]))[0];
//    LNGFRA  lfZerCoeC1 = (&(lpTDoFtr->tbZer.lfCoe[2 * lpTDoFtr->tbZer.usLen]))[1];
//    LNGFRA  lfZerHisC0 = (&(lpTDoFtr->tbZer.lfHis[2 * lpTDoFtr->tbZer.usLen]))[0];
//    LNGFRA  lfZerHisC1 = (&(lpTDoFtr->tbZer.lfHis[2 * lpTDoFtr->tbZer.usLen]))[1];
//    LNGFRA  lfPolCoeC0 = (&(lpTDoFtr->tbPol.lfCoe[2 * lpTDoFtr->tbPol.usLen]))[0];
//    LNGFRA  lfPolHisC0 = (&(lpTDoFtr->tbPol.lfHis[2 * lpTDoFtr->tbPol.usLen]))[0];
//    DWORD   uli = ulSmpCnt;
//
//    /********************************************************************/
//    /********************************************************************/
//    if ((lpTDoFtr->tbZer.usLen <= 1) || (lpTDoFtr->tbPol.usLen != (lpTDoFtr->tbZer.usLen - 1))) 
//        return ((WORD) -1);
//
//    /********************************************************************/
//    /* Filter a 3 section, 2 zero, 1 pole IIR filter                    */
//    /********************************************************************/
//    while (uli--) {
//        /****************************************************************/
//        /* Compute output for first section (A) of filter               */
//        /****************************************************************/
//        lfZerHisA0 = LNGFRAONE * *lpSrcBuf;             
//        lfPolHisA0 = LNGFRAACC (LNGFRAMAC (lfZerCoeA0, lfZerHisA0)
//                        + LNGFRAMAC (lfZerCoeA1, lfZerHisA1)
//                        - LNGFRAMAC (lfPolCoeA0, lfPolHisA0));
//        lfZerHisA1 = lfZerHisA0;
//
//        /****************************************************************/
//        /* Compute output for second section (B) of filter              */
//        /****************************************************************/
//        lfZerHisB0 = lfPolHisA0;
//        lfPolHisB0 = LNGFRAACC (LNGFRAMAC (lfZerCoeB0, lfZerHisB0)
//                        + LNGFRAMAC (lfZerCoeB1, lfZerHisB1)
//                        - LNGFRAMAC (lfPolCoeB0, lfPolHisB0));
//        lfZerHisB1 = lfZerHisB0;
//
//        /****************************************************************/
//        /* Compute output for third section (C) of filter               */
//        /****************************************************************/
//        lfZerHisC0 = lfPolHisB0;
//        lfPolHisC0 = LNGFRAACC (LNGFRAMAC (lfZerCoeC0, lfZerHisC0)
//                        + LNGFRAMAC (lfZerCoeC1, lfZerHisC1)
//                        - LNGFRAMAC (lfPolCoeC0, lfPolHisC0));
//        lfZerHisC1 = lfZerHisC0;
////lfPolHisC0 = lfPolHisB0;
//
//        /****************************************************************/
//        /****************************************************************/
////        lfx = min (lfx, LNGFRAONE * + ATDMAXDEF);  
////        lfx = max (lfx, LNGFRAONE * - ATDMAXDEF);  
//        *lpSrcBuf++ = (short) (lfPolHisC0 >> LNGFRANRM);
//
//    }
//
//    /********************************************************************/
//    /********************************************************************/
//    lpTDoFtr->tbZer.lfCoe[0] = lfZerCoeA0;
//    lpTDoFtr->tbZer.lfCoe[1] = lfZerCoeA1;
//    lpTDoFtr->tbZer.lfHis[0] = lfZerHisA0;
//    lpTDoFtr->tbZer.lfHis[1] = lfZerHisA1;
//    lpTDoFtr->tbPol.lfCoe[0] = lfPolCoeA0;
//    lpTDoFtr->tbPol.lfHis[0] = lfPolHisA0;
//    (&(lpTDoFtr->tbZer.lfCoe[lpTDoFtr->tbZer.usLen]))[0] = lfZerCoeB0;
//    (&(lpTDoFtr->tbZer.lfCoe[lpTDoFtr->tbZer.usLen]))[1] = lfZerCoeB1;
//    (&(lpTDoFtr->tbZer.lfHis[lpTDoFtr->tbZer.usLen]))[0] = lfZerHisB0;
//    (&(lpTDoFtr->tbZer.lfHis[lpTDoFtr->tbZer.usLen]))[1] = lfZerHisB1;
//    (&(lpTDoFtr->tbPol.lfCoe[lpTDoFtr->tbPol.usLen]))[0] = lfPolCoeB0;
//    (&(lpTDoFtr->tbPol.lfHis[lpTDoFtr->tbPol.usLen]))[0] = lfPolHisB0;
//    (&(lpTDoFtr->tbZer.lfCoe[2 * lpTDoFtr->tbZer.usLen]))[0] = lfZerCoeC0;
//    (&(lpTDoFtr->tbZer.lfCoe[2 * lpTDoFtr->tbZer.usLen]))[1] = lfZerCoeC1;
//    (&(lpTDoFtr->tbZer.lfHis[2 * lpTDoFtr->tbZer.usLen]))[0] = lfZerHisC0;
//    (&(lpTDoFtr->tbZer.lfHis[2 * lpTDoFtr->tbZer.usLen]))[1] = lfZerHisC1;
//    (&(lpTDoFtr->tbPol.lfCoe[2 * lpTDoFtr->tbPol.usLen]))[0] = lfPolCoeC0;
//    (&(lpTDoFtr->tbPol.lfHis[2 * lpTDoFtr->tbPol.usLen]))[0] = lfPolHisC0;
//    
//    /********************************************************************/
//    /********************************************************************/
//    return (ulSmpCnt);
//}


