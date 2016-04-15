/************************************************************************/
/* PCM Max/Min Display Support: PCMMxM.c                V2.00  07/15/95 */
/* Copyright (c) 1987-1995 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
// Note: This whole file needs to be improved to incorporate Fast 
// Conversion for all PCM types, to share Fast and Exact processing
// memory algorithms, and to better handle memory to MxM processing

#include "windows.h"                    /* Windows SDK definitions      */
#include "genpcm.h"                     /* PCM/APCM conversion defs     */
#include "pcmsup.h"                     /* PCM/APCM xlate lib defs      */
#include "..\fiodev\filutl.h"           /* File utilities definitions   */

#include <string.h>                     /* String manipulation funcs    */
#include <math.h>                       /* Floating point math funcs    */
#include <dos.h>                        /* Low level dos calls          */

/************************************************************************/
/************************************************************************/
extern  PCMTOMPRC   fpPCMToM[MAXPCM000];
extern  WORD        usViwFtr[MAXPCM000];

/************************************************************************/
/************************************************************************/
#define MAXSETCNT   0xfff0L
typedef DWORD (*PCMTMMPRC) (short, BYTE FAR *, DWORD, MXMBLK FAR *, WORD, 
        float, PCMTYP, WORD, WORD, LPITCB, DWORD FAR *, WORD FAR *);
DWORD   FstPCMtoM (short, BYTE FAR *, DWORD, MXMBLK FAR *, WORD, 
        float, PCMTYP, WORD, WORD, LPITCB, DWORD FAR *, WORD FAR *);
DWORD   ExaPCMtoM (short, BYTE FAR *, DWORD, MXMBLK FAR *, WORD,
        float, PCMTYP, WORD, WORD, LPITCB, DWORD FAR *, WORD FAR *);

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL PCMRd_MxM (short sFilHdl, DWORD ulRsv001, MXMBLK FAR *lpmbDstBuf, 
                DWORD ulDstMxM, float flSetCnt, LPVOID lpWrkBuf, DWORD ulBufSiz,
                PCMTYP usPCMTyp, WORD usChnCnt, WORD usFIOEnc, LPITCB lpITCBlk,
                DWORD FAR *lpulInpByt, WORD FAR *lpErrCod, 
                PCMPOLPRC fpPolPrc, DWORD ulPolDat)
{
    PCMTMMPRC   fpMxMPrc;
    DWORD       ulXfrMxM = 0L;      /* Output  number of MxM pt sets    */
    DWORD       ulCurMxM;           /* Current number of MxM pt sets    */
    DWORD       ulCurByt;
    WORD        usErrCod;

    /********************************************************************/
    /* Test for valid PCM type selection                                */
    /********************************************************************/
    if ((usPCMTyp >= MAXPCM000) || !fpPCMToM[usPCMTyp]) return (0);

    /********************************************************************/
    /* Check set request parameters                                     */
    /********************************************************************/
    if (flSetCnt < 1) flSetCnt = 1;
        
    /********************************************************************/
    /* Show/Hide Progress indicator for time consuming operations       */
    /********************************************************************/
    if (fpPolPrc) fpPolPrc (PCMPOLBEG, ulDstMxM, ulPolDat);

    /********************************************************************/
    /* Choose algorithm depending upon Max / Min set count and          */
    /* availability of fast rendering routines                          */
    /********************************************************************/
    if ((flSetCnt >= usViwFtr[usPCMTyp]) && (fpPCMToM[usPCMTyp] != NulPCMtoM32))
         fpMxMPrc = FstPCMtoM;
    else fpMxMPrc = ExaPCMtoM;     

    /********************************************************************/
    /********************************************************************/
    while (ulXfrMxM < ulDstMxM) {
        /****************************************************************/
        /****************************************************************/
        ulCurMxM = fpMxMPrc (sFilHdl, lpWrkBuf, ulBufSiz, &lpmbDstBuf[ulXfrMxM], 
            (WORD) (ulDstMxM - ulXfrMxM), flSetCnt, usPCMTyp, usChnCnt,
            usFIOEnc, lpITCBlk, &ulCurByt, &usErrCod);
        if ((0L == ulCurMxM) || (-1L == ulCurMxM)) {
            if (-1L == ulCurMxM) ulXfrMxM = ulCurMxM;
            if (lpErrCod) *lpErrCod = usErrCod;
            break;
        }
        ulXfrMxM += ulCurMxM;

        /****************************************************************/
        /****************************************************************/
        if (fpPolPrc && !fpPolPrc (PCMPOLCNT,  ulXfrMxM, ulPolDat)) {
            ulXfrMxM = (DWORD) -1L;
            if (lpErrCod) *lpErrCod = PCM_E_ABT;
            break;
        }
    }
    if (lpulInpByt) *lpulInpByt = 0L;
    if (fpPolPrc) fpPolPrc (PCMPOLEND,  ulXfrMxM, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    return (ulXfrMxM);

}

#define TMPBUFSIZ   2000
char    ucWrkBuf[TMPBUFSIZ];    
DWORD FAR PASCAL PCMPtoMxM (LPVOID lpSrcBuf, DWORD ulSrcByt, 
        MXMBLK FAR *lpmbDstBuf, DWORD ulOscReq, float flSetCnt,  
        PCMTYP usPCMTyp, WORD usChnCnt, WORD usFIOEnc, LPITCB lpITCBlk, 
        DWORD FAR *lpulInpByt, WORD FAR *lpErrCod) 
{
    DWORD   ulSmpCnt;
    DWORD   uli, ulj;
    LPGSMP  lpgsWrkBuf = (LPGSMP) &ucWrkBuf;

    /********************************************************************/
    /* Test for valid PCM type selection                                */
    /********************************************************************/
    if (usPCMTyp >= MAXPCM000) return (0);

// ajm 03/18/96
if (!flSetCnt) return (0);

    /********************************************************************/
    /* Check set request parameters                                     */
    /********************************************************************/
    if (flSetCnt < 1) flSetCnt = 1;
if (flSetCnt > TMPBUFSIZ / sizeof (GENSMP)) flSetCnt = TMPBUFSIZ / sizeof (GENSMP);

// Temporary implementation
/********************************************************************/
/********************************************************************/
ulSmpCnt = PCMPtoG16 (lpSrcBuf, ulSrcByt, lpgsWrkBuf, TMPBUFSIZ / sizeof (GENSMP),
    usPCMTyp, usChnCnt, usFIOEnc, lpITCBlk, lpulInpByt, lpErrCod);

/********************************************************************/
/* Insure that results fit into existing buffer                     */
/********************************************************************/
ulOscReq = min (ulOscReq, (DWORD) ((TMPBUFSIZ / sizeof (GENSMP)) / flSetCnt));

for (uli=0; uli < ulOscReq; uli++) {
    ulj = ((WORD) ((uli + 1) * flSetCnt)) - ((WORD) (uli * flSetCnt));         
    lpmbDstBuf->gsMinWav = lpmbDstBuf->gsMaxWav = *lpgsWrkBuf++;
    while (--ulj) {                             /* 1st already used */   
        lpmbDstBuf->gsMinWav = min (lpmbDstBuf->gsMinWav, *lpgsWrkBuf);
        lpmbDstBuf->gsMaxWav = max (lpmbDstBuf->gsMaxWav, *lpgsWrkBuf);
        lpgsWrkBuf++;
    }
    lpmbDstBuf++;
}
return (ulOscReq);
        
}

/************************************************************************/
/************************************************************************/
DWORD   FstPCMtoM (short sFilHdl, BYTE FAR *lpWrkBuf, DWORD ulBufSiz, 
        MXMBLK FAR *lpmbDstBuf, WORD usOscReq, float flSetCnt, 
        PCMTYP usPCMTyp, WORD usChnCnt, WORD usFIOEnc, LPITCB lpITCBlk, 
        DWORD FAR *lpulInpByt, WORD FAR *lpErrCod)
{
    DWORD   ulSetCnt;
    DWORD   ulSetByt;
    DWORD   ulBytReq;
    DWORD   ulBytCnt;

    /********************************************************************/
    /* Limit set count to the number of PCM bytes that will fit buffer  */
    /* Note: This test will fail if PCM size is larger than GENSMP size */
    /* Round request to full set count bytes; trailing partial ignored. */
    /* Limit osc count to the number of full PCM sets that will fit     */
    /* Limit osc count to the number of max/ min sets that will fit     */
    /* Note: Divide buffer size by set count to prevent round up error  */
    /********************************************************************/
    ulSetCnt = (flSetCnt < MAXSETCNT) ? (DWORD) (flSetCnt + .5) : MAXSETCNT;
    if (!(ulSetByt = PCMSmp2Bh (usPCMTyp, usChnCnt, ulSetCnt, NULL))) return ((DWORD) -1L);
    if (!(usOscReq = min (usOscReq, (WORD) (ulBufSiz / ulSetByt)))) return ((DWORD) -1L);
    if (!(usOscReq = min (usOscReq, (WORD) (ulBufSiz / sizeof (MXMBLK))))) return ((DWORD) -1L);

    /********************************************************************/
    /* Resolution permits DC filtering on an oscillation point basis.   */
    /********************************************************************/
    ulBytReq = ulSetByt * usOscReq;
    ulBytCnt = Rd_FilFwd (sFilHdl, lpWrkBuf, (WORD) ulBytReq, FIOENCNON, lpErrCod);
    if ((-1L == ulBytCnt) || (0L == ulBytCnt)) return (ulBytCnt);

    /********************************************************************/
    /* Pad with silence to form full set.                               */
    /********************************************************************/
// Could do full sets, then partial sets without padding silence
    usOscReq = (WORD) ceil (ulBytCnt / (float) ulSetByt);
    ulBytReq = ulSetByt * usOscReq;
    if (ulBytCnt < ulBytReq) PCMSetSil (usPCMTyp, &lpWrkBuf[ulBytCnt], ulBytReq - ulBytCnt);

    /********************************************************************/
    /* Max / Min calculations are done in place.                        */
    /********************************************************************/
    fpPCMToM[usPCMTyp] ((_segment) lpWrkBuf,
        (BYTE _based ((_segment) lpWrkBuf) *) FP_OFF (lpWrkBuf), NULL,
        (BYTE _based ((_segment) lpWrkBuf) *) FP_OFF (lpWrkBuf), usOscReq,
        (WORD) ulSetCnt, lpITCBlk);
    _fmemcpy (lpmbDstBuf, lpWrkBuf, usOscReq * sizeof (*lpmbDstBuf));

    /********************************************************************/
    /********************************************************************/
    return ((DWORD) usOscReq);

}

/************************************************************************/
/************************************************************************/
DWORD   ExaPCMtoM (short sFilHdl, BYTE FAR *lpWrkBuf, DWORD ulBufSiz, 
        MXMBLK FAR *lpmbDstBuf, WORD usOscReq, float flSetCnt, 
        PCMTYP usPCMTyp, WORD usChnCnt, WORD usFIOEnc, LPITCB lpITCBlk, 
        DWORD FAR *lpulInpByt, WORD FAR *lpErrCod)
{
    DWORD   ulSmpReq;
    DWORD   ulSmpCnt;
    DWORD   ulBytReq;
    LPGSMP  lpgsWrkBuf = (LPGSMP) lpWrkBuf;
    WORD    usi, usj;

    /********************************************************************/
    /* Limit set count to the number of GENSMP's that will fit buffer   */
    /* Limit osc count to the number of GENSMP's / smp per set          */
    /********************************************************************/
    if (!(WORD) flSetCnt) flSetCnt = 1;
    if (flSetCnt > (MAXSETCNT / sizeof (GENSMP))) flSetCnt = (float) MAXSETCNT / sizeof (GENSMP);

    usOscReq = min (usOscReq, (WORD) ((ulBufSiz / sizeof (GENSMP)) / flSetCnt));
    ulSmpReq = (DWORD) (flSetCnt * usOscReq);
    ulBytReq = PCMSmp2Bh (usPCMTyp, usChnCnt, ulSmpReq, NULL);

    /********************************************************************/
    /* Read PCM data into linear sample buffer                          */
    /********************************************************************/
    ulSmpCnt = PCMRd_G16 (sFilHdl, ulBytReq, lpgsWrkBuf, ulSmpReq,
        usPCMTyp, usChnCnt, usFIOEnc, lpITCBlk, lpulInpByt, lpErrCod, NULL, 0L);
    if ((-1L == ulSmpCnt) || (0L == ulSmpCnt)) return (ulSmpCnt);

    /********************************************************************/
    /* Pad with silence to form full set.                               */
    /********************************************************************/
// Could do full sets, then partial sets without padding silence
    usOscReq = (WORD) ceil (ulSmpCnt / flSetCnt);
    ulSmpReq = (DWORD) ceil (flSetCnt * usOscReq);
    if (ulSmpCnt < ulSmpReq) _fmemset (&lpgsWrkBuf[ulSmpCnt],
        0, (WORD) ((ulSmpReq - ulSmpCnt) * sizeof (*lpgsWrkBuf)));

    /********************************************************************/
    /* Resolution prevents filtering on an oscillation point basis.     */
    /* Convert shorts to oscillations.                                  */
    /* Trailing partial oscillation data is dithered.                   */
    /********************************************************************/
    for (usi=0; usi < usOscReq; usi++) {
        usj = ((WORD) ((usi + 1) * flSetCnt)) - ((WORD) (usi * flSetCnt));         
        lpmbDstBuf->gsMinWav = lpmbDstBuf->gsMaxWav = *lpgsWrkBuf++;
        while (--usj) {                             /* 1st already used */   
            lpmbDstBuf->gsMinWav = min (lpmbDstBuf->gsMinWav, *lpgsWrkBuf);
            lpmbDstBuf->gsMaxWav = max (lpmbDstBuf->gsMaxWav, *lpgsWrkBuf);
            lpgsWrkBuf++;
        }
        lpmbDstBuf++;
    }
    
    /********************************************************************/
    /********************************************************************/
    return ((DWORD) usOscReq);

}


