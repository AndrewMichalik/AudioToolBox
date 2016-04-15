/************************************************************************/
/* Generic Windows Memory Support Defs: WinMem.h        V2.00  07/20/93 */
/* Copyright (c) 1987-1994 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define MEMVERNUM       0x0100          /* Memory support ver number    */

/************************************************************************/
/************************************************************************/
typedef unsigned long   VISMEMHDL;      /* VIS general purpose mem hdl  */

/************************************************************************/
/*                  Memory Allocation Functions                         */
/************************************************************************/
VOID NEAR * LocAloLck (VISMEMHDL FAR *, WORD);
VISMEMHDL   LocUnLRel (VISMEMHDL);

DWORD       GloMemMax ();
DWORD       GloMemSiz (VISMEMHDL);
VISMEMHDL   GloAloMem (WORD, DWORD);
VISMEMHDL   GloReAMem (VISMEMHDL, DWORD); 
VISMEMHDL   GloAloBlk (WORD, DWORD, DWORD, DWORD, DWORD FAR *);
LPVOID      GloAloLck (WORD, VISMEMHDL FAR *, DWORD);
LPVOID      GloReALck (VISMEMHDL FAR *, DWORD);
LPVOID      GloDupLck (WORD, VISMEMHDL FAR *); 
LPVOID      GloMemLck (VISMEMHDL);
VISMEMHDL   GloMemUnL (VISMEMHDL); 
VISMEMHDL   GloUnLRel (VISMEMHDL); 
VISMEMHDL   GloAloRel (VISMEMHDL); 

LPVOID      BasAdrAlo (LPVOID, WORD);
LPVOID      BasAdrRel (LPVOID);

VOID huge * hmemmove (VOID huge *, VOID huge *, DWORD);
