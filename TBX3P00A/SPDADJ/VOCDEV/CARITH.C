//#include "pv.h"
//
///*
// * complex arithmetic routines
// */
//complex cadd( complex x, complex y ) { /* return x + y */
// static complex z ;
//    z.re = x.re + y.re ;
//    z.im = x.im + y.im ;
//    return( z ) ;
//}
//complex csub( complex x, complex y ) { /* return x - y */
// static complex z ;
//    z.re = x.re - y.re ;
//    z.im = x.im - y.im ;
//    return( z ) ;
//}
//complex cmult( complex x, complex y ) { /* return x*y */
// static complex z ;
//    z.re = x.re*y.re - x.im*y.im ;
//    z.im = x.re*y.im + x.im*y.re ;
//    return( z ) ;
//}
//complex scmult( float s, complex x ) { /* return s*x */
// static complex z ;
//    z.re = s*x.re ;
//    z.im = s*x.im ;
//    return( z ) ;
//}
//complex cdiv( complex x, complex y ) { /* return x/y */
// static complex z ;
// float mag, ang ; /* polar arithmetic more robust here */
//    mag = CABS( x )/CABS( y ) ;
//    if ( x.re != 0. && y.re != 0. )
//	ang = atan2( x.im, x.re) - atan2( y.im, y.re) ;
//    else
//	ang = 0. ;
//    z.re = mag*cos( ang ) ;
//    z.im = mag*sin( ang ) ;
//    return( z ) ;
//}
//complex conjg( complex x ) { /* return x* */
// static complex y ;
//    y.re = x.re ;
//    y.im = -x.im ;
//    return( y ) ;
//}
//complex csqrt( complex x ) { /* return sqrt(x) */
// static complex z ;
// float mag, ang ;
//    mag = sqrt( CABS( x ) ) ;
//    if ( x.re != 0. )
//	ang = atan2( x.im, x.re)/2. ;
//    else
//	ang = 0. ;
//    z.re = mag*cos( ang ) ;
//    z.im = mag*sin( ang ) ;
//    return( z ) ;
//}

