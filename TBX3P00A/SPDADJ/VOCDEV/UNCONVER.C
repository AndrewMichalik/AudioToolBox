#include "pv.h"

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
	fundamental = (float) R/(N2<<1) ;
        factor = TWOPI*I/R ;
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
	S[real] = mag*cos( phase ) ;
	if ( i != N2 )
	    S[imag] = -mag*sin( phase ) ;
    }
}
