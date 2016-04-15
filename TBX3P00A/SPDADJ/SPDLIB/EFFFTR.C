/************************************************************************/
/* Filter Effects: EffFtr.c                             V2.00  08/15/95 */
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
#include "..\..\effdev\geneff.h"        /* Sound Effects definitions    */
#include "genspd.h"                     /* PCM conversion defs          */
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
void    CpxMul (CPXFLT, CPXFLT, CPXFLT FAR *);

//ajm
    #include "..\vocdev\pv.h"
    VOCBLK  vbVocBlk;                   /* Repeated call block          */
    float   pOscBnk[2048];
    float   P = 1;
    DWORD   ulDstFrq;
    DWORD   ulSrcFrq;
    float   pOscBnk[2048];
    float   *Hwin, *Wanal, *Wsyn, *input,
    *winput, *lpcoef, *buffer, *channel, *output ;
//ajm

/************************************************************************/
/************************************************************************/
WORD    FFTFtrAlo (SPDBLK FAR *lpSpdBlk, FFTBLK FAR *lpFFTBlk, 
        EFFPOLPRC lpPolPrc, DWORD ulPolDat)
{
    float       flResRat;
    float  FAR *lpWinBuf;
    LPCPXF      lpXfrBuf;

    /********************************************************************/
    /********************************************************************/
    _fmemset (lpFFTBlk, 0, sizeof (*lpFFTBlk));

    /********************************************************************/
    /* Initialize FFT block                                             */
    /********************************************************************/
    flResRat = lpSpdBlk->ulDstFrq / (float) lpSpdBlk->ulSrcFrq;
ulDstFrq = lpSpdBlk->ulDstFrq;
ulSrcFrq = lpSpdBlk->ulSrcFrq;
flResRat = (float) 1;
    if (EffFFTAlo (lpSpdBlk->usFFTOrd, flResRat, lpFFTBlk, lpPolPrc, ulPolDat)) {
        SPDERRMSG ("Error setting FFT filter parameters.\n");
        return ((WORD) -1);
    }

    /********************************************************************/
    /* Initialize / allocate vocoder function block                     */
    /********************************************************************/
    if (EffVocAlo (lpSpdBlk->bfFrcOBS ? MSTVocOBS : NULL, &vbVocBlk, NULL, 
      lpSpdBlk->usFFTOrd, lpSpdBlk->usWinOrd, lpSpdBlk->flSpdMul, 
      lpSpdBlk->flPchMul, lpSpdBlk->flSynThr, lpSpdBlk->ulSrcFrq)) {
        if (TBxGlo.usDebFlg & ERR___DEB) 
            MsgDspUsr ("Error allocating vocoder memory.\n");
EffFFTRel (lpFFTBlk);
        return (0);
    }

    /********************************************************************/
    /* Lock data window memory, initialize								*/
    /* Note: Data window is twice the FFT point size - overlap saved	*/
    /********************************************************************/
    if (NULL != (lpWinBuf = (float FAR *) GloMemLck (lpFFTBlk->mhWinHdl))) {
        EffWinGen (EFFWINTAP, lpWinBuf, 2 * lpFFTBlk->ulFFTPts);
        GloMemUnL (lpFFTBlk->mhWinHdl);
    }

    /********************************************************************/
    /* Convert infinite length transfer functions into valid frequency  */
    /* domain filter. Lock FFT filter transfer buffer & window.         */
    /********************************************************************/
    if (NULL != (lpXfrBuf = GloMemLck (lpFFTBlk->mhXfrHdl))) {
        EffWinRsp (EFFWINHAM, lpXfrBuf, lpFFTBlk->ulFFTPts); 
        GloMemUnL (lpFFTBlk->mhXfrHdl);
    }

fvec( Hwin, (INT) lpFFTBlk->ulFFTPts ) ;		/* plain Hamming window */
fvec( Wanal,(INT) vbVocBlk.ulWinPts ) ;		    /* analysis window */
fvec( Wsyn, (INT) vbVocBlk.ulWinPts ) ;		    /* synthesis window */
makewindows (Hwin, Wanal, Wsyn, (INT) vbVocBlk.ulWinPts, 
    (INT) lpFFTBlk->ulFFTPts, (INT) ulSrcFrq, 0);

    /********************************************************************/
    /********************************************************************/
    return (0);

}

WORD    FFTFtrRel (FFTBLK FAR *lpFFTBlk)
{
    /********************************************************************/
    /********************************************************************/
    EffVocRel (&vbVocBlk);
    EffFFTRel (lpFFTBlk);
    return (0);
}

/////////////////////////////////////////////////////////////////////////////
//	Note: MSC 8.0 bug when compiling with Compiler Optimizations = Max Speed
//	Equalizer generates zeros for output data. Is this related
//	to ToolBox 2.00d EffFtr.c MSC 8.0 -Ocglt bug?
/////////////////////////////////////////////////////////////////////////////
#pragma optimize("", off)
/************************************************************************/
/*                  Frequency Domain Filter Routine                     */
/************************************************************************/
WORD FAR PASCAL FFTFtrFnc (float FAR * FAR *ppflSrcBuf, DWORD FAR *lpulDstCnt,
                DWORD ulBufPts, DWORD ulTotPts, DWORD ulHdrPts, DWORD ulCtrPts, 
                DWORD ulDstRem, LPFFTB lpFFTBlk)
{
    SPDBLK  FAR *lpSpdBlk = lpFFTBlk->lpUsrDat;   
    CPXFLT  FAR *lpFFTCpx = (CPXFLT FAR *)(*ppflSrcBuf);
    float   FAR *lpWinBuf;
//    CPXFLT  FAR *lpXfrBuf;
    DWORD       uli;

static DWORD ulTim = 0;

    /********************************************************************/
    /* The EffFFTFtr (caller) delivers a set of data points where       */
    /* ulCtrPts at offset ulHdrPts have been isolated to implement      */
    /* the overlap-save method of FFT corruption prevention.            */
    /********************************************************************/
    
    /********************************************************************/
    /* Default returned data count to zero                              */
    /********************************************************************/
    *lpulDstCnt = 0L;

    /********************************************************************/
    /* Completion pass? Indicate done processing...                     */
    /********************************************************************/
    if (!ulCtrPts) return (FALSE);

    /********************************************************************/
    /* Space available in output? Enough room for resampling?           */
    /********************************************************************/
    if (ulDstRem < ulCtrPts * lpFFTBlk->flResRat) return (FALSE);
    if (ulBufPts < ulTotPts * lpFFTBlk->flResRat) return (FALSE);

//    /********************************************************************/
//    /* Window incoming data                                             */
//    /********************************************************************/
//    if (NULL != (lpWinBuf = (float FAR *) GloMemLck (lpFFTBlk->mhWinHdl))) {
//      for (uli=0; uli < ulTotPts; uli++) (*ppflSrcBuf)[uli] *= lpWinBuf[uli];
//      GloMemUnL (lpFFTBlk->mhWinHdl);
//    }

fold (*ppflSrcBuf, Wanal, (INT) vbVocBlk.ulWinPts, pOscBnk, 
    (INT) lpFFTBlk->ulFFTPts, (INT) ulTim) ;
_fmemmove (*ppflSrcBuf, pOscBnk, sizeof (*pOscBnk) * (INT) ulTotPts);
ulTim += ulCtrPts;

    /********************************************************************/
    /* Transform to frequency domain                                    */
    /********************************************************************/
    EffFFTRea (*ppflSrcBuf, (WORD) (ulTotPts / 2L), EFFFFTFWD);

convert (*ppflSrcBuf, pOscBnk, (INT) (ulTotPts / 2L), 
    (INT) ulDstFrq, (INT) ulSrcFrq);

//*ppflSrcBuf = &(*ppflSrcBuf)[(DWORD) (ulHdrPts * lpFFTBlk->flResRat)];
//convert (*ppflSrcBuf, pOscBnk, (int) (ulCtrPts / 2L), (INT) 1, (INT) ulSrcFrq);

_fmemset (*ppflSrcBuf, 0, sizeof (**ppflSrcBuf) * (WORD) ulTotPts);
oscbank (pOscBnk, (int) ulTotPts / 2, NULL, 0, (INT) ulSrcFrq, (INT) ulTotPts, 
        (INT) (ulTotPts * lpFFTBlk->flResRat), P, *ppflSrcBuf);

//unconvert (pOscBnk, *ppflSrcBuf, (int) (ulTotPts / 2L), (INT) 1, (INT) ulSrcFrq);

//    /********************************************************************/
//    /* Lock FFT filter transfer buffer                                  */
//    /********************************************************************/
//    if (NULL == (lpXfrBuf = (CPXFLT FAR *) GloMemLck (lpFFTBlk->mhXfrHdl))) 
//        return (FALSE);

//    /********************************************************************/
//    /* Handle DC & Nyquist as special case; pack into real data FFT fmt */
//    /********************************************************************/
//    lpFFTCpx[0].flRea *= lpXfrBuf[0].flRea;
//    lpFFTCpx[0].flImg *= lpXfrBuf[0].flImg;

//    /********************************************************************/
//    /* Apply filter function, unlock filter transfer buffer when done   */
//    /********************************************************************/
//    for (uli = 1L; uli < ulTotPts / 2L; uli++) {
//        CpxMul (lpFFTCpx[uli], lpXfrBuf[uli], &lpFFTCpx[uli]);
//    }
//    GloMemUnL (lpFFTBlk->mhXfrHdl);

    /********************************************************************/
    /* Pad with zero if required; transform back to time domain         */
    /********************************************************************/
    for (uli=ulTotPts; uli < ulBufPts; uli++) (*ppflSrcBuf)[uli] = (float) 0;
//    EffFFTRea (*ppflSrcBuf, (WORD) ((ulTotPts * lpFFTBlk->flResRat) / 2), EFFFFTINV);
    //
    //  Note: MSC 8.0 bug when compiling DLL with -Ocglt: 
    //  #pragma optimize("", off) has no effect
    //  0 == (WORD) ((ulTotPts * lpFFTBlk->flResRat) / 2)
    //  0 == (WORD) ((lpFFTBlk->flResRat * ulTotPts) / 2)
    //  0 == (WORD) (ulTotPts * (lpFFTBlk->flResRat / 2.))
    //  0 == (WORD) ((double) ulTotPts * ((double) lpFFTBlk->flResRat / 2.))
    //  WORD usTmp = (WORD) ((lpFFTBlk->flResRat * ulTotPts) / 2); // == 0
    //  Note: Appears to only effect the DLL version, not static lib compile
    //

    /********************************************************************/
    /* Set Output buffer to point to good samples (past header)         */
    /********************************************************************/
    *ppflSrcBuf = &(*ppflSrcBuf)[(DWORD) (ulHdrPts * lpFFTBlk->flResRat)];

    /********************************************************************/
    /* Indicate number of valid output samples and continue processing  */
    /********************************************************************/
    *lpulDstCnt = (DWORD) (ulCtrPts * lpFFTBlk->flResRat);

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);

}

/////////////////////////////////////////////////////////////////////////////
//	End MSC 8.0 bug
/////////////////////////////////////////////////////////////////////////////
#pragma optimize("", on)

void CpxMul (CPXFLT a, CPXFLT b, CPXFLT FAR *pc)
{
    pc->flRea = a.flRea*b.flRea - a.flImg*b.flImg;
    pc->flImg = a.flImg*b.flRea + a.flRea*b.flImg;
}




/************************************************************************/
/************************************************************************/
#include <math.h>                       /* Math library defs            */
float synt = (float) 0.1;         // synthesis channel threshold
float PI ;
float TWOPI ;

/*
 * S is a spectrum in rfft format, i.e., it contains N real values
 * arranged as real followed by imaginary values, except for first
 * two values, which are real parts of 0 and Nyquist frequencies;
 * convert first changes these into N/2+1 PAIRS of magnitude and
 * phase values to be stored in output array C; the phases are then
 * unwrapped and successive phase differences are used to compute
 * estimates of the instantaneous frequencies for each phase vocoder
 * analysis channel; decimation rate D and sampling rate R are used
 * to render these frequency values directly in Hz.
 */
void convert( float FAR *S, float FAR *C, int N2, INT D, INT R ) {
 static INT first = 1 ;
 static float *lastphase, fundamental, factor ;
 float phase, phasediff ;
 INT real, imag, amp, freq ;
 float a, b ;
 INT i ;
/*
 * FIRST TIME ONLY: allocate zeroed space for previous phase
 * values for each channel and compute constants
 */
    if ( first ) {
	first = 0 ;
	fvec( lastphase, N2+1 ) ;
	fundamental = (float) R/(N2<<1) ;
        factor = R/(D*(float) db2PI) ;
    } 
//ajm
//_fmemset (lastphase, 0, sizeof (*lastphase) * (N2+1));

/*
 * unravel rfft-format spectrum: note that N2+1 pairs of
 * values are produced
 */
    for ( i = 0 ; i <= N2 ; i++ ) {
	imag = freq = ( real = amp = i<<1 ) + 1 ;
	a = ( i == N2 ? S[1] : S[real] ) ;
	b = ( i == 0 || i == N2 ? (float) 0. : S[imag] ) ;
/*
 * compute magnitude value from real and imaginary parts
 */
	C[amp] = (float) _hypot( a, b ) ;
/*
 * compute phase value from real and imaginary parts and take
 * difference between this and previous value for each channel
 */
	if ( C[amp] == 0. )
	    phasediff = (float) 0. ;
	else {
	    phasediff = ( phase = (float) -atan2( b, a ) ) - lastphase[i] ;
	    lastphase[i] = phase ;
/*
 * unwrap phase differences
 */
	    while ( phasediff > (float) dbPI )
		phasediff -= (float) db2PI ;
	    while ( phasediff < -(float) dbPI )
		phasediff += (float) db2PI ;
	}
/*
 * convert each phase difference to Hz
 */
	C[freq] = phasediff*factor + i*fundamental ;
    }
}

/*
 * oscillator bank resynthesizer for phase vocoder analyzer
 * uses sum of N+1 cosinusoidal table lookup oscillators to 
 * compute I (interpolation factor) samples of output O
 * from N+1 amplitude and frequency value-pairs in C;
 * frequencies are scaled by P
 */
void 
oscbank( float C[], int N, float lpcoef[], INT npoles, INT R, INT Nw, 
INT I, float P, float O[] ) {
static 	float Iniindex;
    static int NP, L = 4096, first = 1 ;
    static float Iinv, *lastamp, *lastfreq, *index, *table ;
    static float Pinc, ffac ;
    INT amp, freq, n, chan ;
/*
 * FIRST TIME ONLY: allocate memory to hold previous values
 * of amplitude and frequency for each channel, the table
 * index for each oscillator, and the table itself; also
 * compute constants
 */

    if ( first ) {
    float db2PIoL = (float) db2PI/L, tabscale ;
	first = 0 ;
	fvec( lastamp, N+1 ) ;
	fvec( lastfreq, N+1 ) ;
	fvec( index, N+1 ) ;
	fvec( table, L ) ;
	tabscale = npoles ? (float) 2./Nw : ( Nw >= N ? N : 8*N ) ;

tabscale = (float) 1;
//for ( n = 0 ; n < N+1 ; n++ ) {
//lastamp [n] = C[2*n]; 
//lastfreq[n] = C[2*n+1] / 2; 
//}

	for ( n = 0 ; n < L ; n++ )
	    table[n] = (float) ( tabscale*cos( db2PIoL*n ) ) ;
	Iinv = (float) 1./I ;
	Pinc = P*L/(float)R ;
	ffac = P*(float) dbPI/N ;
	if ( P > (float) 1. )
	    NP = (int) (N/P) ;
	else
	    NP = N ;
    }


/*
 * for each channel, compute I samples using linear
 * interpolation on the amplitude and frequency
 * control values
 */
    for ( chan = (INT) (npoles ? P : 0 ) ; chan < NP ; chan++ ) {
    float a, ainc, f, finc, address ;
	freq = ( amp = ( chan << 1 ) ) + 1 ;
	if ( C[amp] < synt ) /* skip the little ones */
	    continue ;
	C[freq] *= Pinc ;
	finc = ( C[freq] - ( f = lastfreq[chan] ) )*Iinv ;
/*
 * if linear prediction specified, REPLACE phase vocoder amplitude
 * measurements with linear prediction estimates
 */
	if ( npoles ) {
	    if ( f == (float) 0. )
		C[amp] = (float) 0. ;
	    else
		C[amp] = lpamp( chan*ffac, lpcoef[0], lpcoef, npoles ) ;
	}
	ainc = ( C[amp] - ( a = lastamp[chan] ) )*Iinv ;
	address = index[chan] ;
/*
 * accumulate the I samples from each oscillator into
 * output array O (initially assumed to be zero);
 * f is frequency in Hz scaled by oscillator increment
 * factor and pitch (Pinc); a is amplitude;
 */
	for ( n = 0 ; n < I ; n++ ) {
        O[n] += a*table[ (INT) address ] ;
//if (n == I/2) {
//lastfreq[chan] = f ;
//lastamp[chan]  = a ;
//index[chan]    = address ;
//}
	    address += f ;
	    while ( address >= L )
		address -= L ;
	    while ( address < 0 )
		address += L ;
	    a += ainc ;
	    f += finc ;
	} 

    /*
     * save current values for next iteration
     */
	lastfreq[chan] = C[freq] ;
	lastamp[chan] = C[amp] ;
	index[chan] = address ;
    }

}

/*
 * unconvert essentially undoes what convert does, i.e., it
 * turns N2+1 PAIRS of amplitude and frequency values in
 * C into N2 PAIR of complex spectrum data (in rfft format)
 * in output array S; sampling rate R and interpolation factor
 * I are used to recompute phase values from frequencies
 */
void unconvert( float C[], float S[], int N2, INT I, INT R ) {
 static INT first = 1 ;
 static float *lastphase, fundamental, factor ;
 INT i, real, imag, amp, freq ;
 float mag, phase ;
/*
 * FIRST TIME ONLY: allocate memory and compute constants
 */
    if ( first ) {
	first = 0 ;
	fvec( lastphase, N2+1 ) ;
	fundamental = R/(float)(N2<<1) ;
        factor = (float) db2PI*I/(float) R ;
    } 
/*
 * subtract out frequencies associated with each channel,
 * compute phases in terms of radians per I samples, and
 * convert to complex form
 */
    for ( i = 0 ; i <= N2 ; i++ ) {
	imag = freq = ( real = amp = i<<1 ) + 1 ;
	if ( i == N2 )
	    real = 1 ;
	mag = C[amp] ;
	lastphase[i] += C[freq] - i*fundamental ;
	phase = lastphase[i]*factor ;
	S[real] = (float) ( mag*cos( phase ) ) ;
	if ( i != N2 )
	    S[imag] = (float) ( -mag*sin( phase ) ) ;
    }
}

/*
 * evaluate magnitude of transfer function at frequency omega
 */
float lpamp( float omega, float a0, float *coef, INT M ) {
 register float wpr, wpi, wr, wi, re, im, temp ;
 register INT i ;
    if ( a0 == 0. )
	return( (float) 0. ) ;
    wpr = (float) cos( omega ) ;
    wpi = (float) sin( omega ) ;
    re = wr = (float) 1. ;
    im = wi = (float) 0. ;
    for ( coef++, i = 1 ; i <= M ; i++, coef++ ) {
	wr = (temp = wr)*wpr - wi*wpi ;
	wi = wi*wpr + temp*wpi ;
	re += *coef*wr ;
	im += *coef*wi ;
    }
    if ( re == 0. && im == 0. )
	return( (float) 0. ) ;
    else
	return( (float) sqrt( a0/(re*re + im*im) ) ) ;
}

/*
 * make balanced pair of analysis (A) and synthesis (S) windows;
 * window lengths are Nw, FFT length is N, synthesis interpolation
 * factor is I, and osc is true (1) if oscillator bank resynthesis 
 * is specified
 */
void
makewindows( float H[], float A[], float S[], INT Nw, INT N, INT I, INT osc ) {
 INT i ;
 float sum ;
/*
 * basic Hamming windows
 */
    for ( i = 0 ; i < Nw ; i++ )
	H[i] = A[i] = S[i] = (float) (0.54 - 0.46*cos( TWOPI*i/(Nw - 1) ) ) ;
/*
 * when Nw > N, also apply interpolating (sinc) windows to
 * ensure that window are 0 at increments of N (the FFT length)
 * away from the center of the analysis window and of I away
 * from the center of the synthesis window
 */
    if ( Nw > N ) {
     float x ;
     float PI = (float) (4.*atan(1.) ) ;
/*
 * take care to create symmetrical windows
 */
	x = -(Nw - 1)/(float)2. ;
	for ( i = 0 ; i < Nw ; i++, x += (float) 1. )
	    if ( x != 0. ) {
		A[i] *= (float) (N*sin( PI*x/N )/(PI*x) ) ;
		if ( I )
		    S[i] *= (float) (I*sin( PI*x/I )/(PI*x) ) ;
	    }
    }
/*
 * normalize windows for unity gain across unmodified
 * analysis-synthesis procedure
 */
    for ( sum = i = 0 ; i < Nw ; i++ )
	sum += A[i] ;

    for ( i = 0 ; i < Nw ; i++ ) {
     float afac = (float) (2./sum) ;
     float sfac = Nw > N ? (float) (1./afac) : afac ;
	A[i] *= afac ;
	S[i] *= sfac ;
    }

    if ( Nw <= N && I ) {
	for ( sum = i = 0 ; i < Nw ; i += I )
	    sum += S[i]*S[i] ;
	for ( sum = (float) (1./sum), i = 0 ; i < Nw ; i++ )
	    S[i] *= sum ;
    }
}

/*
 * multiply current input I by window W (both of length Nw);
 * using modulus arithmetic, fold and rotate windowed input
 * into output array O of (FFT) length N according to current
 * input time n
 */
void fold(float input[], float Wanal[], int Nw, float buffer[], int N, int incount)
{
    int i;
    
    for ( i = 0; i < N; i++) buffer[i] = (float)0.0;
    while ( incount < 0 ) incount += N;
    incount %= N;
    for (i = 0; i < Nw; i++)
	{
		buffer[incount] += input[i] * Wanal[i];
		incount++;
		incount %= N;
    }
}
