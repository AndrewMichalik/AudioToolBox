/************************************************************************/
/* File Frequency Effects: FilEff.c                     V2.00  01/10/93 */
/* Copyright (c) 1987-1995 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#if (defined (W31)) /****************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#include "..\os_dev\winmem.h"           /* Generic memory supp defs     */
#include "..\os_dev\winmsg.h"           /* User message support defs    */
#endif /*****************************************************************/
#if (defined (DOS)) /****************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "..\os_dev\dosmem.h"           /* Generic memory support defs  */
#include "..\os_dev\dosmsg.h"           /* User message support defs    */
#endif /*****************************************************************/
#include "..\fiodev\filutl.h"           /* File utilities definitions   */
#include "..\pcmdev\genpcm.h"           /* PCM/APCM conv routine defs   */
#include "..\effdev\geneff.h"           /* Sound Effects definitions    */
#include "geneff.h"                     /* Frequency effects support    */
#include "libsup.h"                     /* Frequency effects lib supp   */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */
#include <math.h>                       /* Math library defs            */

/************************************************************************/
/*                          External References                         */
/************************************************************************/
extern  TBXGLO  TBxGlo;

/************************************************************************/
/*                          Forward References                          */
/************************************************************************/
WORD FAR PASCAL SubPolPrc (EFFPOL, DWORD, DWORD);

/************************************************************************/
/************************************************************************/
DWORD   FrqFilEff (short sSrcHdl, short sDstHdl, DWORD ulBytReq, 
        LPSTR lpWrkBuf, WORD usBufSiz, ITCBLK FAR *pibSrcITC, 
        ITCBLK FAR *pibDstITC, FRQBLK FAR *lpFrqBlk)
{

    DWORD   ulBytCnt;                   /* Write sample cnt proc / pass */
    EBSBLK  ebSrcEBS = {0L};
    EBSBLK  ebVolEBS = {0L};
    EBSBLK  ebVocEBS = {0L};      
    EBSBLK  ebL16EBS = {0L};
    VOCBLK  vbVocBlk;                   /* Repeated call block          */
    CMPCBK  cbCmpCBk;                   /* Completion block             */
    DWORD   ulSrcAtD;
    DWORD   ulDstAtD;
    DWORD   ulSmpCnt;

    /********************************************************************/
    /********************************************************************/
    if (!ulBytReq || !PCMByt2Sl (lpFrqBlk->usSrcPCM, lpFrqBlk->usSrcChn, ulBytReq, NULL)) return (0L);

    /********************************************************************/
    /* Initialize / allocate vocoder function block                     */
    /********************************************************************/
    if (EffVocAlo (lpFrqBlk->bfFrcOBS ? MSTVocOBS : NULL, &vbVocBlk, NULL, 
      lpFrqBlk->usFFTOrd, lpFrqBlk->usWinOrd, lpFrqBlk->flSpdMul, 
      lpFrqBlk->flPchMul, lpFrqBlk->flSynThr, lpFrqBlk->ulSrcFrq)) {
        if (TBxGlo.usDebFlg & ERR___DEB) 
            MsgDspUsr ("Error allocating vocoder memory.\n");
        return (0);
    }

    /********************************************************************/
    /* Estimate total input to output transfer factor.                  */
    /* Queue latency requires output estimate technique.                */
    /********************************************************************/
    cbCmpCBk.pcMsgTxt = " (convert)";
    cbCmpCBk.ulInpCnt = 0L;
    cbCmpCBk.flInpFac = (float) 1 / PCMByt2Sl (lpFrqBlk->usSrcPCM, lpFrqBlk->usSrcChn, ulBytReq, NULL);
    cbCmpCBk.ulOutCnt = 0L;
    cbCmpCBk.flOutFac = (float) 1 / (PCMSmp2Bh (lpFrqBlk->usDstPCM, lpFrqBlk->usDstChn,
        PCMByt2Sl (lpFrqBlk->usSrcPCM, lpFrqBlk->usSrcChn, ulBytReq, NULL), NULL) 
        * lpFrqBlk->flSpdMul);

    /********************************************************************/
    /********************************************************************/
    while (TRUE) {
        /****************************************************************/
        /* Load Effects Buffer Stream with file PCM                     */
        /****************************************************************/
        ulSmpCnt = EffPtoEBS (sSrcHdl, &ulBytReq, &ebSrcEBS, (DWORD) -1L,
            lpFrqBlk->usSrcPCM, lpFrqBlk->usSrcChn, lpFrqBlk->usSrcBIO,
            lpFrqBlk->ulSrcFrq, pibSrcITC, PCMRd_G16);
        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("\nSmp:%4ld, ", ulSmpCnt);

        /****************************************************************/
        /* Volume Adjust / Normalize                                    */
        /****************************************************************/
        ulSmpCnt = EffVolAdj (&ebSrcEBS, &ebVolEBS, lpFrqBlk->flVolAdj);
        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("Adj:%4ld, ", ulSmpCnt);

        /****************************************************************/
        /* Vocode                                                       */
        /****************************************************************/
        ulSmpCnt = EffFFTVoc (&ebVolEBS, &ebVocEBS, &vbVocBlk, SubPolPrc, 
            (DWORD) ((CMPCBK FAR *) &cbCmpCBk));
        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("Voc:%4ld, ", ulSmpCnt);

        /****************************************************************/
        /* Normalize for output A to D resolution                       */
        /****************************************************************/
        if ((ulSrcAtD = PCMCapQry (lpFrqBlk->usSrcPCM, ATDRESQRY, 0L))
          != (ulDstAtD = PCMCapQry (lpFrqBlk->usDstPCM, ATDRESQRY, 0L))) {
            ulSmpCnt = EffGtoL16 (&ebVocEBS, &ebL16EBS, 
                (WORD) PCMCapQry (lpFrqBlk->usSrcPCM, ATDRESQRY, 0L)); 
            if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("L16:%4ld, ", ulSmpCnt);
            ulSmpCnt = EffL16toG (&ebL16EBS, &ebVocEBS, 
                (WORD) PCMCapQry (lpFrqBlk->usDstPCM, ATDRESQRY, 0L)); 
            if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("Gen:%4ld, ", ulSmpCnt);
        }

        /****************************************************************/
        /* Write PCM to file                                            */
        /****************************************************************/
        ulBytCnt = EffEBStoP (&ebVocEBS, (DWORD) -1L, sDstHdl, (DWORD) -1L,
            lpFrqBlk->usDstPCM, lpFrqBlk->usDstBIO, pibDstITC, PCMWr_PCM);
        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("PCM:%4ld\n", ulBytCnt);

        /****************************************************************/
        /****************************************************************/
        if (-1L == ulBytCnt) break;
        if ((0L == ulBytCnt) && ebSrcEBS.usEOSCod) {
            MsgDspPol (NULL, 100L, 100L);
            break;
        }

        /****************************************************************/
        /****************************************************************/
        cbCmpCBk.ulOutCnt += ulBytCnt;
        MsgDspPol (cbCmpCBk.pcMsgTxt, (DWORD) ((float) 1000 * cbCmpCBk.ulOutCnt * cbCmpCBk.flOutFac), 1000L);
    }

    /********************************************************************/
    /* Release filter function block                                    */
    /********************************************************************/
    EffVocRel (&vbVocBlk);

    /********************************************************************/
    /********************************************************************/
    if (TBxGlo.usDebFlg & MEM___DEB)
        MsgDspUsr ("Src:%ld, Voc:%ld\n", 
        (DWORD) ebSrcEBS.mhBufHdl, (DWORD) ebVocEBS.mhBufHdl);

    /********************************************************************/
    /********************************************************************/
    return (cbCmpCBk.ulOutCnt);

}

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL SubPolPrc (EFFPOL usPolReq, DWORD ulInpCnt, DWORD ulUsrPol)
{
    CMPCBK FAR *lpCmpCBk = (CMPCBK FAR *) ulUsrPol;

    /********************************************************************/
    /********************************************************************/
    switch (usPolReq) {
        case EFFPOLBEG:
            break;
        case EFFPOLCNT:
            MsgDspPol (lpCmpCBk->pcMsgTxt, 
                (DWORD) ((lpCmpCBk->flOutFac * lpCmpCBk->ulOutCnt * 1000L)
                + (lpCmpCBk->flInpFac * ulInpCnt * 1000L)), 1000L);
            break;
        case EFFPOLEND:
            break;
    }

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);

}    
