/************************************************************************/
/* User Supplied I/O Stubs: UsrStb.c                    V2.00  09/15/92 */
/* Copyright (c) 1991-1992 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "usrstb.h"                     /* User supplied I/O stubs      */

#include <stdio.h>                      /* Standard I/O                 */
#include <stdlib.h>                     /* Standard library defs        */
#include <malloc.h>                     /* Memory allocation functions  */
#include <stdarg.h>                     /* ANSI Std var length args     */
#include <conio.h>                      /* Console I/O routines         */
#include <dos.h>                        /* Low level dos calls          */

/************************************************************************/
/*              Global Memory Information Functions                     */
/************************************************************************/
unsigned long cdecl far VISMemMax ()
{
    #define         GLOHDROOM  4096L
    VISMEMHDL       mhGloMem;
    unsigned long   ulReqSiz = 0xf000L;

    /********************************************************************/
    /* Future: Use Compact() with better _memmax() algorithm            */
    /********************************************************************/

    /********************************************************************/
    /* _memmax() is near heap only; do a quick search for largest block */
    /********************************************************************/
    while ((VISMEMHDL) NULL == (mhGloMem = VISMemAlo (ulReqSiz))) {
        ulReqSiz = (ulReqSiz * 3L) / 4L;
        if (ulReqSiz < GLOHDROOM) return (0L);
    }
    VISMemAloRel (mhGloMem);
    return (ulReqSiz);
}

unsigned long cdecl far VISMemSiz (VISMEMHDL mhMemHdl)
{
    return (_fmsize ((void far *) mhMemHdl));
}

VISMEMHDL   cdecl far VISMemAlo (unsigned long ulReqSiz)
{
    /********************************************************************/
    /* Note: Allocated memory must be at offset 0000                    */
    /********************************************************************/
    if (ulReqSiz > 0xffffL) return ((VISMEMHDL) NULL);
    return ((VISMEMHDL) _fmalloc ((int) ulReqSiz));
}

/************************************************************************/
/*              Global Memory Allocation Functions                      */
/************************************************************************/
VISMEMHDL   cdecl far VISMemAloChg (VISMEMHDL mhMemHdl, unsigned long ulReqSiz)
{
    /********************************************************************/
    /* Note: Allocated memory must be at offset 0000                    */
    /********************************************************************/
    if (ulReqSiz > 0xffffL) return ((VISMEMHDL) NULL);
    return ((VISMEMHDL) _frealloc ((void far *) mhMemHdl, (int) ulReqSiz));
}

VISMEMHDL   cdecl far VISMemAloRel (VISMEMHDL mhMemHdl)
{
    /********************************************************************/
    /* Note: MSC _fheapmin releases to OS as per Win Magazine 6/95.     */
    /********************************************************************/
    _ffree ((void far *) mhMemHdl);
    _fheapmin ();
    return ((VISMEMHDL) NULL);
}

void far *  cdecl far VISMemLck (VISMEMHDL mhMemHdl)
{
    return ((void far *) mhMemHdl);
}

VISMEMHDL   cdecl far VISMemLckRel (VISMEMHDL mhMemHdl)
{
    return ((VISMEMHDL) NULL);
}

/************************************************************************/
/*              Based Memory Allocation Functions                       */
/************************************************************************/
void far *  cdecl far VISBasAlo (void far *lpFarAdr, unsigned short usMemLen)
{
    /********************************************************************/
    /* Align memory to offset 0000                                      */
    /********************************************************************/
    unsigned short  usSegAdr = FP_SEG(lpFarAdr) + (FP_OFF(lpFarAdr) >> 4);
    unsigned short  usOffAdr = FP_OFF(lpFarAdr) & 0x000f;
    return (MK_FP (usSegAdr, usOffAdr));
}

void far *  cdecl far VISBasAloRel (void far *lpFarAdr)
{
    return (NULL);
}

/************************************************************************/
/*              File I/O Routines (reserved for future use)             */
/************************************************************************/
short cdecl far VISFilOpn (const char far *lpFilNam, unsigned short usOpnFlg)
{
    return (0);
}

short cdecl far VISFilCls (short sFilHdl)
{
    return (0);
}

long cdecl far  VISFilPos (short sFilHdl, long ulNewPos, unsigned short usPosFlg)
{
    return (0);
}

long cdecl far  VISFilLen (short sFilHdl)
{
    return (0);
}

/************************************************************************/
/*                      Console I/O Routines                            */
/************************************************************************/
int far pascal  VISPrtFmt (const char * pFmtLst, void * vaArgLst)
{
    return (vprintf (pFmtLst, vaArgLst));
}

int far pascal  VISScnFmt (const char * pFmtLst, void * vaArgLst)
{
    char    cRetChr;

    /********************************************************************/
    /********************************************************************/
    vprintf (pFmtLst, vaArgLst);
    scanf ("%c", &cRetChr);
    return (cRetChr);
}

