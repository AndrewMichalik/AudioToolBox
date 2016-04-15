/*
 *	External DSP memory is allocated as follows:
 *	           "Normal" Partition		"XY Partition"
 *	x:$2000|=======================||========================|y:$a000
 *	       |                       ||   FFT imaginary part   |
 *	       |                       ||       FFT_SIZE         |
 *	       |-----------------------||------------------------|
 *	       |                       || Sin table 1/2 FFT_SIZE |
 *	       |-----------------------||------------------------|
 *	       |    Filter window      ||                        |
 *	       |      FFT_SIZE         ||                        |
 *	       |-----------------------||------------------------|
 *	       |                       ||                        |
 *	x:$3000|-----------------------||========================|x:$a000
 *	       |                       ||     FFT real part      |
 *	       |                       ||       FFT_SIZE         |
 *	       |-----------------------||------------------------|
 *	       |                       || Cos table 1/2 FFT_SIZE |
 *	       |-----------------------||------------------------|
 *	       |   Unshuffle buffer    ||                        |
 *	       |      FFT_SIZE         ||                        |
 *	       |-----------------------||------------------------|
 *	       |                       ||                        |
 *	       |=======================||========================|
 *
 *	We must rely on the fact that the "XY Partition" is imaged
 *	in the "Normal" (PMEM) partition because currently all AP macros
 *	except the FFT reference normal memory only.
 *
 *	Note that the sin and cos tables and the filter are in the
 *	full 24-bit range, and the sound file is in 16-bit range
 *	(this leaves some headroom on the DSP).
 */
#include <stdio.h>
#include <stdlib.h>
/*JMS #include <dsp/arrayproc.h> */

/* Constants */
#define	COEF_SIZE	(FFT_SIZE/2)

#define	FFT_DATA	DSPAPGetLowestAddressXY()
#define	FFT_COEF	(FFT_DATA+FFT_SIZE)

/* Defines for AP macros to access XY partition image. */
#define	FFT_IMAG	DSPMapPMemY(FFT_DATA)
#define	FFT_REAL	DSPMapPMemX(FFT_DATA)
#define	SIN_TABLE	DSPMapPMemY(FFT_COEF)
#define	COS_TABLE	DSPMapPMemX(FFT_COEF)
#define	UNSHUFFLE_BUF	DSPMapPMemX(FFT_COEF+COEF_SIZE)

/* Scaling constants */
#define	FFT_SCALE	DSP_FLOAT_TO_INT(1.0/FFT_SIZE)
/*
 * If forward is true, rfft replaces 2*N real data points in x with
 * N complex values representing the positive frequency half of their
 * Fourier spectrum, with x[1] replaced with the real part of the Nyquist
 * frequency value.  If forward is false, rfft expects x to contain a
 * positive frequency spectrum arranged as before, and replaces it with
 * 2*N real values.  N MUST be a power of 2.
 */
void rfft( float x[], int N, int forward ) {
 static int first = 1 ;
 int FFT_SIZE = N*2 ;
 float *sinTab ;
 float *cosTab ;
 float *floatBuf = (float *) malloc( (FFT_SIZE + 2)*sizeof(float) ) ;
 float norm ;
 float normalize( float [], int ) ;
 void unnormalize( float [], int, float ) ;
 void combine( float [], float [], float [], int ) ;
 void separate( float [], float [], float [], int ) ;
    if ( first ) {
	first = 0 ;
/* Enable DSP error logging to stderr and init the DSP */
	DSPSetErrorFP( stderr ) ;
	DSPEnableErrorLog() ;
	DSPAPInit() ;
	sinTab = DSPAPSinTable( FFT_SIZE ) ;
	cosTab = DSPAPCosTable( FFT_SIZE ) ;
/* Download FFT coefficient (sin and cos) tables */
	DSPAPWriteFloatArray( sinTab, SIN_TABLE, 1, COEF_SIZE ) ;
	DSPAPWriteFloatArray( cosTab, COS_TABLE, 1, COEF_SIZE ) ;
    }
    if ( forward ) {
        norm = normalize( x, FFT_SIZE ) ;
/* Download signal data */
        DSPAPWriteFloatArray( x, FFT_REAL, 1, FFT_SIZE ) ;
/* Clear imaginary input */
        DSPAPvclear( FFT_IMAG, 1, FFT_SIZE ) ; 
/* Do FFT */
        DSPAPfftr2a( FFT_SIZE, FFT_DATA, FFT_COEF ) ;
/* Unshuffle and return positive half spectrum */
        DSPAPvmovebr( FFT_REAL, FFT_SIZE/2, UNSHUFFLE_BUF, 1, FFT_SIZE ) ;
        DSPAPReadFloatArray( floatBuf, UNSHUFFLE_BUF, 1, FFT_SIZE/2 + 1 ) ;
        DSPAPvmovebr( FFT_IMAG, FFT_SIZE/2, UNSHUFFLE_BUF, 1, FFT_SIZE ) ;
        DSPAPReadFloatArray( floatBuf+N+1, UNSHUFFLE_BUF, 1, N+1 ) ;
/* Rearrange output into rfft format and scale */
        combine( floatBuf, floatBuf+N+1, x, N ) ;
        unnormalize( x, FFT_SIZE, norm/FFT_SIZE ) ;
    } else {
/* Rearrange rfft-format into separate real and imaginary arrays */
        separate( x, floatBuf, floatBuf+N+1, N ) ;
/* Download positive frequency real part of spectrum data */
        DSPAPWriteFloatArray( floatBuf, FFT_REAL, 1, N+1 ) ;
/* Download positive frequency imaginary part of spectrum data */
        DSPAPWriteFloatArray( floatBuf+N+1, FFT_IMAG, 1, N+1 ) ;
/* Complete real and imaginary data with reverse copies of positive frequency parts */
        DSPAPvreverse( FFT_REAL+1, FFT_REAL+N+1, N-1 ) ;
	DSPAPvreverse( FFT_IMAG+1, FFT_IMAG+N+1, N-1 ) ;
/* Make imaginary part antisymmetrical and conjugate */
	DSPAPvtsi(FFT_IMAG, 1, DSP_FLOAT_TO_INT(-1.), FFT_IMAG, 1, N);
/* Do inverse FFT */
        DSPAPfftr2a(FFT_SIZE, FFT_DATA, FFT_COEF);  
/* Unshuffle real part */
        DSPAPvmovebr(FFT_REAL, FFT_SIZE/2, UNSHUFFLE_BUF, 1, FFT_SIZE);
/* Upload signal data */
        DSPAPReadFloatArray( x, UNSHUFFLE_BUF, 1, FFT_SIZE ) ;
    }
/* Release the DSP */
/*
    DSPAPFree() ;
*/
    free( floatBuf ) ;
}
void combine( float real[], float imag[], float complex[], int N ) {
 int i ;
    for ( i = 0 ; i < N ; i++ ) {
        complex[i<<1] = real[i] ;
	complex[(i<<1)+1] = -imag[i] ;
    }
    complex[1] = real[N] ;
}
void separate( float complex[], float real[], float imag[], int N ) {
 int i ;
    for ( i = 0 ; i < N ; i++ ) {
        real[i] = complex[i<<1] ;
	imag[i] = -complex[(i<<1)+1] ;
    }
    real[N] = complex[1] ;
    imag[0] = imag[N] = 0. ;
}
float normalize( float a[], int N ) {
 float max = a[0], absval ;
 int i ;
    if ( max < 0. )
	max = -max ;
    for ( i = 1 ;  i < N ; i++ ) {
	absval = ( a[i] >= 0. ? a[i] : -a[i] ) ;
	if ( absval > max )
	    max = absval ;
    }
    if ( max == 0. )
        return( 1. ) ;
    max *= 2. ;
    max = 1./max ;
    max /= N ;
    for ( i = 0 ; i < N ; i++ )
        a[i] *= max ;
    return( 1./max ) ;
}
void unnormalize( float a[], int N, float factor ) {
 float *fend = a + N ;
    while ( a < fend )
	*a++ *= factor ;
}

