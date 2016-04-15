#include <stdio.h>
#include <math.h>

#define LENGTH 16


main( int argc, char *argv[] ) {
 int N = LENGTH ;
 double pi2oN = 8.*atan(1.)/N ;
 float x1[LENGTH], x2[LENGTH], X[LENGTH], amp, freq ;
 int n, sincos ;
 void rfft( float [], int, int ) ;
    amp = atof( argv[1] ) ;
    freq = atof( argv[2] ) ;
    sincos = atoi( argv[3] ) ;
    if ( sincos )
	for ( n = 0 ; n < N ; n++ ) 
	    X[n] = x1[n] = amp*sin( pi2oN*freq*n ) ;
    else
	for ( n = 0 ; n < N ; n++ ) 
	    X[n] = x1[n] = amp*cos( pi2oN*freq*n ) ;
	/*
	x1[n] = sin( pi2oN*1*n ) + .33*sin( pi2oN*3*n ) + .2*sin( pi2oN*5*n ) ;
	*/
    rfft( X, N/2, 1 ) ;
    for ( n = 0 ; n < N ; n++ ) 
	x2[n] = X[n] ;
    rfft( x2, N/2, 0 ) ;
    for ( n = 0 ; n < N ; n++ ) {
	printf( "[%2d] %7.4f", n, x1[n] ) ;
	printf( "    " ) ;
	if ( n < N/2 )
	    printf( "( %7.4f, %7.4f)", X[2*n], X[2*n+1] ) ;
	else
	    printf( "                   " ) ;
	printf( "    " ) ;
	printf( "%7.4f\n", x2[n] ) ;
    }
}
