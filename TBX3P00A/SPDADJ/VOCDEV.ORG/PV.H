// AJM
// #define INT long
// #define float double
#define INT int

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define FORWARD 1
#define INVERSE 0

typedef struct { float re ; float im ; } complex ;

#define CABS(x) sqrt( (x).re*(x).re + (x).im*(x).im )

extern complex zero ;
extern complex one ;
extern float synt ;
extern float PI ;
extern float TWOPI ;
/*
 * memory allocation macro
 */
#define fvec( name, size )\
if ( ( name = (float *) calloc( size, sizeof(float) ) ) == NULL) {\
    fprintf( stderr, "\nInsufficient memory\n" ) ;\
    exit( -1 ) ;\
}

complex cadd( complex x, complex y ) ;
complex csub( complex x, complex y ) ;
complex cmult( complex x, complex y ) ;
complex scmult( float s, complex x ) ;
complex cdiv( complex x, complex y ) ;
complex conjg( complex x ) ;
complex csqrt( complex x ) ;
void findpoles( float coef[], complex pole[], int M ) ;
void convert( float S[], float C[], int N2, INT D, INT R ) ;
void findroots( complex a[], complex r[], int M ) ;
complex laguerre( complex a[], INT M, complex x, float eps, INT P ) ;
float lpa( float x[], INT N, float b[], int M ) ;
float lpamp( float omega, float a0, float *coef, INT M ) ;
void
makewindows( float H[], float A[], float S[], INT Nw, INT N, INT I, INT osc ) ;
void 
oscbank( float C[], int N, float lpcoef[], INT npoles, INT R, INT Nw, 
INT I, float P, float O[] ) ;
INT readinput( float array[], INT size ) ;
void rfft( float x[], INT N, INT forward ) ;
int shiftin( float A[], int N, int D ) ;
void shiftout( float A[], int N, int I, int n ) ;
void unconvert( float C[], float S[], int N2, INT I, INT R ) ;
void fold( float I[], float W[], INT Nw, float O[], INT N, INT n ) ;
void overlapadd( float I[], INT N, float W[], float O[], INT Nw, INT n ) ;
void cfft( float x[], INT NC, INT forward ) ;

