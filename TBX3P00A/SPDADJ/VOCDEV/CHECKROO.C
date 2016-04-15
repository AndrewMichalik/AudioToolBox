//#include "pv.h"
//
//void findpoles( float coef[], complex pole[], int M ) {
// int i ;
// complex *a ;
//
//    a = (complex *) malloc( (M+1)*sizeof(complex) ) ;
//    for ( i = 0 ; i <= M ; i++ ) {
//	a[i].re = coef[i] ;
//	a[i].im = 0. ;
//    }
//    findroots( a, pole, M ) ;
//    for ( i = 0 ; i < M ; i++ )
//	pole[i] = cdiv( one, conjg( pole[i] ) ) ;
//    free( a ) ;
//}
