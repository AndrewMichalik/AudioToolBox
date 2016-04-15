/************************************************************************/
/* PCM Far buffer I/O Support: PCMFIO.c                 V2.00  07/15/95 */
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
#include "genpcm.h"                     /* PCM/APCM conversion defs     */
#include "pcmsup.h"                     /* PCM/APCM xlate lib defs      */
#include "..\fiodev\filutl.h"           /* File utilities definitions   */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */
#include <errno.h>                      /* errno value constants        */
#include <math.h>                       /* Floating point math funcs    */
#include <dos.h>                        /* Low level dos calls          */
#include <stdio.h>                      /* Standard I/O                 */
#include <io.h>                         /* Low level file I/O           */

/************************************************************************/
/*                  Conversion Routine References                       */
/************************************************************************/
extern  SETSILPRC   fpSetSil[MAXPCM000];
extern  ALOITCPRC   fpAloITC[MAXPCM000];
extern  INIITCPRC   fpIniITC[MAXPCM000];
extern  CMPITCPRC   fpCmpITC[MAXPCM000];
extern  RELITCPRC   fpRelITC[MAXPCM000];
extern  PCMTOGPRC   fpPCMToG[MAXPCM000];
extern  GTOPCMPRC   fpGenToP[MAXPCM000];
extern  DWORD       ulGrdSmp[MAXPCM000];

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL PCMSetSil (PCMTYP usPCMTyp, LPVOID lpDstBuf, DWORD ulBufSiz)
{
    /********************************************************************/
    /* Test for valid PCM type selection                                */
    /********************************************************************/
    if ((usPCMTyp >= MAXPCM000) || !fpSetSil[usPCMTyp]) return (0L);

//Need to handle buffers > 64K
    return (fpSetSil[usPCMTyp] (lpDstBuf, (WORD) ulBufSiz));
}

LPVOID FAR PASCAL PCMAloITC (PCMTYP usPCMTyp, LPITCB lpITCBlk, LPITCI lpITCIni)
{
    /********************************************************************/
    /* Test for valid PCM type selection                                */
    /********************************************************************/
    if ((usPCMTyp >= MAXPCM000) || !fpAloITC[usPCMTyp] || !fpIniITC[usPCMTyp]) 
        return (lpITCBlk);

    /********************************************************************/
    /* If (NULL == lpITCBlk), fill lpITCIni with default values         */
    /* If (NULL == lpITCIni), initialize lpITCBlk from default values   */
    /* If (NULL == neither),  initialize lpITCBlk from lpITCIni values  */
    /********************************************************************/
    fpAloITC[usPCMTyp] (lpITCBlk, lpITCIni);
    return (lpITCBlk ? fpIniITC[usPCMTyp] (lpITCBlk) : lpITCBlk);
}

DWORD FAR PASCAL PCMGrdITC (short sFilHdl, DWORD ulReqSmp, LPVOID lpWrkBuf, 
                 DWORD ulBufSiz, PCMTYP usPCMTyp, WORD usChnCnt, 
                 WORD usFIOEnc, LPITCB lpITCBlk, LPVOID lpRsv001, 
                 WORD FAR *lpErrCod, PCMPOLPRC fpPolPrc, DWORD ulPolDat)
{
    WORD    usBytCnt;
    DWORD   ulGrdByt;
    WORD    usBufSiz = (WORD) min (ulBufSiz, BYTMAXFIO);
    DWORD   ulFilOff = FilGetPos (sFilHdl);     /* Current PCM file pos     */
    WORD    usErrCod = 0;

    /********************************************************************/
    /* Test for valid PCM type selection                                */
    /********************************************************************/
    if ((usPCMTyp >= MAXPCM000) || !fpIniITC[usPCMTyp] || !fpCmpITC[usPCMTyp]) 
        return (0);

    /********************************************************************/
    /* Initialize ITC Block to "no" history                             */
    /********************************************************************/
    fpIniITC[usPCMTyp] (lpITCBlk);

    /********************************************************************/
    /* Does PCM Type require "guard" samples for good ADPCM/CVSD/etc?   */
    /********************************************************************/
    if (!lpWrkBuf || !ulBufSiz) return (0);
    if (!ulReqSmp && !(ulReqSmp = ulGrdSmp[usPCMTyp])) return (0);
    ulGrdByt = PCMSmp2Bh (usPCMTyp, usChnCnt, ulReqSmp, NULL);

    /********************************************************************/
    /* Set initial file offset to beginning of read request             */
    /********************************************************************/
    if (ulGrdByt >= ulFilOff) {
        ulGrdByt = ulFilOff;
        FilSetPos (sFilHdl, 0L, SEEK_SET);
    } else FilSetPos (sFilHdl, ulFilOff - ulGrdByt, SEEK_SET);

    /********************************************************************/
    /* Compute IniVal using 1 DC filter set, where set is whole buffer  */          
    /********************************************************************/
    while (ulGrdByt > 0L) {
        usBytCnt = Rd_FilFwd (sFilHdl, lpWrkBuf,
            (WORD) min (ulGrdByt , (DWORD) usBufSiz), usFIOEnc, &usErrCod);
        if ((-1 == usBytCnt) || (0 == usBytCnt)) break;
        fpCmpITC[usPCMTyp] ((_segment) lpWrkBuf,
            (BYTE _based(lpWrkBuf) *) FP_OFF (lpWrkBuf), NULL,
            (WORD) PCMByt2Sl (usPCMTyp, usChnCnt, usBytCnt, NULL), lpITCBlk);
        ulGrdByt -= (DWORD) usBytCnt;
    }

    /********************************************************************/
    /* Resets pointer to file original file offset                      */
    /********************************************************************/
    FilSetPos (sFilHdl, ulFilOff, SEEK_SET);
    if (lpErrCod) *lpErrCod = usErrCod;
    return (usBytCnt);

}

LPVOID FAR PASCAL PCMRelITC (PCMTYP usPCMTyp, LPITCB lpITCBlk)
{
    /********************************************************************/
    /* Test for valid PCM type selection                                */
    /********************************************************************/
    if ((usPCMTyp >= MAXPCM000) || !fpRelITC[usPCMTyp]) return (lpITCBlk);

    return (fpRelITC[usPCMTyp] (lpITCBlk));
}

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL PCMRd_G16 (short sFilHdl, DWORD ulSrcByt, 
                 LPGSMP lpgsDstBuf, DWORD ulDstSmp, PCMTYP usPCMTyp, 
                 WORD usChnCnt, WORD usFIOEnc, LPITCB lpITCBlk, 
                 DWORD FAR *lpulInpByt, WORD FAR *lpErrCod, 
                 PCMPOLPRC fpPolPrc, DWORD ulPolDat)
{
    WORD    FAR PASCAL PCMRd_G16CBk (BYTE FAR *, WORD, LPVOID);
    RW_CBK  cbRW_CBk;
    DWORD   ulXfrByt = 0L;
    DWORD   ulXfrSmp = 0L;
    WORD    usCurByt;
    WORD    usOutSmp;

    /********************************************************************/
    /* Test for valid PCM type selection                                */
    /********************************************************************/
    if ((usPCMTyp >= MAXPCM000) || !fpPCMToG[usPCMTyp]) return (0);

    /********************************************************************/
    /* If multiple passes will be required, round destination sample    */
    /* down count for evenly blocked output                             */
    /********************************************************************/
    if (PCMByt2Sl (usPCMTyp, usChnCnt, ulSrcByt, lpITCBlk) > ulDstSmp) {
        ulDstSmp = PCMSmpLow (usPCMTyp, usChnCnt, ulDstSmp, lpITCBlk);
        ulSrcByt = min (ulSrcByt, PCMSmp2Bh (usPCMTyp, usChnCnt, ulDstSmp, lpITCBlk));
    }

    /********************************************************************/
    /* Show/Hide Progress indicator for time consuming operations       */
    /********************************************************************/
    if (fpPolPrc) fpPolPrc (PCMPOLBEG, ulSrcByt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    cbRW_CBk.sFilHdl = sFilHdl;
    cbRW_CBk.sFIOEnc = usFIOEnc;
    cbRW_CBk.iErrCod = 0;

    /********************************************************************/
    /* Convert PCM to samples using destination for work buffer.        */
    /********************************************************************/
    while ((ulSrcByt > 0L) && (ulDstSmp > 0)) {
        usOutSmp = PCMPtoG16_LB (
            (BYTE FAR *) (((GENSMP huge *) lpgsDstBuf) + ulXfrSmp), 
            (WORD) min (ulDstSmp * sizeof(GENSMP), BYTMAXFIO), ulSrcByt, 
            usPCMTyp, usChnCnt, lpITCBlk, &usCurByt, &cbRW_CBk.iErrCod, 
            PCMRd_G16CBk, (LPVOID) &cbRW_CBk);

        /****************************************************************/
        /****************************************************************/
        if (0 == usOutSmp) {
            if (!cbRW_CBk.iErrCod) break;
            ulXfrByt = 0L;
            ulXfrSmp = (DWORD) -1L;
            break;
        }

        /****************************************************************/
        /****************************************************************/
        ulXfrByt += (DWORD) usCurByt;
        ulXfrSmp += (DWORD) usOutSmp;
        ulSrcByt -= (DWORD) usCurByt;
        ulDstSmp -= (DWORD) usOutSmp;

        /****************************************************************/
        /****************************************************************/
        if (fpPolPrc && !fpPolPrc (PCMPOLCNT,  ulXfrByt, ulPolDat)) {
            ulXfrByt = 0L;
            ulXfrSmp = (DWORD) -1L;
            cbRW_CBk.iErrCod = PCM_E_ABT;
            break;
        }
    }
    if (lpulInpByt) *lpulInpByt = ulXfrByt;
    if (fpPolPrc) fpPolPrc (PCMPOLEND,  ulXfrByt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    if (lpErrCod) *lpErrCod = cbRW_CBk.iErrCod;
    return (ulXfrSmp);

}
WORD  FAR PASCAL PCMRd_G16CBk (BYTE FAR *lpDstBuf, WORD usDstByt, 
                 LPVOID lpPolDat)
{
    WORD    usInpByt;

    /********************************************************************/
    /********************************************************************/
    usInpByt = Rd_FilFwd (((RW_CBK FAR *) lpPolDat)->sFilHdl, lpDstBuf, 
        usDstByt, ((RW_CBK FAR *) lpPolDat)->sFIOEnc, 
        &((RW_CBK FAR *) lpPolDat)->iErrCod);
    if (-1 == usInpByt) usInpByt = 0;
    return (usInpByt);
}

DWORD FAR PASCAL PCMWr_PCM (LPGSMP lpgsSrcBuf, DWORD ulSrcSmp, 
                 short sFilHdl, DWORD ulDstByt, PCMTYP usPCMTyp, 
                 WORD usChnCnt, WORD usFIOEnc, LPITCB lpITCBlk, 
                 DWORD FAR *lpulInpSmp, WORD FAR *lpErrCod, 
                 PCMPOLPRC fpPolPrc, DWORD ulPolDat)
{
    DWORD   ulXfrSmp = 0L;
    DWORD   ulXfrByt = 0L;
    WORD    usErrCod = 0;
    WORD    usInpSmp;
    WORD    usOutByt;

    /********************************************************************/
    /* Test for valid PCM type selection                                */
    /********************************************************************/
    if ((usPCMTyp >= MAXPCM000) || !fpGenToP[usPCMTyp]) return (0);

    /********************************************************************/
    /* If multiple passes will be required, round source sample         */
    /* count down for evenly blocked output                             */
    /********************************************************************/
    if (ulSrcSmp > PCMByt2Sl (usPCMTyp, usChnCnt, ulDstByt, lpITCBlk)) {
        ulSrcSmp = PCMByt2Sl (usPCMTyp, usChnCnt, ulDstByt, lpITCBlk);
        ulSrcSmp = PCMSmpLow (usPCMTyp, usChnCnt, ulSrcSmp, lpITCBlk);
    }

    /********************************************************************/
    /* Show/Hide Progress indicator for time consuming operations       */
    /********************************************************************/
    if (fpPolPrc) fpPolPrc (PCMPOLBEG, ulSrcSmp, ulPolDat);

    /********************************************************************/
    /* Convert samples to PCM using source buffer for work buffer.      */
    /* Assume that PCM space requirements are <= original.              */
    /* If not, use PCMG16toP_LB().                                      */
    /********************************************************************/
    while ((ulSrcSmp > 0L) && (ulDstByt > 0)) {
        usOutByt = PCMG16toP_SB (
            (BYTE FAR *) (((GENSMP huge *) lpgsSrcBuf) + ulXfrSmp), 
// ajm test - revert to pre 05/95
(WORD) min (ulSrcSmp * sizeof (*lpgsSrcBuf), BYTMAXFIO), ulSrcSmp, usPCMTyp,
//            (WORD) min (ulDstByt, BYTMAXFIO), ulSrcSmp, usPCMTyp, 
            usChnCnt, lpITCBlk, &usInpSmp, &usErrCod);
        if (0 == usOutByt) break;

        /****************************************************************/
        /****************************************************************/
        usOutByt = Wr_FilFwd (sFilHdl, 
            (BYTE FAR *) (((GENSMP huge *) lpgsSrcBuf) + ulXfrSmp),
// ajm test - support for pre 05/95
//            usOutByt, usFIOEnc, &usErrCod);
(WORD) min (ulDstByt, (DWORD) usOutByt), usFIOEnc, &usErrCod);
    
        /****************************************************************/
        /****************************************************************/
        ulXfrSmp += (DWORD) usInpSmp;
        ulXfrByt += (DWORD) usOutByt;
        ulSrcSmp -= (DWORD) usInpSmp;
        ulDstByt -= (DWORD) usOutByt;

        /****************************************************************/
        /****************************************************************/
        if (fpPolPrc && !fpPolPrc (PCMPOLCNT,  ulXfrSmp, ulPolDat)) {
            ulXfrByt = 0L;
            ulXfrSmp = (DWORD) -1L;
            usErrCod = PCM_E_ABT;
            break;
        }
    }
    if (lpulInpSmp) *lpulInpSmp = ulXfrSmp;
    if (fpPolPrc) fpPolPrc (PCMPOLEND,  ulXfrSmp, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    if (lpErrCod) *lpErrCod = usErrCod;
    return (ulXfrByt);

}

/************************************************************************/
/*              PCM Memory to Memory Conversion Functions               */
/************************************************************************/
DWORD FAR PASCAL PCMPtoG16 (LPSTR lpSrcBuf, DWORD ulSrcByt, 
                 LPGSMP lpgsDstBuf, DWORD ulDstSmp, PCMTYP usPCMTyp, 
                 WORD usChnCnt, WORD usFIOEnc, LPITCB lpITCBlk, 
                 DWORD FAR *lpulInpByt, WORD FAR *lpErrCod)
{
    WORD    FAR PASCAL PCMPtoG16CBk (BYTE FAR *, WORD, LPVOID);
    DWORD   ulXfrByt = 0L;
    DWORD   ulXfrSmp = 0L;
    WORD    usErrCod = 0;
    WORD    usCurByt;
    WORD    usOutSmp;

    /********************************************************************/
    /* Test for valid PCM type selection                                */
    /********************************************************************/
    if ((usPCMTyp >= MAXPCM000) || !fpPCMToG[usPCMTyp]) return (0);

    /********************************************************************/
    /* If multiple passes will be required, round destination sample    */
    /* down count for evenly blocked output                             */
    /********************************************************************/
    if (PCMByt2Sl (usPCMTyp, usChnCnt, ulSrcByt, lpITCBlk) > ulDstSmp) {
        ulDstSmp = PCMSmpLow (usPCMTyp, usChnCnt, ulDstSmp, lpITCBlk);
        ulSrcByt = min (ulSrcByt, PCMSmp2Bh (usPCMTyp, usChnCnt, ulDstSmp, lpITCBlk));
    }

    /********************************************************************/
    /* Assume that PCM space requirements are > original, save to end.  */
    /* If not, use PCMPtoG16_SB().                                      */
    /********************************************************************/
    if ((LPVOID) lpSrcBuf == (LPVOID) lpgsDstBuf) lpSrcBuf = hmemmove 
        (((char huge *)(&lpgsDstBuf[ulDstSmp])) - ulSrcByt, lpSrcBuf, ulSrcByt);

    /********************************************************************/
    /* Convert PCM to samples using destination for work buffer.        */
    /********************************************************************/
    while ((ulSrcByt > 0L) && (ulDstSmp > 0)) {
        usOutSmp = PCMPtoG16_LB (
            (BYTE FAR *) (((GENSMP huge *) lpgsDstBuf) + ulXfrSmp), 
            (WORD) min (ulDstSmp * sizeof(GENSMP), BYTMAXFIO), ulSrcByt, 
            usPCMTyp, usChnCnt, lpITCBlk, &usCurByt, &usErrCod, PCMPtoG16CBk,
            ((char huge *) lpSrcBuf) + ulXfrByt);
        if (0 == usOutSmp) break;

        /****************************************************************/
        /****************************************************************/
        ulXfrByt += (DWORD) usCurByt;
        ulXfrSmp += (DWORD) usOutSmp;
        ulSrcByt -= (DWORD) usCurByt;
        ulDstSmp -= (DWORD) usOutSmp;
    }
    if (lpulInpByt) *lpulInpByt = ulXfrByt;

    /********************************************************************/
    /********************************************************************/
    if (lpErrCod) *lpErrCod = usErrCod;
    return (ulXfrSmp);

}
WORD  FAR PASCAL PCMPtoG16CBk (BYTE FAR *lpDstBuf, WORD usDstByt, 
                 LPVOID lpPolDat)
{
    /********************************************************************/
    /* Assume that PCM space requirements are <= original.              */
    /********************************************************************/
    _fmemmove (lpDstBuf, lpPolDat, usDstByt);
    return (usDstByt);
}

DWORD FAR PASCAL PCMG16toP (LPGSMP lpgsSrcBuf, DWORD ulSrcSmp, 
                 LPSTR lpDstBuf, DWORD ulDstByt, PCMTYP usPCMTyp,
                 WORD usChnCnt, WORD usFIOEnc, LPITCB lpITCBlk, 
                 DWORD FAR *lpulInpSmp, WORD FAR *lpErrCod)
{
    DWORD   ulXfrSmp = 0L;
    DWORD   ulXfrByt = 0L;
    WORD    usErrCod = 0;
    WORD    usInpSmp;
    WORD    usOutByt;

    /********************************************************************/
    /* Test for valid PCM type selection                                */
    /********************************************************************/
    if ((usPCMTyp >= MAXPCM000) || !fpGenToP[usPCMTyp]) return (0);

    /********************************************************************/
    /* If multiple passes will be required, round source sample         */
    /* count down for evenly blocked output                             */
    /********************************************************************/
    if (ulSrcSmp > PCMByt2Sl (usPCMTyp, usChnCnt, ulDstByt, lpITCBlk)) {
        ulSrcSmp = PCMByt2Sl (usPCMTyp, usChnCnt, ulDstByt, lpITCBlk);
        ulSrcSmp = PCMSmpLow (usPCMTyp, usChnCnt, ulSrcSmp, lpITCBlk);
    }

    /********************************************************************/
    /* Convert samples to PCM using source buffer for work buffer.      */
    /* Assume that PCM space requirements are <= original.              */
    /* If not, use PCMG16toP_LB().                                      */
    /********************************************************************/
    while ((ulSrcSmp > 0L) && (ulDstByt > 0)) {
        usOutByt = PCMG16toP_SB (
            (BYTE FAR *) (((GENSMP huge *) lpgsSrcBuf) + ulXfrSmp), 
// ajm test - revert to pre 05/95
(WORD) min (ulSrcSmp * sizeof (*lpgsSrcBuf), BYTMAXFIO), ulSrcSmp, usPCMTyp,
//            (WORD) min (ulDstByt, BYTMAXFIO), ulSrcSmp, usPCMTyp, 
            usChnCnt, lpITCBlk, &usInpSmp, &usErrCod);
        if (0 == usOutByt) break;

        /****************************************************************/
        /****************************************************************/
        _fmemmove (((char huge *) lpDstBuf) + ulXfrByt, 
// ajm test - support for pre 05/95
//            ((GENSMP huge *) lpgsSrcBuf) + ulXfrSmp, usOutByt);
((GENSMP huge *) lpgsSrcBuf) + ulXfrSmp, (WORD) min (ulDstByt, (DWORD) usOutByt));
    
        /****************************************************************/
        /****************************************************************/
        ulXfrSmp += (DWORD) usInpSmp;
        ulXfrByt += (DWORD) usOutByt;
        ulSrcSmp -= (DWORD) usInpSmp;
        ulDstByt -= (DWORD) usOutByt;
    }
    if (lpulInpSmp) *lpulInpSmp = ulXfrSmp;

    /********************************************************************/
    /********************************************************************/
    if (lpErrCod) *lpErrCod = usErrCod;
    return (ulXfrByt);

}

/************************************************************************/
/*                  A to D resolution conversion routines               */
/************************************************************************/
DWORD FAR PASCAL PCML16toG (short FAR *lpsSrcBuf, LPGSMP lpgsDstBuf, 
                 DWORD ulSmpCnt, WORD usDstRes)
{
    WORD            usShfCnt;
    long            lAtDMag;
    DWORD           uli = ulSmpCnt;

    /********************************************************************/
    /* Check linear resolution range & calculate shift count            */
    /********************************************************************/
    if (!usDstRes || (usDstRes >  16)) return (0L);
    if (usDstRes == 16) {
        if ((void huge *) lpsSrcBuf != (void huge *) lpgsDstBuf)
            hmemmove (lpgsDstBuf, lpsSrcBuf, ulSmpCnt * sizeof (*lpgsDstBuf));
        return (ulSmpCnt);
    }

    /********************************************************************/
    /********************************************************************/
    usShfCnt = 16 - usDstRes;
    lAtDMag = ((DWORD) pow (2, usDstRes - 1)) - 1;

    /********************************************************************/
    /* Shift two's comp full range 16 bit right to fit A to D limits    */
    /********************************************************************/
    while (uli-- > 0) {
        long    lDstSmp = *((short huge *)lpsSrcBuf)++ >> (usShfCnt - 1);
        lDstSmp = ((lDstSmp < 0) ? (lDstSmp - 1) : (lDstSmp + 1)) >> 1;
        if (lDstSmp > lAtDMag) lDstSmp = (GENSMP) lAtDMag; 
        else if (lDstSmp < -lAtDMag) lDstSmp = (GENSMP) -lAtDMag; 
        *((GENSMP huge *)lpgsDstBuf)++ = (GENSMP) lDstSmp;
    }
    return (ulSmpCnt);

}

DWORD FAR PASCAL PCMGtoL16 (LPGSMP lpgsSrcBuf, short FAR *lpsDstBuf, 
                 DWORD ulSmpCnt, WORD usSrcRes)
{
    WORD    usShfCnt;
    WORD    usRndBit;
    long    lAtDMag;
    DWORD   uli = ulSmpCnt;

    /********************************************************************/
    /* Check linear resolution range & calculate shift count            */
    /********************************************************************/
    if (!usSrcRes || (usSrcRes >  16)) return (0L);
    if (usSrcRes == 16) {
        if ((void huge *) lpgsSrcBuf != (void huge *) lpsDstBuf)
            hmemmove (lpsDstBuf, lpgsSrcBuf, ulSmpCnt * sizeof (*lpsDstBuf));
        return (ulSmpCnt);
    }

    /********************************************************************/
    /********************************************************************/
    usShfCnt = 16 - usSrcRes;
    lAtDMag = ((DWORD) pow (2, usSrcRes - 1)) - 1;

    /********************************************************************/
    /* Shift two's comp A to D limited left for full range 16 bit       */
    /* Bitwise "or" one bit right minus one to reduce average error     */
    /********************************************************************/
    usRndBit = (usShfCnt > 1) ? (0x7fff >> (15 - (usShfCnt-1))) : 0;
    while (uli--) {
        long    lSrcSmp = *((GENSMP huge *)lpgsSrcBuf)++;
        if (lSrcSmp > lAtDMag) lSrcSmp = lAtDMag; 
        else if (lSrcSmp < -lAtDMag) lSrcSmp = -lAtDMag; 
        *((short huge *)lpsDstBuf)++ = 
            (short) (lSrcSmp ? ((lSrcSmp << usShfCnt) | usRndBit) : 0);
    }
    return (ulSmpCnt);

}

DWORD FAR PASCAL PCMGtoAtD (LPGSMP lpgsSrcBuf, short FAR *lpsDstBuf, 
                 DWORD ulSmpCnt, short usDltRes)
{
    return (0L);
}
