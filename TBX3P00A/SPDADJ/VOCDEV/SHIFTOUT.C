#include "pv.h"

/*
 * if output time n >= 0, output first I samples in
 * array A of length N, then shift A left by I samples,
 * padding with zeros after last sample
 */
void shiftout( float A[], int N, int I, int n ) {
 int i ;
 short sample ;
    if ( n >= 0 )
	for ( i = 0 ; i < I ; i ++ ) {
	    sample = A[i]*32767. ;
	    fwrite( &sample, sizeof(short), 1, stdout ) ;
	}
    for ( i = 0 ; i < N - I ; i++ )
	A[i] = A[i+I] ;
    for ( i = N - I ; i < N ; i++ )
	A[i] = 0. ;
}
