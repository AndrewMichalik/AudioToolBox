/************************************************************************/
/* PCM Block Conversion Functions: BlkCvt.c             V2.00  07/15/95 */
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
#include <dos.h>                        /* Low level dos calls          */

/************************************************************************/
/*                  Conversion Routine References                       */
/************************************************************************/
extern  INIITCPRC   fpIniITC[MAXPCM000];
extern  SETSILPRC   fpSetSil[MAXPCM000];
extern  CMPITCPRC   fpCmpITC[MAXPCM000];
extern  PCMTOGPRC   fpPCMToG[MAXPCM000];
extern  GTOPCMPRC   fpGenToP[MAXPCM000];
extern  PCMTOMPRC   fpPCMToM[MAXPCM000];

/************************************************************************/
/*                  PCM to Larger G16, Block based                      */
/************************************************************************/
WORD    PCMPtoG16_LB (BYTE FAR *lpDstBuf, WORD usDstSiz, DWORD ulMaxByt,
        PCMTYP usPCMTyp, WORD usChnCnt, LPITCB lpITCBlk, WORD FAR *lpusInpByt,
        WORD FAR *lpErrCod, PCMBLKCBK fpInpCbk, LPVOID lpPolDat)
{
    WORD    usSrcByt;
    BYTE _based ((_segment) lpDstBuf) *bpSrcBuf;
    WORD    usOutSmp;

/********************************************************************/
/* Round byte count DOWN to usDstSiz limit until odd sample count   */
/* support is placed into all PCM to GenSmp conversions:            */
/* ___A04toG16 (..., usRsv001) will indicate sample output limit.   */
/********************************************************************/
if (PCMByt2Sl (usPCMTyp, usChnCnt, 1, NULL) > 1) {
    usDstSiz = (WORD) ((usDstSiz / sizeof(GENSMP)) / PCMByt2Sl (usPCMTyp, usChnCnt, 1, NULL));
    usDstSiz = (WORD) ((usDstSiz * sizeof(GENSMP)) * PCMByt2Sl (usPCMTyp, usChnCnt, 1, NULL));
    if (!usDstSiz) return (0);
}

    /********************************************************************/
    /* Convert to pointer with zero offset                              */
    /********************************************************************/
    if (!(lpDstBuf = BasAdrAlo (lpDstBuf, usDstSiz))) return (0);

    /********************************************************************/
    /* Select buffer positions so that last short overwrites last       */
    /* sample byte, ie, use last quarter of buffer for 4 bit PCM,       */
    /* last half for 8 bit PCM, last sixteenth for CVSD.                */
    /********************************************************************/
    usSrcByt = (WORD) min (usDstSiz, PCMSmp2Bh (usPCMTyp, usChnCnt, usDstSiz / sizeof(GENSMP), NULL));
    usSrcByt = (WORD) min (usSrcByt, ulMaxByt);
    bpSrcBuf = (BYTE _based ((_segment) lpDstBuf) *) 
        (FP_OFF(lpDstBuf) + usDstSiz - usSrcByt);

    /********************************************************************/
    /********************************************************************/
    usSrcByt = fpInpCbk (bpSrcBuf, usSrcByt, lpPolDat);

    /********************************************************************/
    /********************************************************************/
    usOutSmp = fpPCMToG[usPCMTyp] ((_segment) lpDstBuf, bpSrcBuf, &usSrcByt,
        (BYTE _based ((_segment) lpDstBuf) *) FP_OFF (lpDstBuf),
        usDstSiz / sizeof (GENSMP), lpITCBlk);
    if (lpusInpByt) *lpusInpByt = usSrcByt;         /* # Src bytes used */

    /********************************************************************/
    /********************************************************************/
    BasAdrRel (lpDstBuf);

    /********************************************************************/
    /********************************************************************/
    return (usOutSmp);

}

/************************************************************************/
/*                  G16 to Smaller PCM, Block based                     */
/************************************************************************/
WORD    PCMG16toP_SB (BYTE FAR *lpSrcBuf, WORD usSrcSiz, DWORD ulMaxSmp, 
        PCMTYP usPCMTyp, WORD usChnCnt, LPITCB lpITCBlk, WORD FAR *lpusInpSmp,
        WORD FAR *lpErrCod)
{
    WORD    usSrcSmp;
    WORD    usOutByt;

    /********************************************************************/
    /* Convert to pointer with zero offset                              */
    /********************************************************************/
    if (!(lpSrcBuf = BasAdrAlo (lpSrcBuf, usSrcSiz))) return (0);

    /********************************************************************/
    /********************************************************************/
    usSrcSmp = (WORD) min (usSrcSiz / sizeof(GENSMP), PCMByt2Sl (usPCMTyp, usChnCnt, usSrcSiz, NULL));
    usSrcSmp = (WORD) min (usSrcSmp, ulMaxSmp);

    /********************************************************************/
    /* Convert samples to PCM.                                          */
    /* Assume that PCM space requirements are <= original.              */
    /********************************************************************/
    usOutByt = fpGenToP[usPCMTyp] ((_segment) lpSrcBuf, 
        (BYTE _based ((_segment) lpSrcBuf) *) FP_OFF (lpSrcBuf), &usSrcSmp, 
        (BYTE _based ((_segment) lpSrcBuf) *) FP_OFF (lpSrcBuf),
        usSrcSmp, lpITCBlk);
    if (lpusInpSmp) *lpusInpSmp = usSrcSmp;         /* # Src samp used  */

    /********************************************************************/
    /********************************************************************/
    BasAdrRel (lpSrcBuf);

    /********************************************************************/
    /********************************************************************/
    return (usOutByt);

}

//WORD    PCMG16toP_LB (BYTE FAR *lpSrcBuf, WORD usSrcSiz, PCMTYP usPCMTyp, 
//        WORD usChnCnt, PCMBLKCBK fpInpCBk, LPVOID lpPolDat, LPITCB lpITCBlk, 
//        WORD FAR *lpusInpSmp)
//{
//    BYTE _based ((_segment) lpSrcBuf) *bpSrcBuf;
//    /********************************************************************/
//    /* Select buffer positions so that last PCM byte overwrites last    */
//    /* sample, ie, read last half of buffer for 32 bit PCM.             */
//    /********************************************************************/
//    usSrcSmp = (WORD) min (usSrcSiz / sizeof(GENSMP), PCMByt2Sl (usPCMTyp, usChnCnt, usSrcSiz, NULL));
//    bpSrcBuf = (BYTE _based ((_segment) lpSrcBuf) *) 
//        (FP_OFF(lpSrcBuf) + usSrcSiz - (usSrcSmp * sizeof(GENSMP)));
//}


