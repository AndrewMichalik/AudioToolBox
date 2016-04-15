/************************************************************************/
/* ToolBox Licensing Defs: TBxLic.h                     V2.00  04/15/96 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
typedef enum _LICCMPCOD {
    LICCMPSUC =  0,                     /* Success                      */
    LICCMPFIO,                          /* File I/O Error               */
    LICCMPINS,                          /* Installation Characteristics */
    LICCMPSER,                          /* Serial Value                 */
} LICCMPCOD;

/************************************************************************/
/************************************************************************/
WORD    ChkLicFil (char far *, char far *, WORD far *);


