/************************************************************************/
/* Effects Buffered Stream Support: EBSSup.c            V2.00  03/15/95 */
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
#include "effsup.h"                     /* Effects support definitions  */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL EBSAloReq (LPEBSB lpNewEBS, PCMTYP usPCMTyp, WORD usAtDRes,
                 WORD usChnCnt, DWORD ulSmpFrq, DWORD ulSmpReq)
{
    DWORD ulDstRem;

    /********************************************************************/
    /* Set to default PCM type if requested                             */
    /********************************************************************/
    if (EBSPCMDEF == usPCMTyp) usPCMTyp = GENPCM000;

    /********************************************************************/
    /********************************************************************/
    while (ulSmpReq) {
        ulDstRem = EBSAloMem (lpNewEBS, usPCMTyp, usAtDRes, usChnCnt, 
            ulSmpFrq, ulSmpReq);
        if (ulDstRem) return (ulDstRem);
        if ((ulSmpReq /= 2L) < EBSCNTMIN) return (0L);
    }
    return (0L);

}

DWORD FAR PASCAL EBSAloMem (LPEBSB lpNewEBS, PCMTYP usPCMTyp, WORD usAtDRes,
                 WORD usChnCnt, DWORD ulSmpFrq, DWORD ulSmpCnt)
{

    /********************************************************************/
    /* Set to default PCM type if requested                             */
    /********************************************************************/
    if (EBSPCMDEF == usPCMTyp) usPCMTyp = GENPCM000;

    /********************************************************************/
    /* Initialize to ulSmpCnt length with zero contents                 */
    /********************************************************************/
    if (!lpNewEBS->mhBufHdl) {

/********************************************************************/
/* Limit Win allocation to <64K until all can handle >64K EBS's     */
/* Limit DOS allocation to <=8K to operate under 400K limit         */
/********************************************************************/
#if (defined (W31)) /************************************************/
    ulSmpCnt = min (ulSmpCnt, EBSByt2Sl (usPCMTyp, usChnCnt, BYTMAXFIO));        
#endif /*************************************************************/
#if (defined (DOS)) /************************************************/
    #define DOSMAXMIO   0x1fff   
    ulSmpCnt = min (ulSmpCnt, EBSByt2Sl (usPCMTyp, usChnCnt, DOSMAXMIO));        
#endif /*************************************************************/

        /****************************************************************/
        /****************************************************************/
        if (0L == (lpNewEBS->mhBufHdl = GloAloMem (GMEM_MOVEABLE, EBSSmp2Bh (usPCMTyp, usChnCnt, ulSmpCnt))))
            return (0L);
        lpNewEBS->usPCMTyp = usPCMTyp;
        lpNewEBS->usAtDRes = usAtDRes;
        lpNewEBS->usChnCnt = usChnCnt;
        lpNewEBS->ulSmpFrq = ulSmpFrq;
        lpNewEBS->ulSmpCnt = 0L;
        lpNewEBS->usEOSCod = FIOEOS_OK;
    }
    else {
        /****************************************************************/
        /* Any room left?                                               */
        /****************************************************************/
        if (lpNewEBS->ulSmpCnt >= EBSByt2Sl (usPCMTyp, usChnCnt, GloMemSiz (lpNewEBS->mhBufHdl))) {
            lpNewEBS->mhBufHdl = GloAloRel (lpNewEBS->mhBufHdl);
            return (0L);
        }
    }

    /********************************************************************/
    /********************************************************************/
    return (EBSByt2Sl (usPCMTyp, usChnCnt, GloMemSiz (lpNewEBS->mhBufHdl)) - lpNewEBS->ulSmpCnt);

}

EOSCOD FAR PASCAL EBSUpdRel (LPEBSB lpSrcEBS, LPVOID lpSrcBuf, DWORD ulRelSmp)
{

    /********************************************************************/
    /********************************************************************/
    ulRelSmp = min (ulRelSmp, lpSrcEBS->ulSmpCnt);

    /********************************************************************/
    /* Shift remaining source samples to front of buffer                */
    /* Return destination End of Stream flag                            */
    /********************************************************************/
    lpSrcEBS->ulSmpCnt -= ulRelSmp;
    if (0L != lpSrcEBS->ulSmpCnt) {
        _fmemmove (lpSrcBuf, 
            &((LPSTR)lpSrcBuf)[EBSSmp2Bh (lpSrcEBS->usPCMTyp, lpSrcEBS->usChnCnt, ulRelSmp)], 
            (WORD) EBSSmp2Bh (lpSrcEBS->usPCMTyp, lpSrcEBS->usChnCnt, lpSrcEBS->ulSmpCnt));
        GloMemUnL (lpSrcEBS->mhBufHdl);
        /****************************************************************/
        /* Return OK if if at end of file. EOF indicator will appear in */
        /* destination buffer after all source samples are used.        */
        /****************************************************************/
        return ((FIOEOSEOF == lpSrcEBS->usEOSCod) ? FIOEOS_OK : lpSrcEBS->usEOSCod);
    }

    /********************************************************************/
    /* If entire source used, delete source memory references           */
    /********************************************************************/
    GloMemUnL (lpSrcEBS->mhBufHdl);
    lpSrcEBS->mhBufHdl = GloAloRel (lpSrcEBS->mhBufHdl);
    return (lpSrcEBS->usEOSCod);

}

/************************************************************************/
/* Effects always work with Generic Sample & do not require an ITCBLK   */
/************************************************************************/
DWORD FAR PASCAL EBSSmp2Bh (PCMTYP usPCMTyp, WORD usChnCnt, DWORD ulSmpCnt)
{
    return (PCMSmp2Bh (usPCMTyp, usChnCnt, ulSmpCnt, NULL));
}

DWORD FAR PASCAL EBSByt2Sl (PCMTYP usPCMTyp, WORD usChnCnt, DWORD ulSmpCnt)
{
    return (PCMByt2Sl (usPCMTyp, usChnCnt, ulSmpCnt, NULL));
}

DWORD FAR PASCAL EBSSmpLow (PCMTYP usPCMTyp, WORD usChnCnt, DWORD ulSmpCnt)
{
    return (PCMSmpLow (usPCMTyp, usChnCnt, ulSmpCnt, NULL));
}

DWORD FAR PASCAL EBSAtDRes (PCMTYP usPCMTyp)
{
    /********************************************************************/
    /* Note: Supported PCM values are one less than A to D resolution   */
    /********************************************************************/
    return (PCMCapQry (usPCMTyp, ATDRESQRY, 0L));
}
