/************************************************************************/
/* Speed Adjust File Effects: FilEff.c                  V2.00  08/15/95 */
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
#include "..\..\pcmdev\genpcm.h"        /* PCM/APCM conv routine defs   */
#include "..\..\effdev\geneff.h"        /* Sound Effects definitions    */
#include "genspd.h"                     /* Speed Adjust support         */
#include "libsup.h"                     /* Speed Adjust lib supp        */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */
#include <math.h>                       /* Math library defs            */

/************************************************************************/
/*                          External References                         */
/************************************************************************/
extern  TBXGLO  TBxGlo;

/************************************************************************/
/************************************************************************/
typedef struct _CMPCBK {                /* Completion callback struct   */
    char *      pcMsgTxt;               /* Completion message text      */
    DWORD       ulInpCnt;               /* Input  count                 */
    float       flInpFac;               /* Input  completion factor     */
    DWORD       ulOutCnt;               /* Output count                 */
    float       flOutFac;               /* Output completion factor     */
    SPDPOLPRC   lpPolDsp;               /* Conversion poll display proc */
} CMPCBK;

/************************************************************************/
/*                          Forward References                          */
/************************************************************************/
WORD FAR PASCAL SubPolPrc (EFFPOL, DWORD, DWORD);

/************************************************************************/
/************************************************************************/
DWORD   SpdFilEff (short sSrcHdl, short sDstHdl, DWORD ulBytReq, 
        LPSTR lpWrkBuf, WORD usBufSiz, ITCBLK FAR *pibSrcITC, 
        ITCBLK FAR *pibDstITC, SPDBLK FAR *lpSpdBlk)
{

    DWORD   ulBytCnt;                   /* Write sample cnt proc / pass */
    EBSBLK  ebSrcEBS = {0L};
    EBSBLK  ebVolEBS = {0L};
    EBSBLK  ebVocEBS = {0L};      
//    EBSBLK  ebL16EBS = {0L};
    FFTBLK  fbFFTBlk = {0L, 0L, 0L};    /* FFT Filter repeated call blk */
    CMPCBK  cbCmpCBk;                   /* Completion block             */
//    DWORD   ulSrcAtD;
//    DWORD   ulDstAtD;
    DWORD   ulSmpCnt;

    /********************************************************************/
    /* Initialize error status                                          */
    /********************************************************************/
    lpSpdBlk->usCmpCod = 0;

    /********************************************************************/
    /* Anything to do?                                                  */
    /********************************************************************/
    if (!ulBytReq || !PCMByt2Sl (lpSpdBlk->usSrcPCM, lpSpdBlk->usSrcChn, 
        ulBytReq, NULL)) return (0L);

    /********************************************************************/
    /* Initialize poll callback display procedures                      */
    /********************************************************************/
    if (NULL == lpSpdBlk->lpPolDsp) lpSpdBlk->lpPolDsp = MsgDspPol;
    cbCmpCBk.lpPolDsp = lpSpdBlk->lpPolDsp;

//    /********************************************************************/
//    /* Initialize / allocate vocoder function block                     */
//    /********************************************************************/
//    if (EffVocAlo (lpSpdBlk->bfFrcOBS ? MSTVocOBS : NULL, &vbVocBlk, NULL, 
//      lpSpdBlk->usFFTOrd, lpSpdBlk->usWinOrd, lpSpdBlk->flSpdMul, 
//      lpSpdBlk->flPchMul, lpSpdBlk->flSynThr, lpSpdBlk->ulSrcFrq)) {
//        if (TBxGlo.usDebFlg & ERR___DEB) 
//            MsgDspUsr ("Error allocating vocoder memory.\n");
//        return (0);
//    }

    /********************************************************************/
    /* Initialize / allocate FFT filter function block                  */
    /********************************************************************/
    if (FFTFtrAlo (lpSpdBlk, &fbFFTBlk, SubPolPrc, (DWORD) (LPVOID) &cbCmpCBk)) {
        lpSpdBlk->usCmpCod = (WORD) -1;
        return (0L);
    }

    /********************************************************************/
    /* Estimate total input to output transfer factor.                  */
    /* Queue latency requires output estimate technique.                */
    /********************************************************************/
    cbCmpCBk.pcMsgTxt = " (speed adj)";
    cbCmpCBk.ulInpCnt = 0L;
    cbCmpCBk.flInpFac = (float) 1 / PCMByt2Sl (lpSpdBlk->usSrcPCM, lpSpdBlk->usSrcChn, ulBytReq, NULL);
    cbCmpCBk.ulOutCnt = 0L;
    cbCmpCBk.flOutFac = (float) 1 / (PCMSmp2Bh (lpSpdBlk->usDstPCM, lpSpdBlk->usDstChn,
        PCMByt2Sl (lpSpdBlk->usSrcPCM, lpSpdBlk->usSrcChn, ulBytReq, NULL), NULL) 
        * lpSpdBlk->flSpdMul);

    /********************************************************************/
    /********************************************************************/
    if (TBxGlo.usDebFlg & INI___DEB) {
      MsgDspUsr ("\nSrc: PCM:%4d, Chn:%4d, BIO:%4d, Frq:%4ld", lpSpdBlk->usSrcPCM,
        lpSpdBlk->usSrcChn, lpSpdBlk->usSrcBIO, lpSpdBlk->ulSrcFrq);
      MsgDspUsr ("\nDst: PCM:%4d, Chn:%4d, BIO:%4d, Frq:%4ld\n", lpSpdBlk->usDstPCM,
        lpSpdBlk->usDstChn, lpSpdBlk->usDstBIO, lpSpdBlk->ulDstFrq);
    }

    /********************************************************************/
    /********************************************************************/
    while (TRUE) {
        /****************************************************************/
        /* Load Effects Buffer Stream with file PCM                     */
        /****************************************************************/
        ulSmpCnt = EffPtoEBS (sSrcHdl, &ulBytReq, &ebSrcEBS, (DWORD) -1L,
            lpSpdBlk->usSrcPCM, lpSpdBlk->usSrcChn, lpSpdBlk->usSrcBIO,
            lpSpdBlk->ulSrcFrq, pibSrcITC, PCMRd_G16);
        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("\nSmp:%4ld, ", ulSmpCnt);

        /****************************************************************/
        /* Volume Adjust / Normalize                                    */
        /****************************************************************/
        ulSmpCnt = EffVolAdj (&ebSrcEBS, &ebVolEBS, lpSpdBlk->flVolAdj);
        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("Adj:%4ld, ", ulSmpCnt);

        /****************************************************************/
        /* Vocode                                                       */
        /****************************************************************/
//        ulSmpCnt = EffFFTVoc (&ebVolEBS, &ebVocEBS, &vbVocBlk, SubPolPrc, 
//            (DWORD) ((CMPCBK FAR *) &cbCmpCBk));
//        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("Voc:%4ld, ", ulSmpCnt);

        ulSmpCnt = EffFFTFtr (&ebVolEBS, &ebVocEBS, &fbFFTBlk, FFTFtrFnc, lpSpdBlk);
        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("FFT:%4ld, ", ulSmpCnt);

//        /****************************************************************/
//        /* Normalize for output A to D resolution                       */
//        /****************************************************************/
//        if ((ulSrcAtD = PCMCapQry (lpSpdBlk->usSrcPCM, ATDRESQRY, 0L))
//          != (ulDstAtD = PCMCapQry (lpSpdBlk->usDstPCM, ATDRESQRY, 0L))) {
//            ulSmpCnt = EffGtoL16 (&ebVocEBS, &ebL16EBS, 
//                (WORD) PCMCapQry (lpSpdBlk->usSrcPCM, ATDRESQRY, 0L)); 
//            if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("L16:%4ld, ", ulSmpCnt);
//            ulSmpCnt = EffL16toG (&ebL16EBS, &ebVocEBS, 
//                (WORD) PCMCapQry (lpSpdBlk->usDstPCM, ATDRESQRY, 0L)); 
//            if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("Gen:%4ld, ", ulSmpCnt);
//        }
//EffVolAtD (&ebResEBS, &ebGenEBS, usDstAtD);

        /****************************************************************/
        /* Write PCM to file                                            */
        /****************************************************************/
        ulBytCnt = EffEBStoP (&ebVocEBS, (DWORD) -1L, sDstHdl, (DWORD) -1L,
            lpSpdBlk->usDstPCM, lpSpdBlk->usDstBIO, pibDstITC, PCMWr_PCM);
        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("PCM:%4ld, ", ulBytCnt);
        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("Mem:%ldK\n", GloMemMax()/1024L);

        /****************************************************************/
        /* Error termination?                                           */
        /****************************************************************/
        if ((-1L == ulBytCnt) ||
            (FIOEOSERR == ebSrcEBS.usEOSCod) ||
            (FIOEOSERR == ebVolEBS.usEOSCod) ||
            (FIOEOSERR == ebVocEBS.usEOSCod)) {
                SPDERRMSG (SI_EFFEOSERR);
                lpSpdBlk->usCmpCod = (WORD) -1;
                cbCmpCBk.ulOutCnt = 0L;
                break;
        }

        /****************************************************************/
        /* Normal Termination?                                          */
        /****************************************************************/
        if ((0L == ulBytCnt) && ebSrcEBS.usEOSCod) {
            lpSpdBlk->lpPolDsp (cbCmpCBk.pcMsgTxt, 100L, 100L);
            break;
        }

        /****************************************************************/
        /****************************************************************/
        cbCmpCBk.ulOutCnt += ulBytCnt;
        if (!lpSpdBlk->lpPolDsp (cbCmpCBk.pcMsgTxt, (DWORD) 
            (1000. * cbCmpCBk.ulOutCnt * cbCmpCBk.flOutFac), 1000L)) break;

//        /****************************************************************/
//        /****************************************************************/
//        if (-1L == ulBytCnt) break;
//        if ((0L == ulBytCnt) && ebSrcEBS.usEOSCod) {
//            MsgDspPol (NULL, 100L, 100L);
//            break;
//        }
//
//        /****************************************************************/
//        /****************************************************************/
//        cbCmpCBk.ulOutCnt += ulBytCnt;
//        MsgDspPol (cbCmpCBk.pcMsgTxt, (DWORD) ((float) 1000 * cbCmpCBk.ulOutCnt * cbCmpCBk.flOutFac), 1000L);
    }

    /********************************************************************/
    /* Release filter function block                                    */
    /********************************************************************/
//    EffVocRel (&vbVocBlk);
FFTFtrRel (&fbFFTBlk);

    /********************************************************************/
    /********************************************************************/
//    if (TBxGlo.usDebFlg & MEM___DEB)
//        MsgDspUsr ("Src:%ld, Voc:%ld\n", 
//        (DWORD) ebSrcEBS.mhBufHdl, (DWORD) ebVocEBS.mhBufHdl);
if (TBxGlo.usDebFlg & MEM___DEB)
    MsgDspUsr ("Src:%lx, Adj:%lx, Voc:%lx, Mem:%ldK\n", 
    (DWORD) ebSrcEBS.mhBufHdl, (DWORD) ebVolEBS.mhBufHdl,  
    (DWORD) ebVocEBS.mhBufHdl, GloMemMax() / 1024L);

    /********************************************************************/
    /* Force release of all EBS blocks before exiting...                */
    /********************************************************************/
    GloAloRel (ebSrcEBS.mhBufHdl);
    GloAloRel (ebVolEBS.mhBufHdl);
    GloAloRel (ebVocEBS.mhBufHdl);
                                
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
            lpCmpCBk->lpPolDsp (lpCmpCBk->pcMsgTxt, 
                (DWORD) ((lpCmpCBk->flOutFac * lpCmpCBk->ulOutCnt * 1000)
                + (lpCmpCBk->flInpFac * ulInpCnt * 1000)), 1000L);
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