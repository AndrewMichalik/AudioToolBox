#include "pv.h"

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
//AJM
static int NP, L = 4096, first = 1 ;
// static int NP, L = 8192, first = 1 ;
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
     float TWOPIoL = TWOPI/L, tabscale ;
	first = 0 ;
	fvec( lastamp, N+1 ) ;
	fvec( lastfreq, N+1 ) ;
	fvec( index, N+1 ) ;
	fvec( table, L ) ;
	tabscale = npoles ? 2./Nw : ( Nw >= N ? N : 8*N ) ;
	for ( n = 0 ; n < L ; n++ )
	    table[n] = tabscale*cos( TWOPIoL*n ) ;
	Iinv = 1./I ;
	Pinc = P*L/R ;
	ffac = P*PI/N ;
	if ( P > 1. )
	    NP = N/P ;
	else
	    NP = N ;
    }
/*
 * for each channel, compute I samples using linear
 * interpolation on the amplitude and frequency
 * control values
 */
    for ( chan = npoles ? P : 0 ; chan < NP ; chan++ ) {
     register float a, ainc, f, finc, address ;
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
	    if ( f == 0. )
		C[amp] = 0. ;
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
