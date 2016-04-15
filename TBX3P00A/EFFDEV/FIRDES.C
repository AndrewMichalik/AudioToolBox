/************************************************************************/
/* IIR Filter Design: IIRDes.c                          V2.00  08/15/95 */
/* Copyright (c) 1987-1995 Andrew J. Michalik                           */
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
  
#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */
#include <math.h>                       /* Math library defs            */

/************************************************************************/
/* FIR Lowpass filter non-optimal design using windowed Fourier series. */
/* usLen  = Length: f(z) = b(0)+b(1)*z**(-1) +...+ b(l)*z**(usLen - 1)  */
/* flFrq  = Normalized cut-off freq in hertz-seconds (FtrFrq/SmpFrq)    */
/* usWin  = Window used to truncate fourier series                      */
/* usCoe  = Digital filter coefficients returned (0:usLen-1)            */
/* ierror = 0 no errors detected                                        */
/*          1 invalid filter length                                     */
/*          2 invalid window type iwndo                                 */
/*          3 invalid cut-off fcn; <=0 or >=0.5                         */
/************************************************************************/
WORD    EffFIRNOp (WORD usLen, float flFrq, WINTYP usWin, BOOL bfFstFlg, 
        LPTDOF lpTDoFtr)

{
    WORD    usi;
    float   flDly;

    /********************************************************************/
    /********************************************************************/
    _fmemset (lpTDoFtr, 0, sizeof (*lpTDoFtr));         /* Initialize   */

    /********************************************************************/
    /********************************************************************/
    if (usLen <= 1) return (1);
    if (usWin < 1 || usWin > 6) return (2);
    if (flFrq <= 0.0 || flFrq >= 0.5) return (3);

    /********************************************************************/
    /* If length is odd, set middle coefficient                         */
    /* Note: all windows equal unity at exact center                    */
    /********************************************************************/
    for (usi = 0; usi < usLen; usi++) lpTDoFtr->tbZer.flCoe[usi] = 0;
    if (usLen % 2) lpTDoFtr->tbZer.flCoe[(usLen-1) / 2] = (float) (2 * flFrq);

    /********************************************************************/
    /********************************************************************/
    flFrq = (float) (2.0 * dbPI * flFrq);       /* Convert to radians   */
    flDly = (float) ((usLen-1) / 2.0);

    /********************************************************************/
    /********************************************************************/
    for (usi = 0; usi < usLen/2; usi++) {
        lpTDoFtr->tbZer.flCoe[usi] = (float) (sin((double) (flFrq * ((float) usi - flDly)))
            / (dbPI * ((float) usi - flDly)) * EffFIRWin (usWin, usLen, usi));
        lpTDoFtr->tbZer.flCoe[usLen - usi - 1] = lpTDoFtr->tbZer.flCoe[usi];
    }

    /********************************************************************/
    /********************************************************************/
    lpTDoFtr->usTyp = EFFFIRXXX;
    lpTDoFtr->usSec = 1;
    lpTDoFtr->bfFst = bfFstFlg;    
    lpTDoFtr->tbZer.usLen = usLen;

    /********************************************************************/
    /* Adjust coefficients for long fraction math (if requested)        */
    /********************************************************************/
    if (lpTDoFtr->bfFst) for (usi = 0; usi < usLen; usi++) 
        lpTDoFtr->tbZer.lfCoe[usi] = (LNGFRA) (LNGFRAONE * lpTDoFtr->tbZer.flCoe[usi]);

    /********************************************************************/
    /********************************************************************/
    return (0);
}

/************************************************************************/
/* This function generates a single sample of a data window.            */
/* usWin = 1 (Rectangular), 2 (Tapered rectangular), 3 (Triangular),    */
/*         4 (Hanning), 5 (Hamming), or 6 (Blackman).                   */
/*         (Note:  tapered rectangular has cosine-tapered 10% ends.)    */
/* usSiz  = Size (total no. samples) of window.                         */
/* usPos  = Sample number within window, from 0 through n-1.            */
/*         (if k is outside this range, window is set to 0.)            */
/************************************************************************/
double  EffFIRWin (WINTYP usWin, WORD usSiz, WORD usPos)
{
    WORD    usTmp;

//    /********************************************************************/
//    /* Future: add one to compensate for FORTRAN port of EffFIRWin      */
//    /********************************************************************/
//    usSiz++;

    /********************************************************************/
    /********************************************************************/
    if ((usWin < 1 || usWin > 6) || (usPos < 0 || usPos >= usSiz)) return (0.0);

    switch (usWin) {
        /****************************************************************/
        /****************************************************************/
        case EFFWINREC:
            break;
        /****************************************************************/
        /****************************************************************/
        case EFFWINTAP:
            usTmp = (usSiz - 2) / 10;
            if (usPos <= usTmp) 
                return (0.5 * (1.0 - cos((double) usPos * dbPI 
                / ((double) usTmp + 1.0))));
            if (usPos > (usSiz - usTmp - 2)) 
                return (0.5 * (1.0 - cos((double) (usSiz - usPos - 1) * dbPI
                / ((double) usTmp + 1.0))));
            break;
        /****************************************************************/
        /****************************************************************/
        case EFFWINTRI:
            return (1.0 - fabs(1.0 - (double) (usPos * 2) / ((double) usSiz - 1.0)));
        /****************************************************************/
        /****************************************************************/
        case EFFWINHAN:
            return (0.5 * (1.0 - cos((double) (usPos * 2) * dbPI
                / ((double) usSiz - 1.0))));
        /****************************************************************/
        /****************************************************************/
        case EFFWINHAM:
            return (0.54 - 0.46 * cos((double) (usPos * 2) * dbPI
                / ((double) usSiz - 1.0)));
            break;
        /****************************************************************/
        /****************************************************************/
        case EFFWINBLK:
            return (0.42 - 0.5 * cos((double) (usPos * 2) * dbPI
                / ((double) usSiz - 1.0))
                + 0.08 * cos((double) (usPos * 4) * dbPI
                / ((double) usSiz - 1.0)));
    }

    /********************************************************************/
    /********************************************************************/
    return (1.0);
}

///************************************************************************/
///*                      FFT Windowing Functions                         */
///************************************************************************/
//#define SQR(a)        ((a)*(a))
//#define PARWIN(j,a,b) (float)(1.0-fabs(((j)-(a))*(b)))      /* Parzen   */
//#define TRIWIN(k, n) (float)(1.0-fabs(1.0-2*(k)/((n)-1.0))) /* Triangle */
//#define SQRWIN(j,a,b) (float) 1.0                           /* Square   */
//#define WELWIN(j,a,b) (float) (1.0-SQR(((j)-(a))*(b)))      /* Welch    */

/************************************************************************/
/* Window an "ideal" infinite duration frequency response               */
/************************************************************************/
WORD FAR PASCAL EffWinRsp (WINTYP usWinTyp, CPXFLT FAR *lpXfrBuf, DWORD ulXfrLen) 
{
/********************************************************************/
/* Note: There appears to be no advantage in expanding window first */
/********************************************************************/
float flWinLen =1;
//  #define     MINMULSIZ  2
//  #define     MAXMULSIZ  8
    #define     MINMULSIZ  1
    #define     MAXMULSIZ  1
    VISMEMHDL   mhInfHdl;
    CPXFLT FAR *lpInfBuf;
    DWORD       ulInfLen;
    DWORD       uli;

    /********************************************************************/
    /* Allocate "infinite" filter bufffer                               */
    /* If no space, simply leave original in place                      */
    /* Note limit size to 64K until all routines can handle large blks  */
    /********************************************************************/
    if ((VISMEMHDL) NULL == (mhInfHdl = GloAloBlk (GMEM_MOVEABLE, ulXfrLen * sizeof (*lpXfrBuf), 
      MINMULSIZ, min (0x8000L / (ulXfrLen * sizeof (*lpXfrBuf)), MAXMULSIZ), 
      &ulInfLen))) 
        return ((WORD) -1);
    if (NULL == (lpInfBuf = GloMemLck (mhInfHdl))) { 
        GloAloRel (mhInfHdl);
        return ((WORD) -1);
    }
    ulInfLen /= sizeof (*lpInfBuf);

if ((flWinLen <= 0) || (flWinLen > 1)) return ((WORD) -1);
    
    /********************************************************************/
    /* Round infinite FFT point count to power of two                   */
    /********************************************************************/
    ulInfLen = (DWORD) pow (2, (floor (LOGBASTWO (ulInfLen))));

    /********************************************************************/
    /* Duplicate filter and extend with zeros                           */
    /********************************************************************/
    _fmemmove (lpInfBuf, lpXfrBuf, (WORD) (ulXfrLen * sizeof (*lpXfrBuf)));
    for (uli=ulXfrLen; uli < ulInfLen; uli++) 
        lpInfBuf[uli].flRea = lpInfBuf[uli].flImg = 0;       
    
    /********************************************************************/
    /* Transform to time domain & window.                               */
    /* Truncate series and return to frequency domain                   */ 
    /* Note: Use asymetrical Hanning window (tail half of series)       */
    /********************************************************************/
    EffFFTRea ((float FAR *) lpInfBuf, (WORD) ulInfLen, EFFFFTINV);
ulXfrLen = (DWORD) (ulXfrLen * flWinLen);
    for (uli=0; uli < ulXfrLen; uli++) { 
        float   flHanWin = (float) EffFIRWin (usWinTyp, 
                (WORD) ((ulXfrLen * 2) + 1), (WORD) (ulXfrLen + uli));
        lpInfBuf[uli].flRea *= flHanWin;
        lpInfBuf[uli].flImg *= flHanWin;
    }
    for (uli=ulXfrLen; uli < ulInfLen; uli++)
        lpInfBuf[uli].flRea = lpInfBuf[uli].flImg = 0;       
ulXfrLen = (DWORD) (ulXfrLen / flWinLen);
    EffFFTRea ((float FAR *) lpInfBuf, (WORD) ulInfLen, EFFFFTFWD);

    /********************************************************************/
    /* Copy back to source buffer                                       */
    /********************************************************************/
    _fmemmove (lpXfrBuf, lpInfBuf, (WORD) (ulXfrLen * sizeof (*lpXfrBuf)));

    /********************************************************************/
    /********************************************************************/
    GloUnLRel (mhInfHdl);
    return (0);

}

DWORD   FAR PASCAL  EffWinGen (WINTYP usWinTyp, float FAR *lpWinBuf, DWORD ulWinLen)
{           
    WORD    usi;

    /********************************************************************/
    /********************************************************************/
    if (!ulWinLen) return (ulWinLen);

    /********************************************************************/
    /* Generate a buffer full of window data                            */
    /* Note: add one to compensate for FORTRAN port of EffFIRWin        */
    /********************************************************************/
    for (usi=0; usi < (WORD) ulWinLen; usi++) {
        lpWinBuf[usi] = (float) EffFIRWin (usWinTyp, (WORD) ulWinLen + 1, usi);
    }

    /********************************************************************/
    /********************************************************************/
    return (ulWinLen);
}

