/************************************************************************/
/* Effects FFT based Equalizer: FFTEqu.c                V2.00  07/15/95 */
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
#include "geneff.h"                     /* Sound Effects definitions    */
#include "effsup.h"                     /* Sound Effects definitions    */

#include <math.h>                       /* Math library defs            */
#include <string.h>                     /* String manipulation          */
#include <stdlib.h>                     /* Misc string/math/error funcs */

/************************************************************************/
/************************************************************************/
DWORD   FltIntDec (float FAR *, float FAR *, DWORD, DWORD, DWORD, float);

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL EffEquAlo (DWORD ulEquBot, DWORD ulEquTop, DWORD ulSmpFrq, 
                WORD usEquBnd, float FAR *lpEquArr, float flEquGai, 
                LPFFTB lpFFTBlk)
{
    CPXFLT FAR *lpXfrBuf;
    DWORD       ulXfrLen;
    float  FAR *lpFtrBuf;
    DWORD       ulFtrFrq;
    DWORD       ulFtrLen;
    DWORD       ulIdxBot;
    DWORD       ulIdxTop;
    DWORD       uli;

    /********************************************************************/
    /* Note: Caller call EffWinRsp() after this routine to convert      */
    /* these equalizer settings into a valid frequency domain filter.   */
    /********************************************************************/

    /********************************************************************/
    /* Set filter freq response to 1/2 Nyquist rate                     */
    /********************************************************************/
    if (!(ulFtrFrq = ulSmpFrq / 2L)) return ((WORD) -1);
    if (!(ulXfrLen = lpFFTBlk->ulFFTPts)) return ((WORD) -1);

    /********************************************************************/
    /* Set index range for filter freq response (not Nyquist rate!)     */
    /********************************************************************/
    ulIdxBot = ulXfrLen * min (ulEquBot, ulFtrFrq) / ulFtrFrq;
    ulIdxTop = ulXfrLen * min (ulEquTop, ulFtrFrq) / ulFtrFrq;
    if (ulIdxBot >= ulIdxTop) return ((WORD) -1);

    /********************************************************************/
    /* Lock FFT filter transfer buffer                                  */
    /********************************************************************/
    if (NULL == (lpXfrBuf = GloMemLck (lpFFTBlk->mhXfrHdl))) {
        return ((WORD) -1);
    }
    lpFtrBuf = (float FAR *) &lpXfrBuf[ulXfrLen/2];

    /********************************************************************/
    /* Resample filter data points to scale of source frequency         */
    /* Set initial data point to 0 if bottom freq is not at freq bin 0  */
    /********************************************************************/
    ulFtrLen = FltIntDec (lpEquArr, lpFtrBuf, usEquBnd, usEquBnd, 
        ulIdxTop - ulIdxBot, ulIdxBot ? 0 : lpEquArr[0]);
    if (!ulFtrLen) return ((WORD) -1);

    /********************************************************************/
    /* Shift filter to bottom frequency offset, zero fill tail          */
    /********************************************************************/
    if (ulFtrLen < ulXfrLen) {
        if (ulIdxBot < ulXfrLen) _fmemmove 
            (&lpFtrBuf[ulIdxBot], lpFtrBuf, (WORD) (ulXfrLen - ulIdxBot));
        if (ulIdxTop < ulXfrLen) _fmemset
            (&lpFtrBuf[ulIdxTop], 0, (WORD) (ulXfrLen - ulIdxTop) * sizeof (*lpFtrBuf));
    }

    /********************************************************************/
    /* Convert dB to voltage multiplier                                 */
    /********************************************************************/
    for (uli=0; uli < ulXfrLen; uli++) 
        lpFtrBuf[uli] = (float) DB_TO_VLT (lpFtrBuf[uli] + flEquGai);        

    /********************************************************************/
	/* Handle DC and N/2 as special case; pack into real FFT format		*/
    /* Spread real filter over complex transfer function                */
    /********************************************************************/
    lpXfrBuf[0].flRea *= lpFtrBuf[0];       
    lpXfrBuf[0].flImg *= lpFtrBuf[ulXfrLen-1];
    for (uli=1; uli < ulXfrLen; uli++) {
        lpXfrBuf[uli].flRea *= lpFtrBuf[uli];       
        lpXfrBuf[uli].flImg *= lpFtrBuf[uli];
    }

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (lpFFTBlk->mhXfrHdl);
    return (0);

}

WORD FAR PASCAL EffEquRel (LPFFTB lpFFTBlk)
{
    /********************************************************************/
    /********************************************************************/
    return (0);
}

DWORD   FltIntDec (float FAR *lpSrcBuf, float FAR *lpDstBuf,
        DWORD ulSrcCnt, DWORD ulSmpFrq, DWORD ulDstFrq, float flIniVal)
{
    #define FIDFRAONE   (1L << 4)           /* Normalized unity value   */   
    RESBLK  rbResBlk = {0L, 0};         	/* Int/Dec call block       */
    LPGSMP  lpSrcInt = (LPGSMP) lpSrcBuf;
    DWORD   ulDstCnt;
    DWORD   uli;

    /********************************************************************/
    /* Convert floats to shifted shorts                                 */
    /********************************************************************/
    for (uli=0; uli < ulSrcCnt; uli++)
        lpSrcInt[uli] = (GENSMP) (FIDFRAONE * lpSrcBuf[uli]);
    rbResBlk.gsCurWav = (GENSMP) (FIDFRAONE * flIniVal);

    /********************************************************************/
    /********************************************************************/
    ulDstCnt = EffIntDec (lpSrcInt, (LPGSMP) lpDstBuf, ulSrcCnt, ulSmpFrq, ulDstFrq,
         &rbResBlk, NULL, NULL, 0L);
    if (!ulDstCnt) return (0L);

    /********************************************************************/
    /* Convert resampled, shifted shorts back to floats                 */
    /********************************************************************/
    lpSrcInt = (LPGSMP) lpDstBuf;
    for (uli=ulDstCnt; uli > 0; uli--)
        lpDstBuf[uli-1L] = lpSrcInt[uli-1L] / (float) FIDFRAONE;

    /********************************************************************/
    /********************************************************************/
    return (ulDstCnt);

}

