/************************************************************************/
/* Sound Chop File Loader: FilLod.c                     V2.00  11/10/93 */
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
#include "..\..\fiodev\genfio.h"        /* Generic File I/O definitions */
#include "..\..\fiodev\filutl.h"        /* File utilities definitions   */
#include "..\..\fiodev\wavfil.h"        /* Wave file (DOS) definitions  */
#include "..\..\fiodev\visfil.h"        /* VIS Interchange definitions  */
#include "..\..\pcmdev\genpcm.h"        /* PCM/APCM conv routine defs   */
#include "..\..\effdev\geneff.h"        /* Sound Effects definitions    */
#include "..\..\regdev\tbxreg.h"        /* User Registration defs       */
#include "..\..\os_dev\cfgsup.h"        /* Configuration support        */
#include "genchp.h"                     /* PCM bytes replacement supp   */
#include "libsup.h"                     /* PCM conversion lib supp      */

#include <stdio.h>                      /* Standard I/O                 */
#include <io.h>                         /* Low level file I/O           */
#include <fcntl.h>                      /* Flags used in open/ sopen    */
#include <sys\types.h>                  /* File status and time types   */
#include <sys\stat.h>                   /* File status types            */
#include <string.h>                     /* String manipulation funcs    */
#include <stdlib.h>                     /* Misc string/math/error funcs */

/************************************************************************/
/*                          External References                         */
/************************************************************************/
extern  TBXGLO  TBxGlo;

/************************************************************************/
/*                          Forward References                          */
/************************************************************************/
DWORD            SrcFilPrm (short, CHPBLK FAR *, MMCKINFO FAR *, VISMEMHDL FAR *, ITCBLK FAR * FAR *);
DWORD            DstFilPrm (short, CHPBLK FAR *, MMCKINFO FAR *, VISMEMHDL FAR *, ITCBLK FAR * FAR *);
DWORD FAR PASCAL ChpFilCop (short, DWORD, short, LPSTR, WORD, DWORD);
WORD  FAR PASCAL FSECBkPrc (FSEEVT, DWORD, DWORD);

/************************************************************************/
/************************************************************************/
typedef struct _CHPLST {
    WORD        usMaxCnt;               /* Sound event max count        */
    WORD        usRecCnt;               /* Sound event record count     */
    CHPPOLPRC   lpPolDsp;               /* Conversion poll display proc */
    DWORD       ulPolTot;               /* Poll display total count     */
    FSEREC      frFSERec[];             /* Find Sound Event record      */
} CHPLST;

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL ChpSndFil (short sSrcHdl, LPSTR szSrcRsv, CHPOPNCBK lpOpnCBk, 
                 CHPCOPCBK lpCopCBk, CHPCLSCBK lpClsCBk, DWORD ulCBkDat, 
                 CHPBLK FAR *lpChpBlk)
{
    VISMEMHDL   mhWrkBuf;
    VISMEMHDL   mhChpLst;
    CHPLST FAR *lpChpLst;
    LPVOID      lpWrkBuf;
    VISMEMHDL   hibSrcITC;
    ITCBLK FAR *pibSrcITC;
    MMCKINFO    ciSrcChk;
    FSEBLK      fbFSEBlk;               /* Find Sound Event call block  */
    DWORD       ulBytReq;               /* Bytes requested total        */
    WORD        usBufSiz;
    DWORD       ulXfrCnt = 0L;
    WORD        usi;

    /********************************************************************/
    /* Initialize error status                                          */
    /********************************************************************/
    lpChpBlk->usCmpCod = 0;

    /********************************************************************/
    /* Check for valid destination file open and close routines         */
    /********************************************************************/
    if ((NULL == lpOpnCBk) || (NULL == lpClsCBk)) {
        lpChpBlk->usCmpCod = (WORD) -1;
        return (ulXfrCnt);
    }
    if (NULL == lpCopCBk) lpCopCBk = ChpFilCop;

    /********************************************************************/
    /* Allocate chop list memory in multiples of 1024 and < 32Kbyte     */
    /********************************************************************/
    if (NULL == (lpChpLst = GloAloLck (GMEM_MOVEABLE, &mhChpLst, 
      (WORD) min ((GloMemMax () / 1024L) * 1024L, 0x8000L)))) {
        lpChpBlk->usCmpCod = (WORD) -1;
        return (ulXfrCnt);
    }
    lpChpLst->usMaxCnt = (lpChpLst->usMaxCnt - sizeof (CHPLST)) / sizeof (FSEREC);
    lpChpLst->usRecCnt = 0;

    /********************************************************************/
    /* Allocate work buffer memory in multiples of 1024 and < 32Kbyte   */
    /********************************************************************/
    usBufSiz = (WORD) min ((GloMemMax () / 1024L) * 1024L, 0x8000L);
    if (NULL == (lpWrkBuf = GloAloLck (GMEM_MOVEABLE, &mhWrkBuf, usBufSiz))) {
        lpChpBlk->usCmpCod = (WORD) -1;
        return (ulXfrCnt);
    }

    /********************************************************************/
    /* Set file input & output conversion parameters                    */
    /********************************************************************/
    if (!(ulBytReq = SrcFilPrm (sSrcHdl, lpChpBlk, &ciSrcChk, &hibSrcITC, &pibSrcITC))) {
        lpChpBlk->usCmpCod = (WORD) -1;
        return (ulXfrCnt);
    }

    /********************************************************************/
    /* Initialize poll callback display procedures                      */
    /********************************************************************/
    if (NULL == lpChpBlk->lpPolDsp) lpChpBlk->lpPolDsp = MsgDspPol;
    lpChpLst->lpPolDsp = lpChpBlk->lpPolDsp;

    /********************************************************************/
    /* Scan file for sound / silence sections                           */
    /********************************************************************/
    if (!EffFSEAlo (lpChpBlk->flChpFrm, lpChpBlk->flChpRes, 
      lpChpBlk->flChpSnd / (float) 100., lpChpBlk->usChpAtk / (float) 100.,
      lpChpBlk->usChpDcy / (float) 100., lpChpBlk->flChpGrd, lpChpBlk->flChpGrd,
      CHPDCFDEF, lpChpBlk->usSrcPCM, lpChpBlk->usSrcChn, lpChpBlk->usSrcBIO, 
      lpChpBlk->ulSrcFrq, &fbFSEBlk)) {
        EffFSEEnu (sSrcHdl, ulBytReq, &fbFSEBlk, pibSrcITC, 
            FSECBkPrc, (DWORD) lpChpLst, PCMRd_G16);
        EffFSERel (&fbFSEBlk);
    }
    else {
        CHPERRMSG ("Error setting Sound Scan parameters.\n");
    }

    /********************************************************************/
    /* Set demo limits                                                  */
    /********************************************************************/
    if (REGDEMKEY == TBxGlo.usRegKey) {
        lpChpLst->usRecCnt = min (lpChpLst->usRecCnt, DEMRECDEF);
        if (TBxGlo.usDebFlg & INI___DEB) 
            MsgDspUsr ("\nusRecCnt: %d ", lpChpLst->usRecCnt);
    }

    /********************************************************************/
    /********************************************************************/
    for (usi=0; usi < lpChpLst->usRecCnt; usi++) {
        short   sDstHdl;
        DWORD   ulBytBeg = PCMSmp2Bh (lpChpBlk->usSrcPCM, lpChpBlk->usSrcChn, 
            lpChpLst->frFSERec[usi].ulBegPos, NULL);
        DWORD   ulBytEnd = PCMSmp2Bh (lpChpBlk->usSrcPCM, lpChpBlk->usSrcChn, 
            lpChpLst->frFSERec[usi].ulEndPos, NULL);

        /****************************************************************/
        /* Position source file                                         */
        /****************************************************************/
        FilSetPos (sSrcHdl, ulBytBeg, SEEK_SET);

        /****************************************************************/
        /* Ask user to open a file                                      */
        /****************************************************************/
        if (!lpOpnCBk (&sDstHdl, ulCBkDat)) break;

        /****************************************************************/
        /* Transfer the specified byte range to the user-opened file    */
        /****************************************************************/
        if (-1 != sDstHdl) ulXfrCnt += lpCopCBk (sSrcHdl, ulBytEnd - ulBytBeg,
            sDstHdl, lpWrkBuf, usBufSiz, (DWORD) lpChpBlk);

        /****************************************************************/
        /****************************************************************/
        if (!lpClsCBk (sDstHdl, ulCBkDat)) break;
    }

    /********************************************************************/
    /* Release any PCM conversion memory                                */
    /********************************************************************/
    PCMRelITC (lpChpBlk->usSrcPCM, pibSrcITC);

    /********************************************************************/
    /* Release any source header memory                                 */
    /********************************************************************/
    if (WAVHDRFMT == lpChpBlk->usSrcFmt) {
        hibSrcITC = GloUnLRel (hibSrcITC);
    }

    /********************************************************************/
    /********************************************************************/
    GloUnLRel (mhWrkBuf);
    GloUnLRel (mhChpLst);

    /********************************************************************/
    /********************************************************************/
    return (ulXfrCnt);

}

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL ChpFilCop (short sSrcHdl, DWORD ulBytCnt, short sDstHdl, 
        LPSTR lpWrkBuf, WORD usBufSiz, DWORD ulCBkDat)
{
    MMCKINFO    ciDstChk;
    VISMEMHDL   hibDstITC;
    ITCBLK FAR *pibDstITC;
    DWORD       ulXfrCnt;
    unsigned    uiErrCod;
    CHPBLK FAR *lpChpBlk = (CHPBLK FAR *) ulCBkDat;

    /********************************************************************/
    /********************************************************************/
    if (DstFilPrm (sDstHdl, lpChpBlk, &ciDstChk, &hibDstITC, &pibDstITC)) 
        return (0L);

    /********************************************************************/
    /* Copy vox portion of file                                         */
    /********************************************************************/
    ulXfrCnt = FilCop (sSrcHdl, sDstHdl, lpWrkBuf, usBufSiz, ulBytCnt, 
        FIOENCNON, &uiErrCod, NULL, 0L);

    /********************************************************************/
    /********************************************************************/
    if (WAVHDRFMT == lpChpBlk->usDstFmt) 
        WIOOutEnd (sDstHdl, &hibDstITC, &ciDstChk);

    /********************************************************************/
    /********************************************************************/
    return (ulXfrCnt);

}

/************************************************************************/
/************************************************************************/
#define HDRHDL VISMEMHDL
typedef struct  _HDRBLK {
    WORD        usFilFmt;
    MMCKINFO    ciChkInf;
    VISMEMHDL   hibITCBlk;
} HDRBLK;
WORD FAR PASCAL ChpHdrCre (short sFilHdl, WORD usFilFmt, WORD usPCMTyp, 
                            WORD usChnCnt, DWORD ulSrcFrq, HDRHDL far *phHdrBlk)
{
    HDRBLK FAR *lpHdrBlk;
    WORD        usErrCod = 0;

    /********************************************************************/
    /********************************************************************/
    if (NULL == (lpHdrBlk = GloAloLck (GMEM_MOVEABLE, phHdrBlk, 
        sizeof (*lpHdrBlk)))) return ((WORD) -1);

    /********************************************************************/
    /********************************************************************/
//    lpChpBlk->usDstFmt = 
//    lpChpBlk->usSrcPCM = 
//    lpChpBlk->usSrcChn = 
//    lpChpBlk->ulSrcFrq = 

    /********************************************************************/
    /********************************************************************/
//    usErrCod = DstFilPrm (sFilHdl, lpChpBlk, &ciDstChk, &hibDstITC, &pibDstITC); 

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (*phHdrBlk);
    return (usErrCod);
}

WORD FAR PASCAL ChpHdrCls (short sDstHdl, HDRHDL mhHdrBlk)
{
    HDRBLK FAR *lpHdrBlk;
    WORD        usErrCod = 0;

    /********************************************************************/
    /********************************************************************/
    if (NULL == (lpHdrBlk = GloMemLck (mhHdrBlk))) return ((WORD) -1);

    /********************************************************************/
    /********************************************************************/
    if (WAVHDRFMT == lpHdrBlk->usFilFmt) 
        WIOOutEnd (sDstHdl, &(lpHdrBlk->hibITCBlk), &(lpHdrBlk->ciChkInf));

    /********************************************************************/
    /********************************************************************/
    GloUnLRel (mhHdrBlk);
    return (usErrCod);
}

/************************************************************************/
/************************************************************************/
DWORD   SrcFilPrm (short sSrcHdl, CHPBLK FAR *lpChpBlk, MMCKINFO FAR *pciSrcChk, 
        VISMEMHDL FAR *phibSrcITC, ITCBLK FAR * FAR *ppibSrcITC)
{

    DWORD           ulBytReq;           /* Bytes requested total        */
    MMCKINFO        ciSrcChk;
    static  ITCBLK  ibSrcITC;

    /********************************************************************/
    /* Re-position file pointer (if requested)                          */
    /********************************************************************/
    if (-1L == FilSetPos (sSrcHdl, lpChpBlk->ulSrcOff, SEEK_SET)) return (0L);
    
    /********************************************************************/
    /* Set input conversion parameters                                  */
    /********************************************************************/
    switch (lpChpBlk->usSrcFmt) {
        /****************************************************************/
        /****************************************************************/
        case ALLPURFMT:
            if (-1L == (ulBytReq = FilGetLen (sSrcHdl))) return (0L);
            *phibSrcITC = (VISMEMHDL) NULL;
            PCMAloITC (lpChpBlk->usSrcPCM, (*ppibSrcITC = &ibSrcITC), NULL);
            break;

        /****************************************************************/
        /****************************************************************/
        case VISHDRFMT:
            if (VIXFndFst (sSrcHdl, "WAVE", 0L, &pciSrcChk->dwDataOffset, FIOENCNON)) {
                CHPERRMSG ("has an unknown file format, skipping...\n");
                return (0L);
            }
            VIXDesCur (sSrcHdl, &pciSrcChk->cksize, FIOENCNON);
            if (VIXLodTyp (sSrcHdl, &lpChpBlk->usSrcPCM, &lpChpBlk->usSrcChn, 
              &lpChpBlk->ulSrcFrq, FIOENCNON)) {
                CHPERRMSG ("has an unknown file format, skipping...\n");
                return (0L);
            }
            VIXFndNxt (sSrcHdl, 0L, FIOENCNON);
            VIXFndNxt (sSrcHdl, 0L, FIOENCNON);

            *phibSrcITC = (VISMEMHDL) NULL;
            PCMAloITC (lpChpBlk->usSrcPCM, (*ppibSrcITC = &ibSrcITC), NULL);
            break;

        /****************************************************************/
        /****************************************************************/
        case WAVHDRFMT:
            if (WIOFndNxt (sSrcHdl, &lpChpBlk->usSrcPCM, phibSrcITC, &ciSrcChk)) {
                CHPERRMSG ("is not a Wave Multimedia Format File, skipping...\n");
                return (0L);
            }
            if (UNKPCM000 == lpChpBlk->usSrcPCM) {
                CHPERRMSG ("has unknown Wave Multimedia PCM type, skipping...\n");
                return (0L);
            }
            if (NULL == (*ppibSrcITC = GloMemLck (*phibSrcITC))) {
                CHPERRMSG ("encountered a memory locking error, skipping...\n");
                return (0L);
            }
            ulBytReq = ciSrcChk.cksize;
            lpChpBlk->usSrcChn = (*ppibSrcITC)->mwMSW.afWEX.wfx.nChannels;
            lpChpBlk->ulSrcFrq = (*ppibSrcITC)->mwMSW.afWEX.wfx.nSamplesPerSec;
            break;

        /****************************************************************/
        /****************************************************************/
        default:
            CHPERRMSG ("has an unknown file format, skipping...\n");
            return (0L);
    }

    /********************************************************************/
    /********************************************************************/
    return (min (ulBytReq, lpChpBlk->ulSrcLen));

}

DWORD   DstFilPrm (short sDstHdl, CHPBLK FAR *lpChpBlk, MMCKINFO FAR *pciDstChk,
        VISMEMHDL FAR *phibDstITC, ITCBLK FAR * FAR *ppibDstITC)
{
    #define CHPDSTPCM   lpChpBlk->usSrcPCM  
    #define CHPDSTCHN   lpChpBlk->usSrcChn  
    #define CHPDSTFRQ   lpChpBlk->ulSrcFrq  
    static  ITCBLK      ibDstITC;

    /********************************************************************/
    /********************************************************************/
    switch (lpChpBlk->usDstFmt) {
        case ALLPURFMT:
            *phibDstITC = (VISMEMHDL) NULL;
            PCMAloITC (CHPDSTPCM, (*ppibDstITC = &ibDstITC), NULL);
            break;

        /****************************************************************/
        /****************************************************************/
        case VISHDRFMT:
            if (VIXWrtFst (sDstHdl, "WAVE", 0L, &pciDstChk->dwDataOffset, FIOENCNON) ||
              VIXWrtTyp (sDstHdl, CHPDSTPCM, CHPDSTCHN, 
              CHPDSTFRQ, FIOENCNON) || 
              VIXWrtDat (sDstHdl, &pciDstChk->cksize, FIOENCNON)) {
                CHPERRMSG ("encountered a file output error, skipping...\n");
                return (0L);
            }
            *phibDstITC = (VISMEMHDL) NULL;
            PCMAloITC (CHPDSTPCM, (*ppibDstITC = &ibDstITC), NULL);
            break;

        /****************************************************************/
        /****************************************************************/
        case WAVHDRFMT:
            if (WIOOutIni (sDstHdl, CHPDSTPCM, CHPDSTCHN,
              CHPDSTFRQ, phibDstITC, pciDstChk)) {
                CHPERRMSG ("encountered a file output error, skipping...\n");
                return ((DWORD) -1L);
            }
            if (NULL == (*ppibDstITC = GloMemLck (*phibDstITC))) {
                CHPERRMSG ("encountered a memory locking error, skipping...\n");
                return ((DWORD) -1L);
            }
            break;

        /****************************************************************/
        /****************************************************************/
        default:
            CHPERRMSG ("has an unknown file format, skipping...\n");
            return ((DWORD) -1L);
    }

    /********************************************************************/
    /********************************************************************/
    return (0L);
  
}

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL FSECBkPrc (FSEEVT usPolReq, DWORD ulInpCnt, DWORD ulUsrPol)
{
    CHPLST FAR *    lpChpLst = (CHPLST FAR *) ulUsrPol;

    /********************************************************************/
    /********************************************************************/
    switch (usPolReq) {
        case EFFFSEINI:                         /* Initialization event */
            lpChpLst->ulPolTot = ulInpCnt;
            break;
        case EFFFSEPOL:                         /* No sound event       */
            lpChpLst->lpPolDsp (" (sound scan)", ulInpCnt, lpChpLst->ulPolTot);
            break;
        case EFFFSESND:                         /* Change to snd event  */
            lpChpLst->frFSERec[lpChpLst->usRecCnt].ulBegPos = ulInpCnt; 
            lpChpLst->frFSERec[lpChpLst->usRecCnt].ulEndPos = (DWORD) -1L;
            if (TBxGlo.usDebFlg & INI___DEB) 
                MsgDspUsr ("Snd Evt at %ld samples\n", ulInpCnt);
            break;
        case EFFFSESIL:                         /* Change to sil event  */
            lpChpLst->frFSERec[lpChpLst->usRecCnt].ulEndPos = ulInpCnt; 
            if (TBxGlo.usDebFlg & INI___DEB) 
                MsgDspUsr ("Sil Evt at %ld samples\n", ulInpCnt);
            if (lpChpLst->usRecCnt < lpChpLst->usMaxCnt) lpChpLst->usRecCnt++;
            else return (FALSE);
            break;
        case EFFFSETRM:                         /* Termination event    */
            lpChpLst->lpPolDsp (NULL, 100L, 100L);
            break;
    }

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);
}


