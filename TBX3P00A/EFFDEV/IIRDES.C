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
/************************************************************************/
typedef float FAR * FLTPTR;
typedef long  FAR * LNGPTR;  

/************************************************************************/
/*                      Forward References                              */
/************************************************************************/
void spiird (LNGPTR, LNGPTR, LNGPTR, LNGPTR, FLTPTR, FLTPTR, FLTPTR, FLTPTR,
     FLTPTR, FLTPTR, FLTPTR, LNGPTR);

/************************************************************************/
/*                  IIR Low Pass filter design                          */
/************************************************************************/
WORD    EffIIROpt (FTRTYP usFtrTyp, PASTYP usPasTyp, WORD usSecCnt, 
                WORD usSecLen, DWORD ulStpFrq, float flPasFac, float flStpAtt, 
                DWORD ulSmpFrq, BOOL bfFstFlg, TDOFTR FAR *lpTDoFtr)
{
    long    lFtrTyp;
    long    lPasTyp;
    long    lSecCnt;
    long    lSecLen;
    float   fFrq002; 
    float   fFrq001;
    float   fFrq003;
    float   fFrq004;
    float   fStpAtt;
    long    lErrCod;
    WORD    usi;

    /********************************************************************/
    /* Zero filter block                                                */
    /********************************************************************/
    _fmemset (lpTDoFtr, 0, sizeof (*lpTDoFtr));         /* Initialize   */

    /********************************************************************/
    /* Check frequency parameters                                       */
    /********************************************************************/
    if (!usSecLen || ((usSecCnt * usSecLen) > TDFBLKMAX)) return ((WORD) -1);
    if ((ulStpFrq >= ulSmpFrq / 2) || (flPasFac > 1)) return ((WORD) -1);

    /********************************************************************/
    /* Initialize filter design parameters                              */
    /********************************************************************/
    lFtrTyp = usFtrTyp;
    lPasTyp = 1;
    lSecCnt = usSecCnt;
    lSecLen = usSecLen - 1;
    fFrq002 = (float) (ulStpFrq / (float) ulSmpFrq);    
    fFrq001 = ((1 - flPasFac) * fFrq002);    
    fFrq003 = 0;    
    fFrq004 = 0;    
    fStpAtt = flStpAtt;
    lErrCod;

    /********************************************************************/
    /********************************************************************/
    spiird (&lFtrTyp, &lPasTyp, &lSecCnt, &lSecLen, &fFrq001, &fFrq002, 
        &fFrq003, &fFrq004, &fStpAtt, lpTDoFtr->tbZer.flCoe, 
        lpTDoFtr->tbPol.flCoe, &lErrCod);
    if (lErrCod) return ((WORD) lErrCod);

    /********************************************************************/
    /********************************************************************/
    lpTDoFtr->usTyp = usFtrTyp;
    lpTDoFtr->usSec = usSecCnt;    
    lpTDoFtr->bfFst = bfFstFlg;    
    lpTDoFtr->tbZer.usLen = usSecLen;
    lpTDoFtr->tbPol.usLen = usSecLen - 1;

    /********************************************************************/
    /* Adjust coefficients for long fraction math (if requested)        */
    /********************************************************************/
    if (lpTDoFtr->bfFst) {
        for (usi = 0; usi < lpTDoFtr->usSec * lpTDoFtr->tbZer.usLen; usi++) 
            lpTDoFtr->tbZer.lfCoe[usi] = (LNGFRA) (LNGFRAONE * lpTDoFtr->tbZer.flCoe[usi]);
        for (usi = 0; usi < lpTDoFtr->usSec * lpTDoFtr->tbPol.usLen; usi++) 
            lpTDoFtr->tbPol.lfCoe[usi] = (LNGFRA) (LNGFRAONE * lpTDoFtr->tbPol.flCoe[usi]);
    }

    /********************************************************************/
    /********************************************************************/
    return (0);

}

/************************************************************************/
/************************************************************************/
#define M_PI    3.14159265358979323846
#define ABS(x) ((x) >= 0 ? (x) : -(x))
#define MIN(a,b) ((a) <= (b) ? (a) : (b))
#define MAX(a,b) ((a) >= (b) ? (a) : (b))
static double pos_d10 = 10.0;
static long pos_i4 = 4;

/************************************************************************/
/* SPBFCT     11/14/85 */
/* GENERATES (I1)!/(I1-I2)!=I1*(I1-1)*...*(I1-I2+1). */
/* NOTE: 0!=1 AND SPBFCT(I,I)=SPBFCT(I,I-1)=I!. */

#ifndef KR
double spbfct(LNGPTR i1, LNGPTR i2)
#else
double spbfct(i1, i2)
LNGPTR i1, *i2;
#endif
{
    /* Local variables */
    long i;
    double ret_val;

    ret_val = 0.0;
    if (*i1 < 0 || *i2 < 0 || *i2 > *i1)
    {
    return(ret_val);
    }

    ret_val = 1.0;
    for (i = *i1 ; i >= (*i1 - *i2 + 1) ; --i)
    {
    ret_val *= (double) i;
    }

    return(ret_val);
} /* spbfct */

/************************************************************************/
#ifndef KR
double pow_ri(FLTPTR ap, LNGPTR bp)
#else
double pow_ri(ap, bp)
FLTPTR ap;
LNGPTR bp;
#endif
{
    double pow, x;
    long n;

    pow = 1;
    x = *ap;
    n = *bp;

    if(n != 0)
    {
    if(n < 0)
    {
        if(x == 0)
        {
        return(pow);
        }
        n = -n;
        x = 1/x;
    }

    for( ; ; )
    {
        if(n & 01)
        pow *= x;

        if(n >>= 1)
        x *= x;
        else
        break;
    }
    }

    return(pow);
}

/************************************************************************/
/* SPBILN     11/13/85 */
/* CONVERTS ANALOG H(S) TO DIGITAL H(Z) VIA BILINEAR TRANSFORM */
/*      ANALOG TRANSFER FUNCTION         DIGITAL TRANSFER FUNCTION */
/*          D(L)*S**L+.....+D(0)             B(0)+......+B(L)*Z**-L */
/*    H(S)=---------------------        H(Z)=---------------------- */
/*          C(L)*S**L+.....+C(0)               1+.......+A(L)*Z**-L */
/* H(S) IS ASSUMED TO BE PRE-SCALED AND PRE-WARPED */
/* LN SPECIFIES THE LENGTH OF THE COEFFICIENT ARRAYS */
/* FILTER ORDER L IS COMPUTED INTERNALLY */
/* WORK IS AN INTERNAL ARRAY (2D) SIZED TO MATCH COEF ARRAYS */
/* IERROR=0    NO ERRORS DETECTED IN TRANSFORMATION */
/*        1    ALL ZERO TRANSFER FUNCTION */
/*        2    INVALID TRANSFER FUNCTION; Y(K) COEF=0 */

#ifndef KR
void spbiln(FLTPTR d, FLTPTR c, LNGPTR ln, FLTPTR b, FLTPTR a, FLTPTR work, LNGPTR error)
#else
void spbiln(d, c, ln, b, a, work, error)
LNGPTR ln, *error;
FLTPTR d, *c, *b, *a, *work;
#endif
{
    /* Local variables */
    long i, j, l, zero_func, work_dim1;
    float scale, tmp;
    double atmp;

    zero_func = TRUE;
    for (i = *ln ; i >= 0 && zero_func ; --i)
    {
    if (c[i] != 0.0 || d[i] != 0.0)
    {
        zero_func = FALSE;
    }
    }

    if ( zero_func )
    {
        *error = 1;
        return;
    }

    work_dim1 = *ln + 1;

    l = i + 1;
    for (j = 0 ; j <= l ; ++j)
    {
    work[j * work_dim1] = (float) 1.0;
    }

    tmp = (float) 1.0;
    for (i = 1 ; i <= l ; ++i)
    {
    tmp = tmp * (float) (l - i + 1) / (float) i;
    work[i] = tmp;
    }

    for (i = 1 ; i <= l ; ++i)
    {
    for (j = 1 ; j <= l ; ++j)
    {
        work[i + j * work_dim1] = work[i + (j - 1) * work_dim1]
                      - work[i - 1 + j * work_dim1]
                      - work[i - 1 + (j - 1) * work_dim1];
    }
    }

    for (i = l ; i >= 0 ; --i)
    {
    b[i] = (float) 0.0;
    atmp = (float) 0.0;

    for (j = 0 ; j <= l ; ++j)
    {
        b[i] += work[i + j * work_dim1] * d[j];
        atmp += (double) work[i + j * work_dim1] * c[j];
    }

    scale = (float) atmp;

    if (i != 0)
    {
        a[i - 1] = (float) atmp;
    }
    }

    if (scale == 0.0)
    {
    *error = 2;
    return;
    }

    b[0] /= scale;
    for (i = 1 ; i <= l ; ++i)
    {
    b[i] /= scale;
    a[i - 1] /= scale;
    }

    if (l < *ln)
    {
    for (i = l + 1 ; i <= *ln ; ++i)
    {
        b[i] = (float) 0.0;
        a[i - 1] = (float) 0.0;
    }
    }

    *error = 0;
    return;
} /* spbiln */

/************************************************************************/
/* SPCHBI     11/13/85 */
/* GENERATES KTH SECTION COEFFICIENTS FOR LTH ORDER NORMALIZED */
/*             LOWPASS CHEBYSHEV TYPE I ANALOG FILTER */
/* SECOND ORDER SECTIONS:  K<=(L+1)/2 */
/* ODD ORDER L: LAST SECTION WILL CONTAIN SINGLE POLE */
/* LN DEFINES COEFFICIENT ARRAY SIZE */
/* EP REGULATES THE PASSBAND RIPPLE */
/* TRANSFER FUNCTION SCALING IS INCLUDED IN FIRST SECTION (L EVEN) */
/* ANALOG COEFFICIENTS ARE RETURNED IN D AND C */
/* IERROR=0    NO ERRORS DETECTED */
/*        1    INVALID FILTER ORDER L */
/*        2    INVALID SECTION NUMBER K */
/*        3    INVALID RIPPLE PARAMETER EP */

#ifndef KR
void spchbi(LNGPTR l, LNGPTR k, LNGPTR ln, FLTPTR ep, FLTPTR d, FLTPTR c, LNGPTR error)
#else
void spchbi(l, k, ln, ep, d, c, error)
LNGPTR l, *k, *ln, *error;
FLTPTR ep, *d, *c;
#endif
{
    /* Local variables */
    long ll;
    float omega, sigma, gam;

    if (*l <= 0)
    {
    *error = 1;
    return;
    }
    if (*k <= 0 || *k > ((*l + 1) / 2))
    {
    *error = 2;
    return;
    }
    if (*ep <= 0.0)
    {
    *error = 3;
    return;
    }

    gam = (float) pow((1.0 + sqrt((double) (1.0 + *ep * *ep))) / (double) *ep,
          1.0 / (double) *l);

    sigma = (float) (0.5 * (1.0 / gam - gam)
        * (float) sin((double) ((*k * 2) - 1)
              * M_PI / (double) (*l * 2)));

    omega = (float) (0.5 * (gam + 1.0 / gam)
        * (float) cos((double) ((*k * 2) - 1)
              * M_PI / (double) (*l * 2)));

    for (ll = 0 ; ll <= *ln ; ++ll)
    {
    d[ll] = (float) 0.0;
    c[ll] = (float) 0.0;
    }

    if ((*l / 2) != ((*l + 1) / 2) && *k == ((*l + 1) / 2))
    {
    d[0] = (float) -1.0 * sigma;
    c[0] = d[0];
    c[1] = (float) 1.0;

    *error = 0;
    return;
    }

    c[0] = sigma * sigma + omega * omega;
    c[1] = (float) -2.0 * sigma;
    c[2] = (float) 1.0;
    d[0] = c[0];

    if ((*l / 2) == ((*l + 1) / 2) && *k == 1)
    {
    d[0] /= (float) sqrt((double) (1.0 + *ep * *ep));
    }

    *error = 0;
    return;
} /* spchbi */

/************************************************************************/
/* SPCBII     11/13/85 */
/* GENERATES KTH SECTION COEFFICIENTS FOR LTH ORDER NORMALIZED */
/*       LOWPASS CHEBYSHEV TYPE II ANALOG FILTER */
/* SECOND ORDER SECTIONS:  K<= (L+1)/2 */
/* ODD ORDER L:  FINAL SECTION WILL CONTAIN SINGLE POLE */
/* LN DEFINES COEFFICIENT ARRAY SIZE */
/* WS AND ATT REGULATE STOPBAND ATTENUATION */
/*       MAGNITUDE WILL BE 1/ATT AT WS RAD/SEC */
/* ANALOG COEFFICIENTS ARE RETURNED IN ARRAYS D AND C */
/* IERROR=0      NO ERRORS DETECTED */
/*        1      INVALID FILTER ORDER L */
/*        2      INVALID SECTION NUMBER K */
/*        3      INVALID STOPBAND FREQUENCY WS */
/*        4      INVALID ATTENUATION PARAMETER */

#ifndef KR
void spcbii(LNGPTR l, LNGPTR k, LNGPTR ln, FLTPTR ws, FLTPTR att, FLTPTR d, FLTPTR c, LNGPTR error)
#else
void spcbii(l, k, ln, ws, att, d, c, error)
LNGPTR l, *k, *ln, *error;
FLTPTR ws, *att, *d, *c;
#endif
{
    /* Local variables */
    long ll;
    float beta, scld, scln, alpha, omega, sigma, gam, tmp_float;

    if (*l <= 0)
    {
    *error = 1;
    return;
    }
    if (*k > ((*l + 1) / 2) || *k < 1)
    {
    *error = 2;
    return;
    }
    if (*ws <= 1.0)
    {
    *error = 3;
    return;
    }
    if (*att <= 0.0)
    {
    *error = 4;
    return;
    }

    gam = (float) pow((double) (*att + sqrt((double) (*att * *att - 1.0))),
          (double) (1.0 / (double) *l));

    alpha = (float) (0.5 * (1.0 / gam - gam)
        * (float) sin((double) ((*k * 2) - 1)
              * M_PI / (double) (*l * 2)));

    beta = (float) (0.5 * (gam + 1.0 / gam)
        * (float) cos((double) ((*k * 2) - 1)
              * M_PI / (double) (*l * 2)));

    sigma = (*ws * alpha) / (alpha * alpha + beta * beta);

    omega = (float) (-1.0 * *ws * beta) / (alpha * alpha + beta * beta);

    for (ll = 0 ; ll <= *ln ; ++ll)
    {
    d[ll] = (float) 0.0;
    c[ll] = (float) 0.0;
    }

    if ((*l / 2) != ((*l + 1) / 2) && *k == ((*l + 1) / 2))
    {
    d[0] = (float) -1.0 * sigma;
    c[0] = d[0];
    c[1] = (float) 1.0;

    *error = 0;
    return;
    }

    scln = sigma * sigma + omega * omega;

    tmp_float = *ws / (float) cos((double) ((*k * 2) - 1)
                  * M_PI / (double) (*l * 2));
    scld = tmp_float * tmp_float;

    d[0] = scln * scld;
    d[2] = scln;
    c[0] = d[0];
    c[1] = (float) -2.0 * sigma * scld;
    c[2] = scld;

    *error = 0;
    return;
} /* spcbii */

/************************************************************************/
/* SPFBLT     10/26/90 */
/* CONVERTS NORMALIZED LP ANALOG H(S) TO DIGITAL H(Z) */
/*       ANALOG TRANSFER FUNCTION           DIGITAL TRANSFER FUNCTION */
/*        D(M)*S**M+.....+D(0)               B(0)+.....+B(L)*Z**-L */
/*   H(S)=--------------------          H(Z)=-------------------- */
/*        C(M)*S**M+.....+C(0)                 1+......+A(L)*Z**-L */
/* FILTER ORDER L IS COMPUTED INTERNALLY */
/* IBAND=1    LOWPASS            FLN=NORMALIZED CUTOFF IN HZ-SEC */
/*       2    HIGHPASS           FLN=NORMALIZED CUTOFF IN HZ-SEC */
/*       3    BANDPASS           FLN=LOW CUTOFF; FHN=HIGH CUTOFF */
/*       4    BANDSTOP           FLN=LOW CUTOFF; FHN=HIGH CUTOFF */
/* LN SPECIFIES COEFFICIENT ARRAY SIZE */
/* WORK(0:LN,0:LN) IS A WORK ARRAY USED INTERNALLY */
/* RETURN IERROR=0    NO ERRORS DETECTED */
/*               1    ALL ZERO TRANSFER FUNCTION */
/*               2    SPBILN: INVALID TRANSFER FUNCTION */
/*               3    FILTER ORDER EXCEEDS ARRAY SIZE */
/*               4    INVALID FILTER TYPE PARAMETER (IBAND) */
/*               5    INVALID CUTOFF FREQUENCY */

#ifndef KR
void spfblt(FLTPTR d, FLTPTR c, LNGPTR ln, LNGPTR iband, FLTPTR fln, FLTPTR fhn, FLTPTR b, FLTPTR a, FLTPTR work, LNGPTR error)
#else
void spfblt(d, c, ln, iband, fln, fhn, b, a, work, error)
LNGPTR ln, *iband, *error;
FLTPTR d, *c, *fln, *fhn, *b, *a, *work;
#endif
{
    /* Builtin functions */
    double pow_ri();

    /* Local variables */
    double spbfct();
//    void spbiln();

    long work_dim1, tmp_int, i, k, l, m, zero_func, ll, mm, ls;
    float tmpc, tmpd, w, w1, w2, w02, tmp;

    if (*iband < 1 || *iband > 4)
    {
    *error = 4;
    return;
    }
    if ((*fln <= 0.0 || *fln > 0.5) ||
    (*iband >= 3 && ( *fln >= *fhn || *fhn > 0.5 )))
    {
    *error = 5;
    return;
    }

    zero_func = TRUE;
    for (i = *ln ; i >= 0 && zero_func ; --i)
    {
    if (c[i] != 0.0 || d[i] != 0.0)
    {
        zero_func = FALSE;
    }
    }

    if ( zero_func ) 
    {
        *error = 1;
        return;
    }

    work_dim1 = *ln + 1;

    m = i + 1;
    w1 = (float) tan(M_PI * *fln);
    l = m;
    if (*iband > 2)
    {
        l = m * 2;
        w2 = (float) tan(M_PI * *fhn);
        w = w2 - w1;
        w02 = w1 * w2;
    }

    if (l > *ln)
    {
    *error = 3;
    return;
    }

    switch (*iband)
    {
    case 1:

/* SCALING S/W1 FOR LOWPASS,HIGHPASS */

        for (mm = 0 ; mm <= m ; ++mm)
        {
        d[mm] /= (float) pow_ri(&w1, &mm);
        c[mm] /= (float) pow_ri(&w1, &mm);
        }

        break;

    case 2:

/* SUBSTITUTION OF 1/S TO GENERATE HIGHPASS (HP,BS) */

        for (mm = 0 ; mm <= (m / 2) ; ++mm)
        {
        tmp = d[mm];
        d[mm] = d[m - mm];
        d[m - mm] = tmp;
        tmp = c[mm];
        c[mm] = c[m - mm];
        c[m - mm] = tmp;
        }

/* SCALING S/W1 FOR LOWPASS,HIGHPASS */

        for (mm = 0 ; mm <= m ; ++mm)
        {
        d[mm] /= (float) pow_ri(&w1, &mm);
        c[mm] /= (float) pow_ri(&w1, &mm);
        }

        break;

    case 3:

/* SUBSTITUTION OF (S**2+W0**2)/(W*S)  BANDPASS,BANDSTOP */

        for (ll = 0 ; ll <= l ; ++ll)
        {
        work[ll] = (float) 0.0;
        work[ll + work_dim1] = (float) 0.0;
        }

        for (mm = 0 ; mm <= m ; ++mm)
        {
        tmp_int = m - mm;
        tmpd = d[mm] * (float) pow_ri(&w, &tmp_int);
        tmpc = c[mm] * (float) pow_ri(&w, &tmp_int);

        for (k = 0 ; k <= mm ; ++k)
        {
            ls = m + mm - (k * 2);
            tmp_int = mm - k;
            tmp = (float) (spbfct(&mm,&mm) / (spbfct(&k,&k)
                         * spbfct(&tmp_int,&tmp_int)));
            work[ls] += (float) (tmpd * pow_ri(&w02, &k) * tmp);
            work[ls + work_dim1] += (float) (tmpc * pow_ri(&w02, &k) * tmp);
        }
        }

        for (ll = 0 ; ll <= l ; ++ll)
        {
        d[ll] = work[ll];
        c[ll] = work[ll + work_dim1];
        }

        break;

    case 4:

/* SUBSTITUTION OF 1/S TO GENERATE HIGHPASS (HP,BS) */

        for (mm = 0 ; mm <= (m / 2) ; ++mm)
        {
        tmp = d[mm];
        d[mm] = d[m - mm];
        d[m - mm] = tmp;
        tmp = c[mm];
        c[mm] = c[m - mm];
        c[m - mm] = tmp;
        }

/* SUBSTITUTION OF (S**2+W0**2)/(W*S)  BANDPASS,BANDSTOP */

        for (ll = 0 ; ll <= l ; ++ll)
        {
        work[ll] = (float) 0.0;
        work[ll + work_dim1] = (float) 0.0;
        }

        for (mm = 0 ; mm <= m ; ++mm)
        {
        tmp_int = m - mm;
        tmpd = d[mm] * (float) pow_ri(&w, &tmp_int);
        tmpc = c[mm] * (float) pow_ri(&w, &tmp_int);

        for (k = 0 ; k <= mm ; ++k)
        {
            ls = m + mm - (k * 2);
            tmp_int = mm - k;
            tmp = (float) (spbfct(&mm,&mm) / (spbfct(&k,&k)
                         * spbfct(&tmp_int,&tmp_int)));
            work[ls] += (float) (tmpd * pow_ri(&w02, &k) * tmp);
            work[ls + work_dim1] += (float) (tmpc * pow_ri(&w02, &k) * tmp);
        }
        }

        for (ll = 0 ; ll <= l ; ++ll)
        {
        d[ll] = work[ll];
        c[ll] = work[ll + work_dim1];
        }

        break;

    }

    spbiln(d, c, ln, b, a, work, error);

    return;
} /* spfblt */

/* SPBWCF     11/13/85 */
/* GENERATES KTH SECTION COEFFICIENTS FOR LTH ORDER NORMALIZED */
/*       LOWPASS BUTTERWORTH FILTER */
/* SECOND ORDER SECTIONS: K<=(L+1)/2 */
/* ODD ORDER L:  FINAL SECTION WILL CONTAIN 1ST ORDER POLE */
/* LN DEFINES COEFFICIENT ARRAY SIZE */
/* ANALOG COEFFICIENTS ARE RETURNED IN D AND C */
/* IERROR=0      NO ERRORS DETECTED */
/*        1      INVALID FILTER ORDER L */
/*        2      INVALID SECTION NUMBER K */

#ifndef KR
void spbwcf(LNGPTR l, LNGPTR k, LNGPTR ln, FLTPTR d, FLTPTR c, LNGPTR error)
#else
void spbwcf(l, k, ln, d, c, error)
LNGPTR l, *k, *ln, *error;
FLTPTR d, *c;
#endif
{
    /* Local variables */
    long i;
    float tmp;

    if (*l <= 0)
    {
    *error = 1;
    return;
    }

    if (*k <= 0 || *k > ((*l + 1) / 2))
    {
    *error = 2;
    return;
    }

    d[0] = (float) 1.0;
    c[0] = (float) 1.0;
    for (i = 1 ; i <= *ln ; ++i)
    {
    d[i] = (float) 0.0;
    c[i] = (float) 0.0;
    }

    tmp = (float) (*k - (*l + 1.0) / 2.0);

    if (tmp == 0.0)
    {
    c[1] = (float) 1.0;
    }
    else
    {
    c[1] = (float) (-2.0 * cos((double) ((*k * 2) + *l - 1)
                   * M_PI / (double) (*l * 2)));
    c[2] = (float) 1.0;
    }

    *error = 0;
    return;
} /* spbwcf */

/************************************************************************/
/* SPIIRD     05/09/86 */
/* IIR LOWPASS, HIGHPASS, BANDPASS, AND BANDSTOP DESIGN OF CHEBYSHEV 1, */
/*   CHEBYSHEV 2, AND BUTTERWORTH DIGITAL FILTERS IN CASCADE FORM. */
/* IFILT=1(CHEB1-PASSBAND RIPPLE), 2(CHEB2-STOPBAND RIPPLE), OR */
/*       3(BUTTERWORTH-NO RIPPLE). */
/* IBAND=1(LOWPASS), 2(HIGHPASS), 3(BANDPASS), OR 4(BANDSTOP). */
/* NS   =NUMBER OF SECTIONS IN CASCADE. */
/* LS   =ORDER OF EACH SECTION: USUALLY 2(IBAND=1,2) OR 4(IBAND=3,4). */
/* F1-F4=FREQ. IN HZ-SEC. (SAMPLING FREQ.=1.0) AS IN PLOTS BELOW. */

/*     LOWPASS        HIGHPASS       BANDPASS        BANDSTOP */

/*         F  F         F  F           F F  F F        F F F F */
/*         1  2         1  2           1 2  3 4        1 2 3 4 */
/*   0 XXX-------   0 +------XXX   0 +----XX----   0 XX--------X */
/*     I X .  .       I .  . X       I . .XX. .      IX. . . .X */
/*     I  X.  .       I .  .X        I . .XX. .      IX. . . .X */
/*     I...X  .       I....X         I...X..X .      I.X.....X */
/*     I    X .       I . X          I . X  X .      I  X. .X */
/*     I     X.       I .X           I .X    X.      I  X. .X */
/*  DB I......X    DB I.X         DB I.X......X   DB I...XXX */
/*     I       X      IX             IX        X     I */

/*       F3 AND F4 ARE NOT USED WITH ANY LOW OR HIGHPASS. */
/*       F2 IS NOT USED WITH LOWPASS BUTTERWORTH. */
/*       F1 IS NOT USED WITH HIGHPASS BUTTERWORTH. */
/*       F1 AND F4 ARE NOT USED WITH BANDPASS BUTTERWORTH. */
/*       F2 AND F3 ARE NOT USED WITH BANDSTOP BUTTERWORTH. */

/* DB   =DB OF STOPBAND REJECTION.  APPLIES TO CHEB. FILTERS ONLY. */
/*       NOT USED WITH BUTTERWORTH.  MUST BE GREATER THAT 3 DB. */
/* B    =NUMERATOR COEFFICIENTS, ALWAYS DIMENSIONED B(0:LS,NS). */
/* A    =DENOMINATOR COEFFICIENTS, ALWAYS DIMENSIONED A(LS,NS). */
/* IERROR=0    NO ERRORS. */
/*        1-5  SEE SPFBLT ERROR LIST. */
/*        6    IFILT OR IBAND OUT OF RANGE. */
/*        7    F1-F4 NOT IN SEQUENCE OR NOT BETWEEN O.O AND 0.5, */
/*             OR DB NOT GREATER THAN 3. */
/*        11+  SEE SPCHBI, SPCBII, OR SPBWCF ERROR LIST. */

#ifndef KR
void spiird(LNGPTR ifilt, LNGPTR iband, LNGPTR ns, LNGPTR ls, FLTPTR f1, FLTPTR f2, FLTPTR f3, FLTPTR f4, FLTPTR db, FLTPTR b, FLTPTR a, LNGPTR error)
#else
void spiird(ifilt, iband, ns, ls, f1, f2, f3, f4, db, b, a, error)
LNGPTR ifilt, *iband, *ns, *ls, *error;
FLTPTR f1, *f2, *f3, *f4, *db, *b, *a;
#endif
{
    /* Local variables */
    void spchbi(), spcbii(), spbwcf(), spfblt();

    long k, b_dim1, a_dim1, tmp_int;
    float omega, fh, alamda, fl, epslon, work[25], c[5], d[5];
    double tmp_double1, tmp_double2, tmp_double3, tmp_double4;

    if (0 >= *ns ||
    (*ifilt < 1 || *ifilt > 3) ||
    (*iband < 1 || *iband > 4))
    {
    *error = 6;
    return;
    }

    if (*iband == 1 || *iband == 4)
    {
    fl = *f1;
    }
    else if (*iband == 2 || *iband == 3)
    {
    fl = *f2;
    }

    if (*iband <= 3)
    {
    fh = *f3;
    }
    else if (*iband == 4)
    {
    fh = *f4;
    }

    if ((*iband < 3 && ((*ifilt < 3 && (0.0 >= *f1 || *f1 >= *f2 || *f2 >= 0.5))
               || (*ifilt == 3 && (0.0 >= fl || fl >= 0.5)))) ||
//ajm
//        (*ifilt < 3 && ((0.0 >= *f1 || *f1 >= *f2 || *f2 >= *f3) ||
//               (*f3 >= *f4 || *f4 >= 0.5))) ||
        (*ifilt == 3 && (0.0 >= fl || fl >= fh || fh >= 0.5)) ||
        (*ifilt < 3 && *db <= 3.0))
    {
    *error = 7;
    return;
    }

    b_dim1 = *ls + 1;
    a_dim1 = *ls;

    if (*ifilt < 3)
    {
    if (*iband <= 2)
    {
        omega = (float) (tan(M_PI * *f2) / tan(M_PI * *f1));
    }
    else if (*iband == 3)
    {
        tmp_double1 = tan(M_PI * *f1);
        tmp_double2 = tan(M_PI * *f4);

        tmp_double3 = (tmp_double1 * tmp_double1
             - tan(M_PI * fh) * tan(M_PI * fl)) / ((tan(M_PI * fh) 
             - tan(M_PI * fl)) * tmp_double1);

        tmp_double4 = (tmp_double2 * tmp_double2
             - tan(M_PI * fh) * tan(M_PI * fl)) / ((tan(M_PI * fh) 
             - tan(M_PI * fl)) * tmp_double2);

        omega = (float) MIN(ABS(tmp_double4),ABS(tmp_double3));
    }
    else if (*iband == 4)
    {
        tmp_double1 = tan(M_PI * *f2);
        tmp_double2 = tan(M_PI * *f3);

        tmp_double3 = 1.0 / ((tmp_double1 * tmp_double1
             - tan(M_PI * fh) * tan(M_PI * fl)) / ((tan(M_PI * fh)
             - tan(M_PI * fl)) * tmp_double1));

        tmp_double4 = 1.0 / ((tmp_double2 * tmp_double2
             - tan(M_PI * fh) * tan(M_PI * fl)) / ((tan(M_PI * fh)
             - tan(M_PI * fl)) * tmp_double2));

        omega = (float) MIN(ABS(tmp_double4),ABS(tmp_double3));
    }

    alamda = (float) pow(pos_d10, (double) (*db / 20.0));
    tmp_double1 = (*ns * 2)
              * log((double) omega
                + sqrt((double) (omega * omega - 1.0)));
    epslon = alamda / (float) (0.5 * (exp(tmp_double1)
                  + exp( -tmp_double1)));
    }

    for (k = 1 ; k <= *ns ; ++k)
    {
    if (*ifilt == 1)
    {
        tmp_int = *ns * 2;
        spchbi(&tmp_int, &k, &pos_i4, &epslon, d, c, error);
    }
    else if (*ifilt == 2)
    {
        tmp_int = *ns * 2;
        spcbii(&tmp_int, &k, &pos_i4, &omega, &alamda, d, c, error);
    }
    else if (*ifilt == 3)
    {
        tmp_int = *ns * 2;
        spbwcf(&tmp_int, &k, &pos_i4, d, c, error);
    }

    if (*error != 0)
    {
        *error = 10 * *ifilt + *error;
        return;
    }

    spfblt(d, c, ls, iband, &fl, &fh, &b[(k - 1) * b_dim1],
           &a[(k - 1) * a_dim1], work, error);

    if (*error != 0)
    {
        return;
    }
    }

    return;
} /* spiird */



