/************************************************************************/
/* Speed Adjust File Loader: FilLod.c                   V2.00  08/15/95 */
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
#include "genspd.h"                     /* Speed Adjust support         */
#include "libsup.h"                     /* Speed Adjust lib supp        */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation          */
#include <stdio.h>                      /* Standard I/O                 */
#include <math.h>                       /* Math library defs            */
#include <io.h>                         /* Low level file I/O           */
  
/************************************************************************/
/*                  Forward & External References                       */
/************************************************************************/
extern  TBXGLO  TBxGlo;                 /* ToolBox Application Globals  */

/************************************************************************/
/*                          Forward References                          */
/************************************************************************/
DWORD   SrcFilPrm (short, SPDBLK FAR *, MMCKINFO FAR *, VISMEMHDL FAR *, ITCBLK FAR * FAR *);
DWORD   DstFilPrm (short, SPDBLK FAR *, MMCKINFO FAR *, VISMEMHDL FAR *, ITCBLK FAR * FAR *);
DWORD   GloFilPrm (SPDBLK FAR *);
long    CfgLngGetDeb (VISMEMHDL, LPSTR, LPSTR, long);
float   CfgFltGetDeb (VISMEMHDL, LPSTR, LPSTR, float);

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL SpdAdjFil (short sSrcHdl, short sDstHdl, LPCSTR szSrcFil, 
                 LPCSTR szDstFil, LPSTR lpWrkBuf, WORD usBufSiz, SPDBLK FAR *lpSpdBlk)
{

    VISMEMHDL   hibSrcITC;
    VISMEMHDL   hibDstITC;
    ITCBLK FAR *pibSrcITC;
    ITCBLK FAR *pibDstITC;
    MMCKINFO    ciSrcChk;
    MMCKINFO    ciDstChk;
    DWORD       ulBytReq;               /* Bytes requested total        */
    DWORD       ulXfrCnt;

    /********************************************************************/
    /* Set file input & output conversion parameters                    */
    /********************************************************************/
    if (!(ulBytReq = SrcFilPrm (sSrcHdl, lpSpdBlk, &ciSrcChk, &hibSrcITC, &pibSrcITC)))
        return (0L);
    if (DstFilPrm (sDstHdl, lpSpdBlk, &ciDstChk, &hibDstITC, &pibDstITC)) 
        return (0L);
    if (GloFilPrm (lpSpdBlk)) return (0L);

    /********************************************************************/
    /********************************************************************/
    ulXfrCnt = SpdFilEff (sSrcHdl, sDstHdl, ulBytReq, lpWrkBuf, usBufSiz, 
        pibSrcITC, pibDstITC, lpSpdBlk);

    /********************************************************************/
    /* Release any PCM conversion memory                                */
    /********************************************************************/
    PCMRelITC (lpSpdBlk->usSrcPCM, pibSrcITC);
    PCMRelITC (lpSpdBlk->usDstPCM, pibDstITC);

    /********************************************************************/
    /* Release any header memory                                        */
    /********************************************************************/
    if (WAVHDRFMT == lpSpdBlk->usSrcFmt) {
        hibSrcITC = GloUnLRel (hibSrcITC);
    }
    if (WAVHDRFMT == lpSpdBlk->usDstFmt) {
        WIOOutEnd (sDstHdl, &hibDstITC, &ciDstChk);
    }

    /********************************************************************/
    /********************************************************************/
    return (ulXfrCnt);

}

/************************************************************************/
/************************************************************************/
DWORD   GloFilPrm (SPDBLK FAR *lpSpdBlk)
{
    /********************************************************************/
    /********************************************************************/
//    lpSpdBlk->usFFTOrd = (WORD) CfgLngGetDeb (lpSpdBlk->mhGloCfg, SI_VOCSECNAM, SI_VOCFFTORD, FFTORDDEF);
//    lpSpdBlk->usWinOrd = (WORD) CfgLngGetDeb (lpSpdBlk->mhGloCfg, SI_VOCSECNAM, SI_VOCWINORD, WINORDDEF);
//    lpSpdBlk->flSynThr =        CfgFltGetDeb (lpSpdBlk->mhGloCfg, SI_VOCSECNAM, SI_VOCSYNTHR, SYNTHRDEF);

    /********************************************************************/
    /********************************************************************/
    return (0L);
  
}

/************************************************************************/
/************************************************************************/
DWORD   SrcFilPrm (short sSrcHdl, SPDBLK FAR *lpSpdBlk, MMCKINFO FAR *pciSrcChk, 
        VISMEMHDL FAR *phibSrcITC, ITCBLK FAR * FAR *ppibSrcITC)
{

    DWORD           ulBytReq;           /* Bytes requested total        */
    MMCKINFO        ciSrcChk;
    static  ITCBLK  ibSrcITC;

    /********************************************************************/
    /* Re-position file pointer (if requested)                          */
    /********************************************************************/
    if (-1L == FilSetPos (sSrcHdl, lpSpdBlk->ulSrcOff, SEEK_SET)) return (0L);
    
    /********************************************************************/
    /* Set input conversion parameters                                  */
    /********************************************************************/
    switch (lpSpdBlk->usSrcFmt) {
        /****************************************************************/
        /****************************************************************/
        case ALLPURFMT:
            if (-1L == (ulBytReq = FilGetLen (sSrcHdl))) return (0L);
            *phibSrcITC = (VISMEMHDL) NULL;
            PCMAloITC (lpSpdBlk->usSrcPCM, (*ppibSrcITC = &ibSrcITC), NULL);
            break;

        /****************************************************************/
        /****************************************************************/
        case VISHDRFMT:
            if (VIXFndFst (sSrcHdl, "WAVE", 0L, &pciSrcChk->dwDataOffset, FIOENCNON)) {
                SPDERRMSG ("has an unknown file format, skipping...\n");
                return (0L);
            }
            VIXDesCur (sSrcHdl, &pciSrcChk->cksize, FIOENCNON);
            if (VIXLodTyp (sSrcHdl, &lpSpdBlk->usSrcPCM, &lpSpdBlk->usSrcChn, 
              &lpSpdBlk->ulSrcFrq, FIOENCNON)) {
                SPDERRMSG ("has an unknown file format, skipping...\n");
                return (0L);
            }
            VIXFndNxt (sSrcHdl, 0L, FIOENCNON);
            VIXFndNxt (sSrcHdl, 0L, FIOENCNON);

            *phibSrcITC = (VISMEMHDL) NULL;
            PCMAloITC (lpSpdBlk->usSrcPCM, (*ppibSrcITC = &ibSrcITC), NULL);
            break;

        /****************************************************************/
        /****************************************************************/
        case WAVHDRFMT:
            if (WIOFndNxt (sSrcHdl, &lpSpdBlk->usSrcPCM, phibSrcITC, &ciSrcChk)) {
                SPDERRMSG ("is not a Wave Multimedia Format File, skipping...\n");
                return (0L);
            }
            if (UNKPCM000 == lpSpdBlk->usSrcPCM) {
                SPDERRMSG ("has unknown Wave Multimedia PCM type, skipping...\n");
                return (0L);
            }
            if (NULL == (*ppibSrcITC = GloMemLck (*phibSrcITC))) {
                SPDERRMSG ("encountered a memory locking error, skipping...\n");
                return (0L);
            }
            ulBytReq = ciSrcChk.cksize;
            lpSpdBlk->usSrcChn = (*ppibSrcITC)->mwMSW.afWEX.wfx.nChannels;
            lpSpdBlk->ulSrcFrq = (*ppibSrcITC)->mwMSW.afWEX.wfx.nSamplesPerSec;
            break;

        /****************************************************************/
        /****************************************************************/
        default:
            SPDERRMSG ("has an unknown file format, skipping...\n");
            return (0L);
    }

    /********************************************************************/
    /* Set demo limits                                                  */
    /********************************************************************/
    if (REGDEMKEY == TBxGlo.usRegKey) {
        ulBytReq = min (ulBytReq, PCMSmp2Bh (lpSpdBlk->usSrcPCM, lpSpdBlk->usSrcChn,
            DEMFILDEF * lpSpdBlk->ulSrcFrq, *ppibSrcITC));
        if (TBxGlo.usDebFlg & INI___DEB) 
            MsgDspUsr ("\nBytReq: %ld ", ulBytReq);
    }

    /********************************************************************/
    /* Get configuration file parameters                                */
    /********************************************************************/
    if ((HARPCM001 == lpSpdBlk->usSrcPCM) || 
        (PTCPCM001 == lpSpdBlk->usSrcPCM) ||
        (NWVPCM001 == lpSpdBlk->usSrcPCM)) {
        ITCINI      iiIni;
        PCMAloITC (lpSpdBlk->usSrcPCM, NULL, &iiIni);       /* Load def */

        /****************************************************************/
        /* Read configuration file                                      */
        /****************************************************************/
        if (lpSpdBlk->mhSrcCfg) {
            iiIni.ciCVS.usLowTyp = (WORD)  CfgLngGetDeb (lpSpdBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSLOWTYP, iiIni.ciCVS.usLowTyp);
            iiIni.ciCVS.ulLowPas = (DWORD) CfgLngGetDeb (lpSpdBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSLOWPAS, iiIni.ciCVS.ulLowPas);
            iiIni.ciCVS.usSigFtr = (WORD)  CfgLngGetDeb (lpSpdBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSSIGFTR, iiIni.ciCVS.usSigFtr);
            iiIni.ciCVS.usSylAtk = (WORD)  CfgLngGetDeb (lpSpdBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSSYLATK, iiIni.ciCVS.usSylAtk);
            iiIni.ciCVS.usSylDcy = (WORD)  CfgLngGetDeb (lpSpdBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSSYLDCY, iiIni.ciCVS.usSylDcy);
            iiIni.ciCVS.usCoiBit = (WORD)  CfgLngGetDeb (lpSpdBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSCOIBIT, iiIni.ciCVS.usCoiBit);
            iiIni.ciCVS.usStpMin = (WORD)  CfgLngGetDeb (lpSpdBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSSTPMIN, iiIni.ciCVS.usStpMin);
            iiIni.ciCVS.usStpMax = (WORD)  CfgLngGetDeb (lpSpdBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSSTPMAX, iiIni.ciCVS.usStpMax);
        }
        iiIni.ciCVS.ulSmpFrq = lpSpdBlk->ulSrcFrq;
        PCMAloITC (lpSpdBlk->usSrcPCM, *ppibSrcITC, &iiIni);  
    }

    /********************************************************************/
    /********************************************************************/
    return (min (ulBytReq, lpSpdBlk->ulSrcLen));

}

DWORD   DstFilPrm (short sDstHdl, SPDBLK FAR *lpSpdBlk, MMCKINFO FAR *pciDstChk,
        VISMEMHDL FAR *phibDstITC, ITCBLK FAR * FAR *ppibDstITC)
{
    static  ITCBLK  ibDstITC;

    /********************************************************************/
    /********************************************************************/
    switch (lpSpdBlk->usDstFmt) {
        case ALLPURFMT:
            *phibDstITC = (VISMEMHDL) NULL;
            PCMAloITC (lpSpdBlk->usDstPCM, (*ppibDstITC = &ibDstITC), NULL);
            break;

        /****************************************************************/
        /****************************************************************/
        case VISHDRFMT:
            if (VIXWrtFst (sDstHdl, "WAVE", 0L, &pciDstChk->dwDataOffset, FIOENCNON) ||
              VIXWrtTyp (sDstHdl, lpSpdBlk->usDstPCM, lpSpdBlk->usDstChn, 
              lpSpdBlk->ulDstFrq, FIOENCNON) || 
              VIXWrtDat (sDstHdl, &pciDstChk->cksize, FIOENCNON)) {
                SPDERRMSG ("encountered a file output error, skipping...\n");
                return (0L);
            }
            *phibDstITC = (VISMEMHDL) NULL;
            PCMAloITC (lpSpdBlk->usDstPCM, (*ppibDstITC = &ibDstITC), NULL);
            break;

        /****************************************************************/
        /****************************************************************/
        case WAVHDRFMT:
            if (WIOOutIni (sDstHdl, lpSpdBlk->usDstPCM, lpSpdBlk->usDstChn,
              lpSpdBlk->ulDstFrq, phibDstITC, pciDstChk)) {
                SPDERRMSG ("encountered a file output error, skipping...\n");
                return ((DWORD) -1L);
            }
            if (NULL == (*ppibDstITC = GloMemLck (*phibDstITC))) {
                SPDERRMSG ("encountered a memory locking error, skipping...\n");
                return ((DWORD) -1L);
            }
            break;

        /****************************************************************/
        /****************************************************************/
        default:
            SPDERRMSG ("has an unknown file format, skipping...\n");
            return ((DWORD) -1L);
    }

    /********************************************************************/
    /* Get configuration file parameters                                */
    /* Note: Low pass filter unused on output                           */
    /********************************************************************/
    if ((HARPCM001 == lpSpdBlk->usDstPCM) || 
        (PTCPCM001 == lpSpdBlk->usDstPCM) ||
        (NWVPCM001 == lpSpdBlk->usDstPCM)) {
        ITCINI      iiIni;
        PCMAloITC (lpSpdBlk->usDstPCM, NULL, &iiIni);       /* Load def */
        /****************************************************************/
        /* Read configuration file                                      */
        /****************************************************************/
        if (lpSpdBlk->mhDstCfg) {
            iiIni.ciCVS.usSigFtr = (WORD)  CfgLngGetDeb (lpSpdBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSSIGFTR, iiIni.ciCVS.usSigFtr);
            iiIni.ciCVS.usSylAtk = (WORD)  CfgLngGetDeb (lpSpdBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSSYLATK, iiIni.ciCVS.usSylAtk);
            iiIni.ciCVS.usSylDcy = (WORD)  CfgLngGetDeb (lpSpdBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSSYLDCY, iiIni.ciCVS.usSylDcy);
            iiIni.ciCVS.usCoiBit = (WORD)  CfgLngGetDeb (lpSpdBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSCOIBIT, iiIni.ciCVS.usCoiBit);
            iiIni.ciCVS.usStpMin = (WORD)  CfgLngGetDeb (lpSpdBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSSTPMIN, iiIni.ciCVS.usStpMin);
            iiIni.ciCVS.usStpMax = (WORD)  CfgLngGetDeb (lpSpdBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSSTPMAX, iiIni.ciCVS.usStpMax);
        }
        iiIni.ciCVS.ulSmpFrq = lpSpdBlk->ulDstFrq;
        PCMAloITC (lpSpdBlk->usDstPCM, *ppibDstITC, &iiIni);  
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