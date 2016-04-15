//#include "pv.h"
//
///*
// * polynomial is in a[] in the form
// *         a[0] + a[1]*x + a[2]*x^2 + ... + a[M]*x^M
// * if P is 0, laguerre attempts to return a root to
// * within eps of its value, given an initial guess x;
// * if P is nonzero, eps is ignored and laguerre attempts
// * to improve the guess x to within the achievable
// * roundoff limit, specified as "tiny"
// */
//complex laguerre( complex a[], INT M, complex x, float eps, INT P ) {
// complex dx, x1, b, d, f, g, h, sq, gp, gm, g2, q ;
// INT i, j, npol ;
// float dxold, cdx, tiny = 1.e-15 ;
//    if ( P ) {
//	dxold = CABS( x ) ;
//	npol = 0 ;
//    }
///*
// * iterate up to 100 times
// */
//    for ( i = 0 ; i < 100 ; i++ ) {
//	b = a[M] ;
//	d = zero ;
//	f = zero ;
///*
// * compute polynomial and its first two derivatives
// */
//	for ( j = M-1 ; j >= 0 ; j-- ) {
//	    f = cadd( cmult( x, f ), d ) ;
//	    d = cadd( cmult( x, d ), b ) ;
//	    b = cadd( cmult( x, b ), a[j] ) ;
//	}
//	if ( CABS( b ) <= tiny )      /* are we on the root? */
//	    dx = zero ;
//	else if ( CABS( d ) <= tiny && CABS( f ) <= tiny ) {
//	    q = cdiv( b, a[M] ) ;  /* this is a special case */
//	    dx.re = pow( CABS( q ), 1./M ) ;
//	    dx.im = 0. ;
//	} else {          /* general case: Laguerre's method */
//	    g = cdiv( d, b ) ;
//	    g2 = cmult( g, g ) ;
//	    h = csub( g2, scmult( 2., cdiv( f, b ) ) ) ;
//	    sq = csqrt( 
//		scmult( (float) M-1,
//		    csub( scmult( (float) M, h ), g2 )
//		)
//	    ) ;
//	    gp = cadd( g, sq ) ;
//	    gm = csub( g, sq ) ;
//	    if ( CABS( gp ) < CABS( gm ) )
//		gp = gm ;
//	    q.re = M ; q.im = 0. ;
//	    dx = cdiv( q, gp ) ;
//	}
//	x1 = csub( x, dx ) ;
//	if ( x1.re == x.re && x1.im == x.im )
//	    return( x ) ;                  /* converged */
//	x = x1 ;
//	if ( P ) {
//	    npol++ ;
//	    cdx = CABS( dx ) ;
//	    if ( npol > 9 && cdx >= dxold )
//		return( x ) ; /* reached roundoff limit */
//	    dxold = cdx ;
//	} else 
//	    if ( CABS( dx ) <= eps*CABS( x ) )
//		return( x ) ;              /* converged */
//    }
//    fprintf( stderr, "root convergence failure" ) ;
//    return( x ) ;                          /* best we could do */
//}
