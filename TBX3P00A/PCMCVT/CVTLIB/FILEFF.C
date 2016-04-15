/************************************************************************/
/* File Conversion Effects: FilEff.c                    V2.00  01/10/93 */
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
typedef struct _CMPCBK {                /* Completion callback struct   */
    char *      pcMsgTxt;               /* Completion message text      */
    DWORD       ulInpCnt;               /* Input  count                 */
    float       flInpFac;               /* Input  completion factor     */
    DWORD       ulOutCnt;               /* Output count                 */
    float       flOutFac;               /* Output completion factor     */
    CVTPOLPRC   lpPolDsp;               /* Conversion poll display proc */
} CMPCBK;

/************************************************************************/
/*                          Forward References                          */
/************************************************************************/
WORD FAR PASCAL FSECBkPrc (FSEEVT, DWORD, DWORD);
WORD FAR PASCAL SubPolPrc (EFFPOL, DWORD, DWORD);

/************************************************************************/
/************************************************************************/
DWORD   CvtFilEff (short sSrcHdl, short sDstHdl, DWORD ulBytReq, 
        LPSTR lpWrkRsv, WORD usSizRsv, ITCBLK FAR *pibSrcITC, 
        ITCBLK FAR *pibDstITC, CVTBLK FAR *lpCvtBlk)
{

    DWORD   ulBytCnt;                   /* Write sample cnt proc / pass */
    EBSBLK  ebSrcEBS = {0L};
    EBSBLK  ebCrpEBS = {0L};
    EBSBLK  ebMixEBS = {0L};
    EBSBLK  ebVolEBS = {0L};
    EBSBLK  ebDCEEBS = {0L};
    EBSBLK  ebFtrEBS = {0L};      
    EBSBLK  ebResEBS = {0L};
    EBSBLK  ebGenEBS = {0L};
    RESBLK  rbResBlk = {0L, 0};         /* Int/Dec repeated call block  */
    FSEBLK  fbFSEBlk;                   /* Find Sound Event call block  */
    DCEBLK  dbDCEBlk;                   /* Cmp/Exp repeated call block  */
    FFTBLK  fbFFTBlk = {0L, 0L, 0L};    /* FFT Filter repeated call blk */
    TDOFTR  tfTDoFtr = {0L, 0L};        /* Time domain ftr rep call blk */
    CMPCBK  cbCmpCBk;                   /* Completion block             */
    FSEREC  frFSERec;                   /* Find Sound Event record      */
    float   flVolTot;                   /* Total volume adjust (dB)     */
    DWORD   ulSmpCnt;
    WORD    usDstAtD;

    /********************************************************************/
    /* Initialize error status                                          */
    /********************************************************************/
    lpCvtBlk->usCmpCod = 0;

    /********************************************************************/
    /* Anything to do?                                                  */
    /********************************************************************/
    if (!ulBytReq || !PCMByt2Sl (lpCvtBlk->usSrcPCM, lpCvtBlk->usSrcChn, 
        ulBytReq, NULL)) return (0L);

    /********************************************************************/
    /* Initialize total volume adjustment level                         */
    /********************************************************************/
    flVolTot = lpCvtBlk->flVolAdj;

    /********************************************************************/
    /* Initialize poll callback display procedures                      */
    /********************************************************************/
    if (NULL == lpCvtBlk->lpPolDsp) lpCvtBlk->lpPolDsp = MsgDspPol;
    cbCmpCBk.lpPolDsp = lpCvtBlk->lpPolDsp;
    frFSERec.ulUsrDat = (DWORD) lpCvtBlk->lpPolDsp;

    /********************************************************************/
    /* Scan file for normalization level                                */
    /********************************************************************/
    if (lpCvtBlk->usNrmLvl) {
        #define	NRMNOILVL (float) .05
        #define	NRMNOIEXC (float) .01
        float    flNrmAtt;
        cbCmpCBk.pcMsgTxt = " (normalize)";
        cbCmpCBk.ulInpCnt = 0L;
        cbCmpCBk.flInpFac = (float) 1 / (float) ulBytReq;
        cbCmpCBk.ulOutCnt = 0L;
        cbCmpCBk.flOutFac = 0;
        EffNrmLvl (sSrcHdl, ulBytReq, 
            NRMNOILVL, NRMNOIEXC, lpCvtBlk->usNrmLvl / (float) 100, 
            lpCvtBlk->flNrmExc / (float) 100, lpCvtBlk->usNrmMax, 
            lpCvtBlk->usSrcPCM, lpCvtBlk->usSrcChn, lpCvtBlk->usSrcBIO, 
            lpCvtBlk->ulSrcFrq, &flNrmAtt, pibSrcITC, SubPolPrc,
            (DWORD) ((CMPCBK FAR *) &cbCmpCBk), PCMRd_G16);
        flVolTot += flNrmAtt;
        /****************************************************************/
        if (TBxGlo.usDebFlg & INI___DEB) 
            MsgDspUsr ("\nAdj+Nrm: %f\n", flVolTot);
        lpCvtBlk->lpPolDsp (cbCmpCBk.pcMsgTxt, 100L, 100L);
    }

    /********************************************************************/
    /* Scan file for sound / silence sections                           */
    /********************************************************************/
    if (lpCvtBlk->flCrpFrm) {
        if (!EffFSEAlo (lpCvtBlk->flCrpFrm, lpCvtBlk->flCrpRes, 
          lpCvtBlk->flCrpSnd / (float) 100., lpCvtBlk->usCrpAtk / (float) 100.,
          lpCvtBlk->usCrpDcy / (float) 100., lpCvtBlk->flCrpGrd, lpCvtBlk->flCrpGrd, 
          CRPDCFDEF, lpCvtBlk->usSrcPCM, lpCvtBlk->usSrcChn, lpCvtBlk->usSrcBIO, 
          lpCvtBlk->ulSrcFrq, &fbFSEBlk)) {
            EffFSEEnu (sSrcHdl, ulBytReq, &fbFSEBlk, pibSrcITC, 
                FSECBkPrc, (DWORD) ((FSEREC FAR *) &frFSERec), PCMRd_G16);
            EffFSERel (&fbFSEBlk);
        }
        /****************************************************************/
        if (TBxGlo.usDebFlg & INI___DEB) 
            MsgDspUsr ("\nSound from %.2f to %.2f seconds\n", 
                frFSERec.ulBegPos / (float) lpCvtBlk->ulSrcFrq, 
                frFSERec.ulEndPos / (float) lpCvtBlk->ulSrcFrq);
    }

    /********************************************************************/
    /* Initialize / allocate Dynamic Compressor/Expander block          */
    /********************************************************************/
    EffDCEAlo (lpCvtBlk->usSrcPCM, lpCvtBlk->usSrcChn, lpCvtBlk->ulSrcFrq,
      lpCvtBlk->usCmpThr / (float) 100, lpCvtBlk->usCmpAtk, lpCvtBlk->flCmpMin,
      lpCvtBlk->usExpThr / (float) 100, lpCvtBlk->usExpAtk, lpCvtBlk->flExpMax,
      lpCvtBlk->usCE_Dcy, lpCvtBlk->usNoiThr / (float) 100, lpCvtBlk->flNoiAtt, 
      &dbDCEBlk);

    /********************************************************************/
    /* Initialize / allocate FFT filter function block                  */
    /********************************************************************/
    if (FFTFtrAlo (lpCvtBlk, &fbFFTBlk, SubPolPrc, (DWORD) (LPVOID) &cbCmpCBk)) {
        lpCvtBlk->usCmpCod = (WORD) -1;
        return (0L);
    }

    /********************************************************************/
    /* Initialize / allocate 2 pole filter function block               */
    /********************************************************************/
    if (!lpCvtBlk->bfFFTAAF && lpCvtBlk->usTDLAAF) {
        WORD    usSecCnt = Q2_SECCNT;
        WORD    usSecOrd = Q2_SECORD;
        if (AAF_Q6TYP == lpCvtBlk->usTDLAAF) {
            usSecCnt = Q6_SECCNT;
            usSecOrd = Q6_SECORD;
        }
        if (EffIIRAlo (EFFIIRCH2, EFFPASLOW, usSecCnt, usSecOrd, 
          lpCvtBlk->ulDstFrq / 2L, 0L, AAFPASFAC, AAFSTPATT, 
          lpCvtBlk->ulSrcFrq, lpCvtBlk->bfTDLFst, &tfTDoFtr)) {
            CVTERRMSG ("Error setting time domain filter parameters.\n");
            lpCvtBlk->usCmpCod = (WORD) -1;
            return (0L);
        }
    } 

    /********************************************************************/
    /* Estimate total input to output transfer factor.                  */
    /* Queue latency requires output estimate technique.                */
    /********************************************************************/
    cbCmpCBk.pcMsgTxt = " (convert)";
    cbCmpCBk.ulInpCnt = 0L;
    cbCmpCBk.flInpFac = (float) 1 / PCMByt2Sl (lpCvtBlk->usSrcPCM, lpCvtBlk->usSrcChn, ulBytReq, NULL);
    cbCmpCBk.ulOutCnt = 0L;
    cbCmpCBk.flOutFac = (float) 1 / (PCMSmp2Bh (lpCvtBlk->usDstPCM, lpCvtBlk->usDstChn,
        PCMByt2Sl (lpCvtBlk->usSrcPCM, lpCvtBlk->usSrcChn, ulBytReq, NULL), NULL) 
        * ((float) lpCvtBlk->ulDstFrq * lpCvtBlk->usDstChn) /
          ((float) lpCvtBlk->ulSrcFrq * lpCvtBlk->usSrcChn));

    /********************************************************************/
    /* Initialize destination A to D resolution                         */
    /********************************************************************/
    usDstAtD = (WORD) PCMCapQry (lpCvtBlk->usDstPCM, ATDRESQRY, 0L);

    /********************************************************************/
    /********************************************************************/
    if (TBxGlo.usDebFlg & INI___DEB) {
      MsgDspUsr ("\nSrc: PCM:%4d, Chn:%4d, BIO:%4d, Frq:%4ld", lpCvtBlk->usSrcPCM,
        lpCvtBlk->usSrcChn, lpCvtBlk->usSrcBIO, lpCvtBlk->ulSrcFrq);
      MsgDspUsr ("\nDst: PCM:%4d, Chn:%4d, BIO:%4d, Frq:%4ld\n", lpCvtBlk->usDstPCM,
        lpCvtBlk->usDstChn, lpCvtBlk->usDstBIO, lpCvtBlk->ulDstFrq);
    }

    /********************************************************************/
    /********************************************************************/
    while (TRUE) {
        /****************************************************************/
        /* Load Effects Buffer Stream with file PCM                     */
        /****************************************************************/
        ulSmpCnt = EffPtoEBS (sSrcHdl, &ulBytReq, &ebSrcEBS, (DWORD) -1L,
            lpCvtBlk->usSrcPCM, lpCvtBlk->usSrcChn, lpCvtBlk->usSrcBIO,
            lpCvtBlk->ulSrcFrq, pibSrcITC, PCMRd_G16);
        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("\nSmp:%4ld, ", ulSmpCnt);

        /****************************************************************/
        /* Auto Crop (if requested)                                     */
        /****************************************************************/
        ulSmpCnt = EffFSEChp (&ebSrcEBS, &ebCrpEBS, lpCvtBlk->flCrpFrm ? 1 : 0, &frFSERec);
        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("\nCrp:%4ld, ", ulSmpCnt);

        /****************************************************************/
        /* Mix or separate channels                                     */
        /****************************************************************/
        ulSmpCnt = EffChXtoM (&ebCrpEBS, &ebMixEBS, lpCvtBlk->usSrcChn);
        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("Mix:%4ld, ", ulSmpCnt);

        /****************************************************************/
        /* Volume Adjust / Normalize                                    */
        /****************************************************************/
        ulSmpCnt = EffVolAdj (&ebMixEBS, &ebVolEBS, flVolTot);
        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("Adj:%4ld, ", ulSmpCnt);

        /****************************************************************/
        /* Dynamic Compress / Expand                                    */
        /****************************************************************/
        ulSmpCnt = EffVolDCE (&ebVolEBS, &ebDCEEBS, &dbDCEBlk);
        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("DCE:%4ld, ", ulSmpCnt);

        /****************************************************************/
        /* Filter (if requested / necessary)                            */
        /****************************************************************/
        if (lpCvtBlk->bfFFTAAF || lpCvtBlk->bfFFTDTM 
          || lpCvtBlk->bfFFTFEq || lpCvtBlk->bfFFTRes) {
            ulSmpCnt = EffFFTFtr (&ebDCEEBS, &ebFtrEBS, &fbFFTBlk, FFTFtrFnc, lpCvtBlk);
            if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("FFT:%4ld, ", ulSmpCnt);
        } else if (lpCvtBlk->usTDLAAF) {
            ulSmpCnt = EffIIRFtr (&ebDCEEBS, &ebFtrEBS, &tfTDoFtr);
            if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("TDL:%4ld, ", ulSmpCnt);
        }
        else {
            ulSmpCnt = EffEBStoE (&ebDCEEBS, &ebFtrEBS, (DWORD) -1L);
            if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("Ftr:%4ld, ", ulSmpCnt);
        }

        /****************************************************************/
        /* Re-sample if required                                        */
        /****************************************************************/
        ulSmpCnt = EffFrqRes (&ebFtrEBS, &ebResEBS, lpCvtBlk->ulDstFrq,
            &rbResBlk);
        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("Res:%4ld, ", ulSmpCnt);

        /****************************************************************/
        /* Normalize for output A to D resolution                       */
        /****************************************************************/
        EffVolAtD (&ebResEBS, &ebGenEBS, usDstAtD);

        /****************************************************************/
        /* Write PCM to file; use ulDstLen to limit PCM output          */
        /****************************************************************/
//      ulBytCnt = EffEBStoP (&ebGenEBS, (DWORD) -1L, sDstHdl, (DWORD) -1L, 
//          lpCvtBlk->usDstPCM, lpCvtBlk->usDstBIO, pibDstITC, PCMWr_PCM);
// 09-12-96
        ulBytCnt = EffEBStoP (&ebGenEBS, (DWORD) -1L, sDstHdl, 
            (lpCvtBlk->ulDstLen - min (lpCvtBlk->ulDstLen, cbCmpCBk.ulOutCnt)),
            lpCvtBlk->usDstPCM, lpCvtBlk->usDstBIO, pibDstITC, PCMWr_PCM);

        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("PCM:%4ld, ", ulBytCnt);
        if (TBxGlo.usDebFlg & MEM___DEB) MsgDspUsr ("Mem:%ldK\n", GloMemMax()/1024L);

        /****************************************************************/
        /* Error termination?                                           */
        /****************************************************************/
        if ((-1L == ulBytCnt) ||
            (FIOEOSERR == ebSrcEBS.usEOSCod) ||
            (FIOEOSERR == ebCrpEBS.usEOSCod) ||
            (FIOEOSERR == ebMixEBS.usEOSCod) ||
            (FIOEOSERR == ebVolEBS.usEOSCod) ||
            (FIOEOSERR == ebDCEEBS.usEOSCod) ||
            (FIOEOSERR == ebFtrEBS.usEOSCod) ||     
            (FIOEOSERR == ebResEBS.usEOSCod) ||
            (FIOEOSERR == ebGenEBS.usEOSCod)) {
                CVTERRMSG (SI_EFFEOSERR);
                lpCvtBlk->usCmpCod = (WORD) -1;
                cbCmpCBk.ulOutCnt = 0L;
                break;
        }

        /****************************************************************/
        /* Normal Termination?                                          */
        /****************************************************************/
        if ((0L == ulBytCnt) && ebSrcEBS.usEOSCod) {
            lpCvtBlk->lpPolDsp (cbCmpCBk.pcMsgTxt, 100L, 100L);
            break;
        }

        /****************************************************************/
        /****************************************************************/
        cbCmpCBk.ulOutCnt += ulBytCnt;
        if (!lpCvtBlk->lpPolDsp (cbCmpCBk.pcMsgTxt, (DWORD) 
            (1000. * cbCmpCBk.ulOutCnt * cbCmpCBk.flOutFac), 1000L)) break;
    }

    /********************************************************************/
    /* Release filter function block                                    */
    /********************************************************************/
    EffDCERel (&dbDCEBlk);
    FFTFtrRel (&fbFFTBlk);
    EffIIRRel (&tfTDoFtr);

    /********************************************************************/
    /********************************************************************/
    if (TBxGlo.usDebFlg & MEM___DEB)
        MsgDspUsr ("Src:%lx, Crp:%lx, Mix:%lx, Adj:%lx, DCE:%lx, Ftr:%lx, Res:%lx, Gen:%lx, Mem:%ldK\n", 
        (DWORD) ebSrcEBS.mhBufHdl, (DWORD) ebCrpEBS.mhBufHdl, 
        (DWORD) ebMixEBS.mhBufHdl, (DWORD) ebVolEBS.mhBufHdl,  
        (DWORD) ebDCEEBS.mhBufHdl, (DWORD) ebFtrEBS.mhBufHdl,  
        (DWORD) ebResEBS.mhBufHdl, (DWORD) ebGenEBS.mhBufHdl,
        GloMemMax() / 1024L);

    /********************************************************************/
    /* Force release of all EBS blocks before exiting...                */
    /********************************************************************/
    GloAloRel (ebSrcEBS.mhBufHdl);
    GloAloRel (ebCrpEBS.mhBufHdl);
    GloAloRel (ebMixEBS.mhBufHdl);
    GloAloRel (ebVolEBS.mhBufHdl);
    GloAloRel (ebDCEEBS.mhBufHdl);
    GloAloRel (ebFtrEBS.mhBufHdl);
    GloAloRel (ebResEBS.mhBufHdl);
    GloAloRel (ebGenEBS.mhBufHdl);
                                
    /********************************************************************/
    /********************************************************************/
    return (cbCmpCBk.ulOutCnt);

}

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL FSECBkPrc (FSEEVT usPolReq, DWORD ulInpCnt, DWORD ulUsrPol)
{
    LPFSER          lpFSERec = (LPFSER) ulUsrPol;
    static DWORD    ulTotCnt;

    /********************************************************************/
    /* Future: Convert lpFSERec->ulUsrDat to block containing ulTotCnt  */
    /********************************************************************/
    switch (usPolReq) {
        case EFFFSEINI:                         /* Initialization event */
            ulTotCnt = ulInpCnt;
            lpFSERec->ulBegPos = (DWORD) -1L; 
            lpFSERec->ulEndPos = (DWORD) -1L; 
            break;
        case EFFFSEPOL:                         /* No sound event       */
            ((CVTPOLPRC) lpFSERec->ulUsrDat) (" (sound scan)", ulInpCnt, ulTotCnt);
            break;
        case EFFFSESND:                         /* Change to snd event  */
            if (-1L == lpFSERec->ulBegPos) lpFSERec->ulBegPos = ulInpCnt; 
            if (TBxGlo.usDebFlg & INI___DEB) 
                MsgDspUsr ("\nSnd Evt at %ld samples", ulInpCnt);
            break;
        case EFFFSESIL:                         /* Change to sil event  */
            lpFSERec->ulEndPos = ulInpCnt; 
            if (TBxGlo.usDebFlg & INI___DEB) 
                MsgDspUsr ("\nSil Evt at %ld samples", ulInpCnt);
            break;
        case EFFFSETRM:                         /* Termination event    */
            ((CVTPOLPRC) lpFSERec->ulUsrDat) (" (sound scan)", 100L, 100L);
            break;
    }

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);
}

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