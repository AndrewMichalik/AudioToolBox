/************************************************************************/
/* Frequency Effects File Loader: FilLod.c              V2.00  10/10/93 */
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
#include "..\fiodev\genfio.h"           /* Generic File I/O definitions */
#include "..\fiodev\filutl.h"           /* File utilities definitions   */
#include "..\fiodev\wavfil.h"           /* Wave file (DOS) definitions  */
#include "..\fiodev\visfil.h"           /* VIS Interchange definitions  */
#include "..\pcmdev\genpcm.h"           /* PCM/APCM conv routine defs   */
#include "..\effdev\geneff.h"           /* Sound Effects definitions    */
#include "..\regdev\tbxreg.h"           /* User Registration defs       */
#include "..\os_dev\cfgsup.h"           /* Configuration support        */
#include "geneff.h"                     /* Frequency effects suppurt    */
#include "libsup.h"                     /* Frequency effects lib supp   */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation          */
#include <stdio.h>                      /* Standard I/O                 */
#include <math.h>                       /* Math library defs            */
#include <io.h>                         /* Low level file I/O           */
  
/************************************************************************/
/*                  Forward & External References                       */
/************************************************************************/
extern  TBXGLO  TBxGlo;                 /* ToolBox Application Globals  */

DWORD   SrcFilPrm (short, FRQBLK FAR *, MMCKINFO FAR *, VISMEMHDL FAR *, ITCBLK FAR * FAR *);
DWORD   DstFilPrm (short, FRQBLK FAR *, MMCKINFO FAR *, VISMEMHDL FAR *, ITCBLK FAR * FAR *);
DWORD   GloFilPrm (FRQBLK FAR *);
long    CfgLngGetDeb (VISMEMHDL, char *, char *, long);
float   CfgFltGetDeb (VISMEMHDL, LPSTR, LPSTR, float);

/************************************************************************/
/************************************************************************/
BOOL FAR PASCAL EffChkFil (LPCSTR szFilLst, FRQBLK FAR *lpFrqBlk)
{
    return (TRUE);
}

BOOL FAR PASCAL EffEndFil (DWORD ulRepCnt, FRQBLK FAR *lpFrqBlk)
{
    return (FALSE);
}

DWORD FAR PASCAL EffPCMFil (short sSrcHdl, short sDstHdl, LPCSTR szSrcFil, 
                 LPCSTR szDstFil, LPSTR lpWrkBuf, WORD usBufSiz, FRQBLK FAR *lpFrqBlk)
{

    VISMEMHDL   hibSrcITC;
    VISMEMHDL   hibDstITC;
    ITCBLK FAR *pibSrcITC;
    ITCBLK FAR *pibDstITC;
    MMCKINFO    ciSrcChk;
    MMCKINFO    ciDstChk;
    DWORD       ulBytReq;               /* Bytes requested total        */
    DWORD       ulXfrCnt = 0L;

    /********************************************************************/
    /* Set file input & output conversion parameters                    */
    /********************************************************************/
    if (!(ulBytReq = SrcFilPrm (sSrcHdl, lpFrqBlk, &ciSrcChk, &hibSrcITC, &pibSrcITC)))
        return (0L);
//Future: break RepBlk into modular components, pass file params to function
lpFrqBlk->usDstFmt = lpFrqBlk->usSrcFmt;
lpFrqBlk->usDstPCM = lpFrqBlk->usSrcPCM;
lpFrqBlk->usDstChn = lpFrqBlk->usSrcChn;
lpFrqBlk->usDstBIO = lpFrqBlk->usSrcBIO;
lpFrqBlk->ulDstFrq = lpFrqBlk->ulSrcFrq;
    if (DstFilPrm (sDstHdl, lpFrqBlk, &ciDstChk, &hibDstITC, &pibDstITC)) 
        return (0L);
    if (GloFilPrm (lpFrqBlk)) return (0L);

    /********************************************************************/
    /********************************************************************/
    ulXfrCnt = FrqFilEff (sSrcHdl, sDstHdl, ulBytReq, lpWrkBuf, usBufSiz, 
        pibSrcITC, pibDstITC, lpFrqBlk);

    /********************************************************************/
    /* Release any PCM conversion memory                                */
    /********************************************************************/
    PCMRelITC (lpFrqBlk->usSrcPCM, pibSrcITC);
    PCMRelITC (lpFrqBlk->usDstPCM, pibDstITC);

    /********************************************************************/
    /* Release any header memory                                        */
    /********************************************************************/
    if (WAVHDRFMT == lpFrqBlk->usSrcFmt) {
        hibSrcITC = GloUnLRel (hibSrcITC);
    }
    if (WAVHDRFMT == lpFrqBlk->usDstFmt) {
        WIOOutEnd (sDstHdl, &hibDstITC, &ciDstChk);
    }

    /********************************************************************/
    /********************************************************************/
    lpFrqBlk->ulTotCnt++;
    return (ulXfrCnt);

}

/************************************************************************/
/************************************************************************/
DWORD   GloFilPrm (FRQBLK FAR *lpFrqBlk)
{
    /********************************************************************/
    /********************************************************************/
//    lpFrqBlk->usFFTOrd = (WORD) CfgLngGetDeb (lpFrqBlk->mhGloCfg, SI_VOCSECNAM, SI_VOCFFTORD, FFTORDDEF);
//    lpFrqBlk->usWinOrd = (WORD) CfgLngGetDeb (lpFrqBlk->mhGloCfg, SI_VOCSECNAM, SI_VOCWINORD, WINORDDEF);
//    lpFrqBlk->flSynThr =        CfgFltGetDeb (lpFrqBlk->mhGloCfg, SI_VOCSECNAM, SI_VOCSYNTHR, SYNTHRDEF);

    /********************************************************************/
    /********************************************************************/
    return (0L);
  
}

/************************************************************************/
/************************************************************************/
DWORD   SrcFilPrm (short sSrcHdl, FRQBLK FAR *lpFrqBlk, MMCKINFO FAR *pciSrcChk, 
        VISMEMHDL FAR *phibSrcITC, ITCBLK FAR * FAR *ppibSrcITC)
{

    DWORD           ulBytReq;           /* Bytes requested total        */
    MMCKINFO        ciSrcChk;
    static  ITCBLK  ibSrcITC;

    /********************************************************************/
    /* Re-position file pointer (if requested)                          */
    /********************************************************************/
    if (-1L == FilSetPos (sSrcHdl, lpFrqBlk->ulSrcOff, SEEK_SET)) return (0L);
    
    /********************************************************************/
    /* Set input conversion parameters                                  */
    /********************************************************************/
    switch (lpFrqBlk->usSrcFmt) {
        /****************************************************************/
        /****************************************************************/
        case ALLPURFMT:
            if (-1L == (ulBytReq = FilGetLen (sSrcHdl))) return (0L);
            *phibSrcITC = (VISMEMHDL) NULL;
            PCMAloITC (lpFrqBlk->usSrcPCM, (*ppibSrcITC = &ibSrcITC), NULL);
            break;

        /****************************************************************/
        /****************************************************************/
        case VISHDRFMT:
            if (VIXFndFst (sSrcHdl, "WAVE", 0L, &pciSrcChk->dwDataOffset, FIOENCNON)) {
                if (TBxGlo.usDebFlg & ERR___DEB) 
                    MsgDspUsr ("has an unknown file format, skipping...\n");
                return (0L);
            }
            VIXDesCur (sSrcHdl, &pciSrcChk->cksize, FIOENCNON);
            if (VIXLodTyp (sSrcHdl, &lpFrqBlk->usSrcPCM, &lpFrqBlk->usSrcChn, 
              &lpFrqBlk->ulSrcFrq, FIOENCNON)) {
                if (TBxGlo.usDebFlg & ERR___DEB) 
                    MsgDspUsr ("has an unknown file format, skipping...\n");
                return (0L);
            }
            VIXFndNxt (sSrcHdl, 0L, FIOENCNON);
            VIXFndNxt (sSrcHdl, 0L, FIOENCNON);

            *phibSrcITC = (VISMEMHDL) NULL;
            PCMAloITC (lpFrqBlk->usSrcPCM, (*ppibSrcITC = &ibSrcITC), NULL);
            break;

        /****************************************************************/
        /****************************************************************/
        case WAVHDRFMT:
            if (WIOFndNxt (sSrcHdl, &lpFrqBlk->usSrcPCM, phibSrcITC, &ciSrcChk)) {
                if (TBxGlo.usDebFlg & ERR___DEB) 
                    MsgDspUsr ("is not a Wave Multimedia Format File, skipping...\n");
                return (0L);
            }
            if (UNKPCM000 == lpFrqBlk->usSrcPCM) {
                if (TBxGlo.usDebFlg & ERR___DEB) 
                    MsgDspUsr ("has unknown Wave Multimedia PCM type, skipping...\n");
                return (0L);
            }
            if (NULL == (*ppibSrcITC = GloMemLck (*phibSrcITC))) {
                if (TBxGlo.usDebFlg & ERR___DEB) 
                    MsgDspUsr ("encountered a memory locking error, skipping...\n");
                return (0L);
            }
            ulBytReq = ciSrcChk.cksize;
            lpFrqBlk->usSrcChn = (*ppibSrcITC)->mwMSW.afWEX.wfx.nChannels;
            lpFrqBlk->ulSrcFrq = (*ppibSrcITC)->mwMSW.afWEX.wfx.nSamplesPerSec;
            break;

        /****************************************************************/
        /****************************************************************/
        default:
            if (TBxGlo.usDebFlg & ERR___DEB) 
                MsgDspUsr ("has an unknown file format, skipping...\n");
            return (0L);
    }

    /********************************************************************/
    /* Set demo limits                                                  */
    /********************************************************************/
    if (REGDEMKEY == TBxGlo.usRegKey) {
        ulBytReq = min (ulBytReq, PCMSmp2Bh (lpFrqBlk->usSrcPCM, lpFrqBlk->usSrcChn,
            DEMFILDEF * lpFrqBlk->ulSrcFrq, *ppibSrcITC));
        if (TBxGlo.usDebFlg & INI___DEB) 
            MsgDspUsr ("\nBytReq: %ld ", ulBytReq);
    }

    /********************************************************************/
    /* Get configuration file parameters                                */
    /********************************************************************/
    if ((PTCPCM001 == lpFrqBlk->usSrcPCM) || (NWVPCM001 == lpFrqBlk->usSrcPCM)) {
        ITCINI      iiIni;
        PCMAloITC (lpFrqBlk->usSrcPCM, NULL, &iiIni);       /* Load def */
        if (lpFrqBlk->mhSrcCfg) {
            iiIni.ciCVS.usLowTyp = (WORD)  CfgLngGetDeb (lpFrqBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSLOWTYP, iiIni.ciCVS.usLowTyp);
            iiIni.ciCVS.ulLowPas = (DWORD) CfgLngGetDeb (lpFrqBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSLOWPAS, iiIni.ciCVS.ulLowPas);
            iiIni.ciCVS.usSigFtr = (WORD)  CfgLngGetDeb (lpFrqBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSSIGFTR, iiIni.ciCVS.usSigFtr);
            iiIni.ciCVS.usSylAtk = (WORD)  CfgLngGetDeb (lpFrqBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSSYLATK, iiIni.ciCVS.usSylAtk);
            iiIni.ciCVS.usSylDcy = (WORD)  CfgLngGetDeb (lpFrqBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSSYLDCY, iiIni.ciCVS.usSylDcy);
            iiIni.ciCVS.usCoiBit = (WORD)  CfgLngGetDeb (lpFrqBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSCOIBIT, iiIni.ciCVS.usCoiBit);
            iiIni.ciCVS.usStpMin = (WORD)  CfgLngGetDeb (lpFrqBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSSTPMIN, iiIni.ciCVS.usStpMin);
            iiIni.ciCVS.usStpMax = (WORD)  CfgLngGetDeb (lpFrqBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSSTPMAX, iiIni.ciCVS.usStpMax);
        }
        iiIni.ciCVS.ulSmpFrq = lpFrqBlk->ulSrcFrq;
        PCMAloITC (lpFrqBlk->usSrcPCM, *ppibSrcITC, &iiIni);  
    }

    /********************************************************************/
    /********************************************************************/
    return (min (ulBytReq, lpFrqBlk->ulSrcLen));

}

DWORD   DstFilPrm (short sDstHdl, FRQBLK FAR *lpFrqBlk, MMCKINFO FAR *pciDstChk,
        VISMEMHDL FAR *phibDstITC, ITCBLK FAR * FAR *ppibDstITC)
{
    static  ITCBLK  ibDstITC;

    /********************************************************************/
    /********************************************************************/
    switch (lpFrqBlk->usDstFmt) {
        case ALLPURFMT:
            *phibDstITC = (VISMEMHDL) NULL;
            PCMAloITC (lpFrqBlk->usDstPCM, (*ppibDstITC = &ibDstITC), NULL);
            break;

        /****************************************************************/
        /****************************************************************/
        case VISHDRFMT:
            if (VIXWrtFst (sDstHdl, "WAVE", 0L, &pciDstChk->dwDataOffset, FIOENCNON) ||
              VIXWrtTyp (sDstHdl, lpFrqBlk->usDstPCM, lpFrqBlk->usDstChn, 
              lpFrqBlk->ulDstFrq, FIOENCNON) || 
              VIXWrtDat (sDstHdl, &pciDstChk->cksize, FIOENCNON)) {
                if (TBxGlo.usDebFlg & ERR___DEB) 
                    MsgDspUsr ("encountered a file output error, skipping...\n");
                return (0L);
            }
            *phibDstITC = (VISMEMHDL) NULL;
            PCMAloITC (lpFrqBlk->usDstPCM, (*ppibDstITC = &ibDstITC), NULL);
            break;

        /****************************************************************/
        /****************************************************************/
        case WAVHDRFMT:
            if (WIOOutIni (sDstHdl, lpFrqBlk->usDstPCM, lpFrqBlk->usDstChn,
              lpFrqBlk->ulDstFrq, phibDstITC, pciDstChk)) {
                if (TBxGlo.usDebFlg & ERR___DEB) 
                    MsgDspUsr ("encountered a file output error, skipping...\n");
                return ((DWORD) -1L);
            }
            if (NULL == (*ppibDstITC = GloMemLck (*phibDstITC))) {
                if (TBxGlo.usDebFlg & ERR___DEB) 
                    MsgDspUsr ("encountered a memory locking error, skipping...\n");
                return ((DWORD) -1L);
            }
            break;

        /****************************************************************/
        /****************************************************************/
        default:
            if (TBxGlo.usDebFlg & ERR___DEB) 
                MsgDspUsr ("has an unknown file format, skipping...\n");
            return ((DWORD) -1L);
    }

    /********************************************************************/
    /* Get configuration file parameters                                */
    /********************************************************************/
    if ((PTCPCM001 == lpFrqBlk->usDstPCM) || (NWVPCM001 == lpFrqBlk->usDstPCM)) {
        ITCINI      iiIni;
        PCMAloITC (lpFrqBlk->usDstPCM, NULL, &iiIni);       /* Load def */
        if (lpFrqBlk->mhDstCfg) {
            iiIni.ciCVS.usSigFtr = (WORD)  CfgLngGetDeb (lpFrqBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSSIGFTR, iiIni.ciCVS.usSigFtr);
            iiIni.ciCVS.usSylAtk = (WORD)  CfgLngGetDeb (lpFrqBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSSYLATK, iiIni.ciCVS.usSylAtk);
            iiIni.ciCVS.usSylDcy = (WORD)  CfgLngGetDeb (lpFrqBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSSYLDCY, iiIni.ciCVS.usSylDcy);
            iiIni.ciCVS.usCoiBit = (WORD)  CfgLngGetDeb (lpFrqBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSCOIBIT, iiIni.ciCVS.usCoiBit);
            iiIni.ciCVS.usStpMin = (WORD)  CfgLngGetDeb (lpFrqBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSSTPMIN, iiIni.ciCVS.usStpMin);
            iiIni.ciCVS.usStpMax = (WORD)  CfgLngGetDeb (lpFrqBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSSTPMAX, iiIni.ciCVS.usStpMax);
        }
        iiIni.ciCVS.ulSmpFrq = lpFrqBlk->ulDstFrq;
        PCMAloITC (lpFrqBlk->usDstPCM, *ppibDstITC, &iiIni);  
    }

    /********************************************************************/
    /********************************************************************/
    return (0L);
  
}

long    CfgLngGetDeb (VISMEMHDL mhCfgMem, LPSTR szSecNam, LPSTR szPrmNam, 
        long lDefVal)
{
    /********************************************************************/
    /* Display configuration if in debug mode                           */
    /********************************************************************/
    lDefVal = CfgLngGet (mhCfgMem, szSecNam, szPrmNam, lDefVal);
    if (TBxGlo.usDebFlg & CFG___DEB) 
        MsgDspUsr ("\n%Fs %Fs %ld", szSecNam, szPrmNam, lDefVal);

    /********************************************************************/
    /********************************************************************/
    return (lDefVal);
}

float   CfgFltGetDeb (VISMEMHDL mhCfgMem, LPSTR szSecNam, LPSTR szPrmNam, 
        float flDefVal)
{
    /********************************************************************/
    /* Display configuration if in debug mode                           */
    /********************************************************************/
    flDefVal = CfgFltGet (mhCfgMem, szSecNam, szPrmNam, flDefVal);
    if (TBxGlo.usDebFlg & CFG___DEB) 
        MsgDspUsr ("\n%Fs %Fs %.2f", szSecNam, szPrmNam, flDefVal);

    /********************************************************************/
    /********************************************************************/
    return (flDefVal);
}

