#include "pv.h"

#define dbPI        3.141592653589793   /* Pi                           */
#define db2PI       6.28318530717959    /* 2 * Pi                       */

/************************************************************************/
/************************************************************************/
#define EFFFFTFWD   +1                  /* FFT forward transform flag   */
#define EFFFFTINV    0                  /* FFT reverse transform flag   */

/************************************************************************/
/* FFT Processing Support: FFTSup.c                     V2.00  03/15/95 */
/* Copyright (c) 1987-1995 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
//#include "..\os_dev\winmem.h"           /* Generic memory supp defs     */
//#include "geneff.h"                     /* Sound Effects definitions    */
//#include "effsup.h"                     /* Sound Effects definitions    */

#include <math.h>                       /* Floating point math funcs    */
#include <stdio.h>                      /* Standard I/O                 */
#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <malloc.h>                     /* Memory library defs          */

/************************************************************************/
/************************************************************************/
void    FFTCpx  (float FAR *data, int nn, int isign);

/************************************************************************/
/************************************************************************/
typedef struct _CPXFLT {                /* Complex float                */
        float flRea;
        float flImg;
} CPXFLT;   

/************************************************************************/
/*                  Constant and Macro definitions                      */
/************************************************************************/
#define PARWIN(j,a,b) (float)(1.0-fabs(((j)-(a))*(b)))      /* Parzen   */
#define TRIWIN(k, n) (float)(1.0-fabs(1.0-2*(k)/((n)-1.0))) /* Triangle */
#define SQRWIN(j,a,b) (float) 1.0                           /* Square   */
#define WELWIN(j,a,b) (float) (1.0-SQR(((j)-(a))*(b)))      /* Welch    */

#define SWAP(a,b)     tempr=(a);(a)=(b);(b)=tempr
//#define SQR(a)        (sqrarg=(a),sqrarg*sqrarg)

/************************************************************************/
/* Calculates the Fourier transform of a set of 2n real-valued data     */
/* data points. Replaces the data (stored in array data[0..2n-1]) by    */
/* the positive frequency half of its complex Fourier transform.        */
/* data[] is an array of length 2*n, used as a real array of            */
///* data[0...2*n-1], returned as real [0-n] and imag [(n+1) - (2n-1)].   */
  /* data[0...2*n-1], returned as real [even] and imag [odd].           */
/* The real-valued first and last components of the complex transform   */
/* are returned as data[0] and data[n] respectively. n must be a        */
/* power of 2. This routine also calculate the inverse transform of a   */
/* complex data array if it is the transform of real data. (Result in   */
/* this this case must be multiplied by 1/n).                           */
/************************************************************************/
void rfft (float *data, INT n, INT isign)
{

    int i, ni;
    float c1 = (float) 0.5, c2, h1r, h1i, h2r, h2i;
    double wr,wi,wpr,wpi,wtemp,theta;       /* D prec for trig recur    */
    float FAR * real;                       /* Real array array portion */
    float FAR * imag;                       /* Imaginary array portion  */

    /********************************************************************/
    theta= dbPI/(double) n;                 /* Initialize recurrence.   */
    real = &data[0];                        /* Real array array portion */
    imag = &data[1];                        /* Imaginary array portion  */

    /********************************************************************/
    if (isign == EFFFFTFWD) {
        c2 = (float) -0.5;
        FFTCpx (data, n, EFFFFTFWD);           /* The forward transform    */
    }
    else {                                  /* Otherwise set for inv    */
        c2 = (float) 0.5;                             
        theta = -theta;
    }
    wtemp = sin (0.5*theta);
    wpr = -2.0 * wtemp * wtemp;
    wpi = sin (theta);
    wr = 1.0 + wpr;
    wi = wpi;
    for (i=2; i<(int)n; i+=2) {             /* case i=0 done separately */
        ni = 2*n - i;
        h1r =  c1*(real[i] + real[ni]);     /* Two xforms separated out */
        h1i =  c1*(imag[i] - imag[ni]);
        h2r = -c2*(imag[i] + imag[ni]);
        h2i =  c2*(real[i] - real[ni]);
        real[i]  = (float) ( h1r+wr*h2r - wi*h2i);  /* Recombined here  */
        imag[i]  = (float) ( h1i+wr*h2i + wi*h2r);
        real[ni] = (float) ( h1r-wr*h2r + wi*h2i);
        imag[ni] = (float) (-h1i+wr*h2i + wi*h2r);
        wr = (wtemp=wr)*wpr-wi*wpi+wr;              /* Trig recurrence  */
        wi = wi*wpr+wtemp*wpi+wi;
    }
    /********************************************************************/
    /* Squeeze the first & last data item together to fit orig array    */
    /********************************************************************/
    if (isign == EFFFFTFWD) {
        real[0] = (h1r=real[0])+imag[0];
        imag[0] = h1r-imag[0];
        for (i=0; i <(int)(2*n); i+=2) {    /* Normalize                */
            real[i] /= 2 * n;
            imag[i] /= 2 * n;
        }
    }
    else {
        real[0]=c1*((h1r=real[0])+imag[0]);
        imag[0]=c1*(h1r-imag[0]);
        FFTCpx (data, n, EFFFFTINV);        /* Inv trans for ISIGN = -1 */
        for (i=0; i <(int)(2*n); i+=2) {    /* Normalize                */
            real[i] *= (n / 2);
            imag[i] *= (n / 2);
        }
    }
}


/************************************************************************/
/* Replaces data[] by its discrete Fourier Transform (if isign = 1);    */
/* or replaces data[] by nn times its inverse discrete Fourier          */
/* transform (if isign = -1). data[] is a complex array of length nn,   */
/* input as a float array of data[0...2*nn-1]. nn MUST be an integer    */
/* power of 2 (this is not checked for!).                               */
/************************************************************************/
void FFTCpx (float FAR *data, int nn, int isign)
{
    int n,mmax,m,j,istep,i;
    double wtemp,wr,wpr,wpi,wi,theta;       /* Dbl prec for trig recur  */
    float tempr,tempi;

    n=nn << 1;
    j=0;
    for (i=0; i<n-1; i+=2) {            /* This is the bit-reversal     */
        if (j > i) {
            SWAP(data[j],data[i]);      /* Exchange two complex numbers */
            SWAP(data[j+1],data[i+1]);
        }
        m=n >> 1;
        while (m >= 2 && j >= m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    /********************************************************************/
    /* Here begins the Danielson-Lanczos section of routine             */
    /********************************************************************/
    mmax=2;
    while (n > mmax) {                      /* Outer loop log2 nn times */
        istep=2*mmax;
        theta=db2PI/(isign*mmax);           /* Init for trig recur  */
        wtemp=sin(0.5*theta);
        wpr = -2.0*wtemp*wtemp;
        wpi=sin(theta);
        wr=1.0;
        wi=0.0;
        for (m=0; m<mmax-1; m+=2) {         /* Two nested inner loops  */
            for (i=m; i<=n-1; i+=istep) {
                j = i+mmax;                 /* Danielson-Lanczos form  */
                tempr = (float) (wr*data[j]-wi*data[j+1]);
                tempi = (float) (wr*data[j+1]+wi*data[j]);
                data[j] = data[i]-tempr;
                data[j+1] = data[i+1]-tempi;
                data[i] += tempr;
                data[i+1] += tempi;
            }
            wr=(wtemp=wr)*wpr-wi*wpi+wr;
            wi=wi*wpr+wtemp*wpi+wi;
        }                                   /* Trigonometric recurrence */
        mmax=istep;
    }
}

