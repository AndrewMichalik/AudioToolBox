#include "pv.h"
#include <io.h>
/*JMS #include <dsp/arrayproc.h> */

complex zero = { 0., 0. } ;
complex one = { 1., 0. } ;
float PI ;
float TWOPI ;
float synt ;

void
main( int argc, char *argv[] ) {
 int R, N, N2, Nw, Nw2, D, I, in, on, eof = 0, obank = 0, Np ;
 float P, *Hwin, *Wanal, *Wsyn, *input,
 *winput, *lpcoef, *buffer, *channel, *output ;
 void vvmult( float *out, float *a, float *b, int n ) ;
 float maxof( float *a, int n ) ;

// AJM 06/10/95
// Reassign stdin to input file.
FILE *fTmpInp;
int hTmpInp;
// Reassign stdout to output file.
FILE *fTmpOut;
int hTmpOut;


    if ( argc < 9 ) {
	fprintf( stderr, "%s%s%s%s%s%s%s%s%s",
"USAGE: pvdsp  R N Nw D I P Np synt < shortsams  > shortsams\n",
"	R    - sampling rate in Hz\n",
"	N    - FFT length in samples (must be 2^n <= 1024)\n",
"	Nw   - window size in samples\n",
"	D    - decimation factor in samples [0 for synthesis only]\n",
"	I    - interpolation factor in samples [0 for analysis only]\n",
"	P    - oscillator bank pitch factor [0 for overlap-add resynthesis]\n",
"	Np   - linear prediction order [0 for none]\n",
"	synt - synthesis channel threshold [0 for all channels]\n"
) ;
	exit( 0 ) ;
    }
/*
 * pick up arguments from command line
 */
    R     = atoi( argv[1] ) ;	/* sampling rate */
    N     = atoi( argv[2] ) ;	/* FFT length */
    Nw    = atoi( argv[3] ) ;	/* window size */
    D     = atoi( argv[4] ) ;	/* decimation factor */
    I     = atoi( argv[5] ) ;	/* interpolation factor */
    P     = atof( argv[6] ) ;	/* oscillator bank pitch factor */
    Np    = atoi( argv[7] ) ;	/* linear prediction order */
    synt  = atof( argv[8] ) ;	/* synthesis threshhold */

// AJM 06/10/95
// Reassign stdin to input file.
// Duplicate handle of stdin. Save the original to restore later.
hTmpInp = _dup( _fileno( stdin ) );
fTmpInp = freopen( "if.", "rb", stdin );
if( (fTmpInp == NULL) || (hTmpInp == -1 ) )
{
    _dup2( hTmpInp, _fileno( stdin ) );
    perror( "Can't reassign standard input" );
    exit( 1 );
}
// Reassign stdout to output file.
hTmpOut = _dup( _fileno( stdout ) );
fTmpOut = freopen( "of.", "wb", stdout );
if( (fTmpOut == NULL) || (hTmpOut == -1 ) )
{
    _dup2( hTmpOut, _fileno( stdout ) );
    perror( "Can't reassign standard Outut" );
    exit( 1 );
}

    freopen( "pv.out", "w", stderr ) ;
    fprintf( stderr, "pv parameters:\n" ) ;
    fprintf( stderr, "R = %d\n", R ) ;
    fprintf( stderr, "N = %d\n", N ) ;
    fprintf( stderr, "Nw = %d\n", Nw ) ;
    fprintf( stderr, "D = %d\n", D ) ;
    fprintf( stderr, "I = %d\n", I ) ;
    fprintf( stderr, "P = %g\n", P ) ;
    fprintf( stderr, "Np = %d\n", Np ) ;
    fprintf( stderr, "synt = %g\n", synt ) ;
    fflush( stderr ) ;

    PI = 4.*atan(1.) ;
    TWOPI = 8.*atan(1.) ;
    obank = P != 0. ;
    N2 = N>>1 ;
    Nw2 = Nw>>1 ;
/*
 * allocate memory
 */
    fvec( Wanal, Nw ) ;		/* analysis window */
    fvec( Wsyn, Nw ) ;		/* synthesis window */
    fvec( input, Nw ) ;		/* input buffer */
    fvec( Hwin, Nw ) ;		/* plain Hamming window */
    fvec( winput, Nw ) ;	/* windowed input buffer */
    fvec( lpcoef, Np+1 ) ;	/* lp coefficients */
    fvec( buffer, N ) ;		/* FFT buffer */
    fvec( channel, N+2 ) ;	/* analysis channels */
    fvec( output, Nw ) ;	/* output buffer */
/*
 * create windows
 */
    makewindows( Hwin, Wanal, Wsyn, Nw, N, I, obank ) ;
/*
 * initialize input and output time values (in samples)
 */
    in = -Nw ;
    if ( D )
	on = (in*I)/D ;
    else
	on = in ;
/*
 * main loop--perform phase vocoder analysis-resynthesis
 */
    while ( !eof ) {
/*
 * increment times
 */
	in += D ;
	on += I ;
/*
 * analysis: input D samples; window, fold and rotate input
 * samples into FFT buffer; take FFT; and convert to
 * amplitude-frequency (phase vocoder) form
 */
	if ( D == 0 ) {
	    if ( Np )
		if ( fread( lpcoef, sizeof(float), Np+1, stdin ) == 0 )
		    eof = 1 ;
	    if ( fread( channel, sizeof(float), N+2, stdin ) == 0 )
		eof = 1 ;
	} else {
	    eof = shiftin( input, Nw, D ) ;
	    fprintf( stderr, "%d=%.3fs (%.3g)/", in, (float)in/R, 
		maxof( &input[Nw-D], D ) ) ;

	    if ( Np ) {
		vvmult( winput, Hwin, input, Nw ) ;
		lpcoef[0] = lpa( winput, Nw, lpcoef, Np ) ;
		fprintf( stderr, "%.3g/", lpcoef[0] ) ;
	    }
	    fold( input, Wanal, Nw, buffer, N, in ) ;
	    rfft( buffer, N2, FORWARD ) ;
	    convert( buffer, channel, N2, D, R ) ;
	}
	if ( I == 0 ) {
	    if ( Np )
		fwrite( lpcoef, sizeof(float), Np+1, stdout ) ;
	    fwrite( channel, sizeof(float), N+2, stdout ) ;
	    fflush( stdout ) ;
	    fprintf( stderr, "\n" ) ;
	    fflush( stderr ) ;
	    continue ;
	}
/*
 * at this point channel[2*i] contains amplitude data and
 * channel[2*i+1] contains frequency data (in Hz) for phase
 * vocoder channels i = 0, 1, ... N/2; the center frequency
 * associated with each channel is i*f, where f is the
 * fundamental frequency of analysis R/N; any desired spectral
 * modifications can be made at this point: pitch modifications
 * are generally well suited to oscillator bank resynthesis,
 * while time modifications are generally well (and more
 * efficiently) suited to overlap-add resynthesis
 */
	if ( obank ) {
/*
 * oscillator bank resynthesis
 */
	    oscbank( channel, N2, lpcoef, Np, R, Nw, I, P, output ) ;
	    fprintf( stderr, "%d=%.3fs (%.3g)\n", on, (float)on/R,
		maxof( output, I ) ) ;
	    fflush( stderr ) ;
	    shiftout( output, Nw, I, on+Nw-I ) ;
	} else {
/*
 * overlap-add resynthesis
 */
	    unconvert( channel, buffer, N2, I, R ) ;
	    rfft( buffer, N2, INVERSE ) ;
	    overlapadd( buffer, N, Wsyn, output, Nw, on ) ;
	    fprintf( stderr, "%d=%.3fs (%.3g)\n", on, (float)on/R,
		maxof( output, I ) ) ;
	    fflush( stderr ) ;
	    shiftout( output, Nw, I, on ) ;
	}
    }

// AJM 06/10/95
// Reassign stdin back to original so that we can use the original stdin
_dup2( hTmpInp, _fileno( stdin ) );
// Reassign stdout back to original so that we can use the original stdout
_dup2( hTmpOut, _fileno( stdout ) );

/* Release the DSP */
   /*JMS DSPAPFree() ; */
    exit( 0 ) ;
}

void vvmult( float *out, float *a, float *b, int n ) {
 register float *lim = out + n ;
    while ( out < lim )
	*out++ = *a++ * *b++ ;
}

float maxof( float *a, int n ) {
 register float *lim = a + n, m ;
    for ( m = *a++ ; a < lim ; a++ )
	if ( *a > m )
	    m = *a ;
    return( m ) ;
}
