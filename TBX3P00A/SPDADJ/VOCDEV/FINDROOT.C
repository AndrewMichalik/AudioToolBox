//#include "pv.h"
//
///*
// * a[] contains M+1 complex polynomial coefficients in the order
// *         a[0] + a[1]*x + a[2]*x^2 + ... + a[M]*x^M
// * find and return its M roots in r[0] through r[M-1]
// */
//void findroots( complex a[], complex r[], int M ) {
// complex x, b, c ;
// float eps = 1.e-6 ;
// INT i, j, jj ;
// static complex *ad ;
// static INT LM ;
//    if ( M != LM ) {
//	if ( ad )
//	    free( ad ) ;
//	ad = (complex *) malloc( (M+1)*sizeof(complex) ) ;
//	LM = M ;
//    }
///*
// * make temp copy of polynomial coefficients
// */
//    for ( i = 0 ; i <= M ; i++ )
//	ad[i] = a[i] ;
///*
// * use Laguerre's method to estimate each root
// */
//    for ( j = M ; j > 0 ; j-- ) {
//	x = zero ;
//	x = laguerre( ad, j, x, eps, 0 ) ;
//	if ( fabs( x.im ) <= pow( 2.*eps, 2.*fabs( x.re ) ) )
//	    x.im = 0. ;
//	r[j-1] = x ;
///*
// * factor each root as it is found out of the polynomial
// * using synthetic division
// */
//	b = ad[j] ;
//	for ( jj = j - 1 ; jj >= 0 ; jj-- ) {
//	    c = ad[jj] ;
//	    ad[jj] = b ;
//	    b = cadd( cmult( x, b ), c ) ;
//	}
//    }
///*
// * polish each root, (i.e., improve its accuracy)
// * also by using Laguerre's method
//    for ( j = 0 ; j < M ; j++ )
//	r[j] = laguerre( a, M, r[j], eps, 1 ) ;
// */
///*
// * sort roots by their real parts
// */
//    for ( i = 0 ; i < M-1 ; i++ ) {
//	for ( j = i + 1 ; j < M ; j++ ) {
//	    if ( r[j].re < r[i].re ) {
//		x = r[i] ;
//		r[i] = r[j] ;
//		r[j] = x ;
//	    }
//	}
//    }
//}
