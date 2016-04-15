#include "pv.h"
/*
 * read up to size samples into array from stdin
 * zero-pad if EOF encountered while reading
 * return number of samples actually read
 */
INT readinput( float array[], INT size ) {
 float value ;
 INT count ;
    for( count = 0 ; count < size ; count++ ) {
	if ( scanf( "%f", &value ) == EOF )
	    break ;
	array[count] = value ;
    }
    if ( count < size ) {
     INT i = count ;
	while ( i < size )
	    array[i++] = 0. ;
    }
    return( count ) ;
}
