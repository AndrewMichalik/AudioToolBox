/************************************************************************/
/* PCM Conversion File Loader: FilLod.c                 V2.00  08/10/93 */
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
#include "gencvt.h"                     /* PCM conversion defs          */
#include "libsup.h"                     /* PCM conversion lib supp      */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation          */
#include <stdio.h>                      /* Standard I/O                 */
#include <math.h>                       /* Math library defs            */
#include <io.h>                         /* Low level file I/O           */
  
/************************************************************************/
/*                          External References                         */
/************************************************************************/
extern  TBXGLO  TBxGlo;                 /* ToolBox Application Globals  */

/************************************************************************/
/*                          Forward References                          */
/************************************************************************/
DWORD   SrcFilPrm (short, CVTBLK FAR *, MMCKINFO FAR *, VISMEMHDL FAR *, ITCBLK FAR * FAR *);
DWORD   DstFilPrm (short, CVTBLK FAR *, MMCKINFO FAR *, VISMEMHDL FAR *, ITCBLK FAR * FAR *);
DWORD   GloFilPrm (CVTBLK FAR *);
long    CfgLngGetDeb (VISMEMHDL, LPSTR, LPSTR, long);
float   CfgFltGetDeb (VISMEMHDL, LPSTR, LPSTR, float);
LPSTR   CfgStrGetDeb (VISMEMHDL, LPSTR, LPSTR, LPSTR, LPSTR, WORD);
LPSTR   _fitoa  (int, LPSTR, int);

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL CvtPCMFil (short sSrcHdl, short sDstHdl, LPCSTR szSrcRsv, 
                 LPCSTR szDstRsv, LPSTR lpWrkRsv, WORD usSizRsv, CVTBLK FAR *lpCvtBlk)
{

    VISMEMHDL   hibSrcITC;
    VISMEMHDL   hibDstITC;
    ITCBLK FAR *pibSrcITC;
    ITCBLK FAR *pibDstITC;
    MMCKINFO    ciSrcChk;
    MMCKINFO    ciDstChk;
    DWORD       ulBytReq;               /* Bytes requested total        */
    DWORD       ulXfrCnt;

//ajm -doWAV000 support
//CVTBLK  cbCvtBlk = *lpCvtBlk;

    /********************************************************************/
    /* Set file input & output conversion parameters                    */
    /********************************************************************/
    if (!(ulBytReq = SrcFilPrm (sSrcHdl, lpCvtBlk, &ciSrcChk, &hibSrcITC, &pibSrcITC)))
        return (0L);

//ajm -doWAV000 support: need to set lpCvtBlk in CvtFileff also
//if (UNKPCM000 == lpCvtBlk->usDstPCM) {
//                        cbCvtBlk.usDstPCM = cbCvtBlk.usSrcPCM;
//if (!cbCvtBlk.usDstChn) cbCvtBlk.usDstChn = cbCvtBlk.usSrcChn;
//if (!cbCvtBlk.ulDstFrq) cbCvtBlk.ulDstFrq = cbCvtBlk.ulSrcFrq;
//lpCvtBlk = &cbCvtBlk;
//}

    if (DstFilPrm (sDstHdl, lpCvtBlk, &ciDstChk, &hibDstITC, &pibDstITC)) 
        return (0L);
    if (GloFilPrm (lpCvtBlk)) return (0L);

    /********************************************************************/
    /********************************************************************/
    ulXfrCnt = CvtFilEff (sSrcHdl, sDstHdl, ulBytReq, NULL, 0, 
        pibSrcITC, pibDstITC, lpCvtBlk);

    /********************************************************************/
    /* Release any PCM conversion memory                                */
    /********************************************************************/
    PCMRelITC (lpCvtBlk->usSrcPCM, pibSrcITC);
    PCMRelITC (lpCvtBlk->usDstPCM, pibDstITC);

    /********************************************************************/
    /* Release any header memory                                        */
    /********************************************************************/
    if (WAVHDRFMT == lpCvtBlk->usSrcFmt) {
        hibSrcITC = GloUnLRel (hibSrcITC);
    }
    if (WAVHDRFMT == lpCvtBlk->usDstFmt) {
        WIOOutEnd (sDstHdl, &hibDstITC, &ciDstChk);
    }

    /********************************************************************/
    /********************************************************************/
    return (ulXfrCnt);

}

/************************************************************************/
/************************************************************************/
DWORD   GloFilPrm (CVTBLK FAR *lpCvtBlk)
{
    #define FLTEOFFLG   ((float) -999999)
    char    szBndKey[CFGMAXSTR];
    char    szIdxKey[CFGMAXSTR + TBXMAXSTR];
    WORD    usi;

    /********************************************************************/
    /********************************************************************/
    lpCvtBlk->usFFTOrd = (WORD) CfgLngGetDeb (lpCvtBlk->mhGloCfg, SI_GLOSECNAM, SI_GLOFFTORD, FFTORDDEF);

    /********************************************************************/
    /********************************************************************/
    if (lpCvtBlk->usNrmLvl) {
        lpCvtBlk->flNrmExc =        CfgFltGetDeb (lpCvtBlk->mhGloCfg, SI_VOLSECNAM, SI_VOLNRMEXC, lpCvtBlk->flNrmExc);
        lpCvtBlk->usNrmLvl = (WORD) CfgLngGetDeb (lpCvtBlk->mhGloCfg, SI_VOLSECNAM, SI_VOLNRMLVL, NRMLVLDEF);
        lpCvtBlk->usNrmMax = (WORD) CfgLngGetDeb (lpCvtBlk->mhGloCfg, SI_VOLSECNAM, SI_VOLNRMMAX, NRMMAXDEF);
    }

    /********************************************************************/
    /********************************************************************/
    if (lpCvtBlk->flDCELvl) {
        lpCvtBlk->usCmpThr = (WORD) CfgLngGetDeb (lpCvtBlk->mhGloCfg, SI_DCESECNAM, SI_DCECMPTHR, CMPTHRDEF);
        lpCvtBlk->usCmpAtk = (WORD) CfgLngGetDeb (lpCvtBlk->mhGloCfg, SI_DCESECNAM, SI_DCECMPATK, CMPATKDEF);
        lpCvtBlk->flCmpMin =        CfgFltGetDeb (lpCvtBlk->mhGloCfg, SI_DCESECNAM, SI_DCECMPMIN, lpCvtBlk->flDCELvl);
        lpCvtBlk->usExpThr = (WORD) CfgLngGetDeb (lpCvtBlk->mhGloCfg, SI_DCESECNAM, SI_DCEEXPTHR, EXPTHRDEF);
        lpCvtBlk->usExpAtk = (WORD) CfgLngGetDeb (lpCvtBlk->mhGloCfg, SI_DCESECNAM, SI_DCEEXPATK, EXPATKDEF);
        lpCvtBlk->flExpMax =        CfgFltGetDeb (lpCvtBlk->mhGloCfg, SI_DCESECNAM, SI_DCEEXPMAX, lpCvtBlk->flDCELvl);
        lpCvtBlk->usCE_Dcy = (WORD) CfgLngGetDeb (lpCvtBlk->mhGloCfg, SI_DCESECNAM, SI_DCECE_DCY, CE_DCYDEF);
        lpCvtBlk->usNoiThr = (WORD) CfgLngGetDeb (lpCvtBlk->mhGloCfg, SI_DCESECNAM, SI_DCENOITHR, NOITHRDEF);
        lpCvtBlk->flNoiAtt =        CfgFltGetDeb (lpCvtBlk->mhGloCfg, SI_DCESECNAM, SI_DCENOIATT, NOIATTDEF);
    }

    /********************************************************************/
    /********************************************************************/
    if (lpCvtBlk->flCrpFrm) {
        lpCvtBlk->flCrpFrm =        CfgFltGetDeb (lpCvtBlk->mhGloCfg, SI_CRPSECNAM, SI_CRPEVTFRM, CRPFRMDEF);
        lpCvtBlk->flCrpRes =        CfgFltGetDeb (lpCvtBlk->mhGloCfg, SI_CRPSECNAM, SI_CRPEVTRES, CRPRESDEF);
        lpCvtBlk->flCrpSnd =        CfgFltGetDeb (lpCvtBlk->mhGloCfg, SI_CRPSECNAM, SI_CRPEVTSND, CRPSNDDEF);
        lpCvtBlk->usCrpAtk = (WORD) CfgLngGetDeb (lpCvtBlk->mhGloCfg, SI_CRPSECNAM, SI_CRPEVTATK, CRPATKDEF);
        lpCvtBlk->usCrpDcy = (WORD) CfgLngGetDeb (lpCvtBlk->mhGloCfg, SI_CRPSECNAM, SI_CRPEVTDCY, CRPDCYDEF);
        lpCvtBlk->flCrpGrd =        CfgFltGetDeb (lpCvtBlk->mhGloCfg, SI_CRPSECNAM, SI_CRPEVTGRD, lpCvtBlk->flCrpGrd);
    }

    /********************************************************************/
    /********************************************************************/
    if (lpCvtBlk->bfFFTFEq) {
        /****************************************************************/
        /****************************************************************/
        lpCvtBlk->flFEqGai = CfgFltGetDeb (lpCvtBlk->mhGloCfg, SI_FEQSECNAM, SI_FEQGAIADJ, lpCvtBlk->flFEqGai);
        lpCvtBlk->ulFEqBot = CfgLngGetDeb (lpCvtBlk->mhGloCfg, SI_FEQSECNAM, SI_FEQBOTFRQ, lpCvtBlk->ulFEqBot);
        lpCvtBlk->ulFEqTop = CfgLngGetDeb (lpCvtBlk->mhGloCfg, SI_FEQSECNAM, SI_FEQTOPFRQ, lpCvtBlk->ulFEqTop);

        /****************************************************************/
        /* Top frequency of zero indicates Nyquist (ie, max) frequency  */
        /****************************************************************/
        if (!lpCvtBlk->ulFEqTop) lpCvtBlk->ulFEqTop = lpCvtBlk->ulSrcFrq / 2L;

        /****************************************************************/
        /* Read equalizer band data; append "1 to x" to key name.       */ 
        /* Note: Final (EOF) point value is ignored, usi is "count of". */
        /****************************************************************/
        if (NULL != CfgStrGetDeb (lpCvtBlk->mhGloCfg, SI_FEQSECNAM, SI_FEQBNDKEY, NULL, szBndKey, CFGMAXSTR)) {
            for (usi=0; usi < FEQMAXCNT; usi++) {
                float   flFEqPnt;
                _fstrcpy (szIdxKey, szBndKey);
                _fitoa (usi+1, &szIdxKey[_fstrlen(szIdxKey)], 10);
                flFEqPnt = CfgFltGetDeb (lpCvtBlk->mhGloCfg, SI_FEQSECNAM, szIdxKey, FLTEOFFLG);
                if (FLTEOFFLG == flFEqPnt) break;
                lpCvtBlk->flFEqPnt[usi] = flFEqPnt;
            }
            if (usi) lpCvtBlk->usFEqCnt = usi;
        }
    }

    /********************************************************************/
    /********************************************************************/
    return (0L);
  
}

/************************************************************************/
/************************************************************************/
DWORD   SrcFilPrm (short sSrcHdl, CVTBLK FAR *lpCvtBlk, MMCKINFO FAR *pciSrcChk, 
        VISMEMHDL FAR *phibSrcITC, ITCBLK FAR * FAR *ppibSrcITC)
{

    DWORD           ulBytReq;           /* Bytes requested total        */
    MMCKINFO        ciSrcChk;
    static  ITCBLK  ibSrcITC;

    /********************************************************************/
    /* Re-position file pointer (if requested)                          */
    /********************************************************************/
    if (-1L == FilSetPos (sSrcHdl, lpCvtBlk->ulSrcOff, SEEK_SET)) return (0L);
    
    /********************************************************************/
    /* Set input conversion parameters                                  */
    /********************************************************************/
    switch (lpCvtBlk->usSrcFmt) {
        /****************************************************************/
        /****************************************************************/
        case ALLPURFMT:
            if (-1L == (ulBytReq = FilGetLen (sSrcHdl))) return (0L);
            *phibSrcITC = (VISMEMHDL) NULL;
            PCMAloITC (lpCvtBlk->usSrcPCM, (*ppibSrcITC = &ibSrcITC), NULL);
            break;

        /****************************************************************/
        /****************************************************************/
        case VISHDRFMT:
            if (VIXFndFst (sSrcHdl, "WAVE", 0L, &pciSrcChk->dwDataOffset, FIOENCNON)) {
                CVTERRMSG ("has an unknown file format, skipping...\n");
                return (0L);
            }
            VIXDesCur (sSrcHdl, &pciSrcChk->cksize, FIOENCNON);
            if (VIXLodTyp (sSrcHdl, &lpCvtBlk->usSrcPCM, &lpCvtBlk->usSrcChn, 
              &lpCvtBlk->ulSrcFrq, FIOENCNON)) {
                CVTERRMSG ("has an unknown file format, skipping...\n");
                return (0L);
            }
            VIXFndNxt (sSrcHdl, 0L, FIOENCNON);
            VIXFndNxt (sSrcHdl, 0L, FIOENCNON);

            *phibSrcITC = (VISMEMHDL) NULL;
            PCMAloITC (lpCvtBlk->usSrcPCM, (*ppibSrcITC = &ibSrcITC), NULL);
            break;

        /****************************************************************/
        /****************************************************************/
        case WAVHDRFMT:
            if (WIOFndNxt (sSrcHdl, &lpCvtBlk->usSrcPCM, phibSrcITC, &ciSrcChk)) {
                CVTERRMSG ("is not a Wave Multimedia Format File, skipping...\n");
                return (0L);
            }
            if (UNKPCM000 == lpCvtBlk->usSrcPCM) {
                CVTERRMSG ("has unknown Wave Multimedia PCM type, skipping...\n");
                return (0L);
            }
            if (NULL == (*ppibSrcITC = GloMemLck (*phibSrcITC))) {
                CVTERRMSG ("encountered a memory locking error, skipping...\n");
                return (0L);
            }
            ulBytReq = ciSrcChk.cksize;
            lpCvtBlk->usSrcChn = (*ppibSrcITC)->mwMSW.afWEX.wfx.nChannels;
            lpCvtBlk->ulSrcFrq = (*ppibSrcITC)->mwMSW.afWEX.wfx.nSamplesPerSec;

// ajm test 10/01/96
// Once the wave routines have loaded the MS-defined data, overwrite
// the ITCB with the VISI initialized information block.
switch (lpCvtBlk->usSrcPCM) {
    case DLGPCM004:
    case G11PCMF08:
    case G11PCMI08:
        PCMAloITC (lpCvtBlk->usSrcPCM, *ppibSrcITC, NULL);  
        break;
    default:
        break;
}

            break;

        /****************************************************************/
        /****************************************************************/
        default:
            CVTERRMSG ("has an unknown file format, skipping...\n");
            return (0L);
    }

    /********************************************************************/
    /* Set demo limits                                                  */
    /********************************************************************/
    if (REGDEMKEY == TBxGlo.usRegKey) {
        ulBytReq = min (ulBytReq, PCMSmp2Bh (lpCvtBlk->usSrcPCM, lpCvtBlk->usSrcChn,
            DEMFILDEF * lpCvtBlk->ulSrcFrq, *ppibSrcITC));
        if (TBxGlo.usDebFlg & INI___DEB) 
            MsgDspUsr ("\nBytReq: %ld ", ulBytReq);
    }

    /********************************************************************/
    /* Get configuration file parameters                                */
    /********************************************************************/
    if ((HARPCM001 == lpCvtBlk->usSrcPCM) || 
        (PTCPCM001 == lpCvtBlk->usSrcPCM) ||
        (NWVPCM001 == lpCvtBlk->usSrcPCM)) {
        ITCINI      iiIni;
        PCMAloITC (lpCvtBlk->usSrcPCM, NULL, &iiIni);       /* Load def */

        /****************************************************************/
        /* Use higher order filter for CVSD if requested                */
        /****************************************************************/
        if ((AAF_Q6TYP == lpCvtBlk->usTDLAAF) || lpCvtBlk->bfFFTAAF) {
            iiIni.ciCVS.usLowTyp = CVSLI6FLT;
        }

        /****************************************************************/
        /* Read configuration file                                      */
        /****************************************************************/
        if (lpCvtBlk->mhSrcCfg) {
            iiIni.ciCVS.usLowTyp = (WORD)  CfgLngGetDeb (lpCvtBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSLOWTYP, iiIni.ciCVS.usLowTyp);
            iiIni.ciCVS.ulLowPas = (DWORD) CfgLngGetDeb (lpCvtBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSLOWPAS, iiIni.ciCVS.ulLowPas);
            iiIni.ciCVS.usSigFtr = (WORD)  CfgLngGetDeb (lpCvtBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSSIGFTR, iiIni.ciCVS.usSigFtr);
            iiIni.ciCVS.usSylAtk = (WORD)  CfgLngGetDeb (lpCvtBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSSYLATK, iiIni.ciCVS.usSylAtk);
            iiIni.ciCVS.usSylDcy = (WORD)  CfgLngGetDeb (lpCvtBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSSYLDCY, iiIni.ciCVS.usSylDcy);
            iiIni.ciCVS.usCoiBit = (WORD)  CfgLngGetDeb (lpCvtBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSCOIBIT, iiIni.ciCVS.usCoiBit);
            iiIni.ciCVS.usStpMin = (WORD)  CfgLngGetDeb (lpCvtBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSSTPMIN, iiIni.ciCVS.usStpMin);
            iiIni.ciCVS.usStpMax = (WORD)  CfgLngGetDeb (lpCvtBlk->mhSrcCfg, SI_CVSSECNAM, SI_CVSSTPMAX, iiIni.ciCVS.usStpMax);
        }
        iiIni.ciCVS.ulSmpFrq = lpCvtBlk->ulSrcFrq;
        PCMAloITC (lpCvtBlk->usSrcPCM, *ppibSrcITC, &iiIni);  
    }

    /********************************************************************/
    /********************************************************************/
    return (min (ulBytReq, lpCvtBlk->ulSrcLen));

}

DWORD   DstFilPrm (short sDstHdl, CVTBLK FAR *lpCvtBlk, MMCKINFO FAR *pciDstChk,
        VISMEMHDL FAR *phibDstITC, ITCBLK FAR * FAR *ppibDstITC)
{
    static  ITCBLK  ibDstITC;

    /********************************************************************/
    /********************************************************************/
    switch (lpCvtBlk->usDstFmt) {
        case ALLPURFMT:
            *phibDstITC = (VISMEMHDL) NULL;
            PCMAloITC (lpCvtBlk->usDstPCM, (*ppibDstITC = &ibDstITC), NULL);
            break;

        /****************************************************************/
        /****************************************************************/
        case VISHDRFMT:
            if (VIXWrtFst (sDstHdl, "WAVE", 0L, &pciDstChk->dwDataOffset, FIOENCNON) ||
              VIXWrtTyp (sDstHdl, lpCvtBlk->usDstPCM, lpCvtBlk->usDstChn, 
              lpCvtBlk->ulDstFrq, FIOENCNON) || 
              VIXWrtDat (sDstHdl, &pciDstChk->cksize, FIOENCNON)) {
                CVTERRMSG ("encountered a file output error, skipping...\n");
                return (0L);
            }
            *phibDstITC = (VISMEMHDL) NULL;
            PCMAloITC (lpCvtBlk->usDstPCM, (*ppibDstITC = &ibDstITC), NULL);
            break;

        /****************************************************************/
        /****************************************************************/
        case WAVHDRFMT:
            if (WIOOutIni (sDstHdl, lpCvtBlk->usDstPCM, lpCvtBlk->usDstChn,
              lpCvtBlk->ulDstFrq, phibDstITC, pciDstChk)) {
                CVTERRMSG ("encountered a file output error, skipping...\n");
                return ((DWORD) -1L);
            }
            if (NULL == (*ppibDstITC = GloMemLck (*phibDstITC))) {
                CVTERRMSG ("encountered a memory locking error, skipping...\n");
                return ((DWORD) -1L);
            }
            break;

        /****************************************************************/
        /****************************************************************/
        default:
            CVTERRMSG ("has an unknown file format, skipping...\n");
            return ((DWORD) -1L);
    }

    /********************************************************************/
    /* Get configuration file parameters                                */
    /* Note: Low pass filter unused on output                           */
    /********************************************************************/
    if ((HARPCM001 == lpCvtBlk->usDstPCM) || 
        (PTCPCM001 == lpCvtBlk->usDstPCM) ||
        (NWVPCM001 == lpCvtBlk->usDstPCM)) {
        ITCINI      iiIni;
        PCMAloITC (lpCvtBlk->usDstPCM, NULL, &iiIni);       /* Load def */
        /****************************************************************/
        /* Read configuration file                                      */
        /****************************************************************/
        if (lpCvtBlk->mhDstCfg) {
            iiIni.ciCVS.usSigFtr = (WORD)  CfgLngGetDeb (lpCvtBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSSIGFTR, iiIni.ciCVS.usSigFtr);
            iiIni.ciCVS.usSylAtk = (WORD)  CfgLngGetDeb (lpCvtBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSSYLATK, iiIni.ciCVS.usSylAtk);
            iiIni.ciCVS.usSylDcy = (WORD)  CfgLngGetDeb (lpCvtBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSSYLDCY, iiIni.ciCVS.usSylDcy);
            iiIni.ciCVS.usCoiBit = (WORD)  CfgLngGetDeb (lpCvtBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSCOIBIT, iiIni.ciCVS.usCoiBit);
            iiIni.ciCVS.usStpMin = (WORD)  CfgLngGetDeb (lpCvtBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSSTPMIN, iiIni.ciCVS.usStpMin);
            iiIni.ciCVS.usStpMax = (WORD)  CfgLngGetDeb (lpCvtBlk->mhDstCfg, SI_CVSSECNAM, SI_CVSSTPMAX, iiIni.ciCVS.usStpMax);
        }
        iiIni.ciCVS.ulSmpFrq = lpCvtBlk->ulDstFrq;
        PCMAloITC (lpCvtBlk->usDstPCM, *ppibDstITC, &iiIni);  
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
        MsgDspUsr ("\n[%Fs] %Fs = %ld", szSecNam, szPrmNam, lDefVal);

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
        MsgDspUsr ("\n[%Fs] %Fs = %.2f", szSecNam, szPrmNam, flDefVal);

    /********************************************************************/
    /********************************************************************/
    return (flDefVal);
}

LPSTR   CfgStrGetDeb (VISMEMHDL mhCfgMem, LPSTR szSecNam, LPSTR szPrmNam, 
        LPSTR lpDefVal, LPSTR lpDstStr, WORD usDstLen)
{
    /********************************************************************/
    /* Display configuration if in debug mode                           */
    /********************************************************************/
    lpDefVal = CfgStrGet (mhCfgMem, szSecNam, szPrmNam, lpDefVal, lpDstStr, usDstLen);
    if (TBxGlo.usDebFlg & CFG___DEB) 
        MsgDspUsr ("\n[%Fs] %Fs = %Fs", szSecNam, szPrmNam, lpDefVal);

    /********************************************************************/
    /********************************************************************/
    return (lpDefVal);
}

char FAR *  _fitoa (int iValue, char FAR * lpString, int iRadix)
{
    static char szWrkBuf[TBXMAXSTR];

    /********************************************************************/
    /********************************************************************/
    _itoa (iValue, szWrkBuf, iRadix);
    _fstrncpy (lpString, szWrkBuf, min (TBXMAXSTR, strlen (szWrkBuf) + 1));

    /********************************************************************/
    /********************************************************************/
    return (lpString);
}
