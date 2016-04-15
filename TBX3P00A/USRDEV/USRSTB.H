/************************************************************************/
/* User Supplied I/O Stub Defs: UsrStb.h                V2.00  09/15/92 */
/* Copyright (c) 1991-1992 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
typedef unsigned long VISMEMHDL;

/************************************************************************/
/* User definable memory and I/O routines                               */
/************************************************************************/
unsigned long   cdecl far VISMemMax     (void);
unsigned long   cdecl far VISMemSiz     (VISMEMHDL);
VISMEMHDL       cdecl far VISMemAlo     (unsigned long);
VISMEMHDL       cdecl far VISMemAloChg  (VISMEMHDL, unsigned long);
VISMEMHDL       cdecl far VISMemAloRel  (VISMEMHDL);
void far *      cdecl far VISMemLck     (VISMEMHDL);
VISMEMHDL       cdecl far VISMemLckRel  (VISMEMHDL);
void far *      cdecl far VISBasAlo     (void far *, unsigned short);
void far *      cdecl far VISBasAloRel  (void far *);

/************************************************************************/
/************************************************************************/
short           cdecl far VISFilOpn (const char far *, unsigned short);
short           cdecl far VISFilCls (short);
long            cdecl far VISFilPos (short, long, unsigned short);
long            cdecl far VISFilLen (short);

/************************************************************************/
/************************************************************************/
int far pascal            VISPrtFmt (const char *, void *);
int far pascal            VISScnFmt (const char *, void *);


