/************************************************************************/
/* Generic DOS Memory Support: DOSMem.c                 V2.00  08/15/93 */
/* Copyright (c) 1991-1992 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "dosmem.h"                     /* Generic memory support defs  */
#include "..\usrdev\usrstb.h"           /* User supplied I/O stubs      */

#include <stdlib.h>                     /* Standard library defs        */
#include <string.h>                     /* String manipulation funcs    */

/************************************************************************/
/************************************************************************/
#define GLOHDROOM       4096L

/************************************************************************/
/*              Global Memory Information Functions                     */
/************************************************************************/
DWORD       GloMemMax ()
{
    return (VISMemMax ());
}

DWORD       GloMemSiz (VISMEMHDL hgGloHdl)
{
    return (VISMemSiz (hgGloHdl));
}

/************************************************************************/
/*              Global Memory Allocation Functions                      */
/************************************************************************/
VISMEMHDL   GloAloMem (WORD usAloFlg, DWORD ulReqSiz) 
{
// Warning msg if fail?
// Note: Will generate invalid warnings for routines that re-try allocation
    return (VISMemAlo (ulReqSiz));
}

VISMEMHDL   GloReAMem (VISMEMHDL hgGloHdl, DWORD ulReqSiz) 
{
    return ((VISMEMHDL) VISMemAloChg (hgGloHdl, ulReqSiz));
}

VISMEMHDL   GloAloBlk (WORD usAloFlg, DWORD ulBlkSiz, DWORD ulMinCnt, 
            DWORD ulMaxCnt, DWORD FAR *pulAllSiz)
{           
    VISMEMHDL   hgGloMem;
    DWORD       ulTotMem;                       /* Total Memory         */
    DWORD       ulLrgBlk;                       /* Largest contig block */
    DWORD       uli;

    /********************************************************************/
    /********************************************************************/
    if ((ulBlkSiz == 0L) || (ulMinCnt == 0L) || (ulMinCnt > ulMaxCnt)) 
        return ((VISMEMHDL) NULL);

    /********************************************************************/
    /* Check total memory available and largest block, use smaller      */
    /* request if needed to allow GLOHDROOM bytes headroom.             */
    /* Compute uli for: Min Mem <= (mem = i * ulBlkSiz) <= Avail Mem    */
    /********************************************************************/
    if ((ulTotMem = GloMemMax()) <= GLOHDROOM) {
// Warning msg if fail?
        return ((VISMEMHDL) NULL);
    }
    if ((ulLrgBlk = GloMemMax()) <= GLOHDROOM) {
// Warning msg if fail?
        return ((VISMEMHDL) NULL);
    }

    if ((ulTotMem - GLOHDROOM) < ulLrgBlk) ulLrgBlk = ulTotMem - GLOHDROOM;
    if ((uli = ulLrgBlk/ulBlkSiz) < ulMinCnt) {
// Warning msg if fail?
        return ((VISMEMHDL) NULL);
    }

    if (uli > ulMaxCnt) uli = ulMaxCnt;

    /********************************************************************/
    /********************************************************************/
    if ((VISMEMHDL) NULL == (hgGloMem = GloAloMem (usAloFlg, uli * ulBlkSiz))) {
// Warning msg if fail?
        return ((VISMEMHDL) NULL);
    }

    /********************************************************************/
    /* Trim extra if allocation is larger do to paragraph alignment     */
    /********************************************************************/
    *pulAllSiz = (GloMemSiz (hgGloMem) / ulBlkSiz) * ulBlkSiz;
    return (hgGloMem);
  
}

LPVOID  GloAloLck (WORD usAloFlg, VISMEMHDL FAR *phgGloHdl, DWORD ulReqSiz)
{
    LPVOID  lpMemPtr;

    /********************************************************************/
    /********************************************************************/
    *phgGloHdl = (VISMEMHDL) GloAloMem (usAloFlg, ulReqSiz);
    if ((VISMEMHDL) NULL == *phgGloHdl) {
// Warning msg if fail?
        return (NULL);
    }
    lpMemPtr = GloMemLck (*phgGloHdl);
    if (NULL == lpMemPtr) {
        *phgGloHdl = (VISMEMHDL) GloAloRel (*phgGloHdl);
// Warning msg if fail?
        return (NULL);
    }

    /********************************************************************/
    /********************************************************************/
    return (lpMemPtr);

}

LPVOID  GloReALck (VISMEMHDL FAR *phgGloHdl, DWORD ulReqSiz)
{
    DWORD       ulOrgSiz;
    LPVOID      lpMemPtr;
    VISMEMHDL   mhNewHdl;

    /********************************************************************/
    /********************************************************************/
    ulOrgSiz = GloMemSiz (*phgGloHdl);

    /********************************************************************/
    /********************************************************************/
    mhNewHdl = (VISMEMHDL) GloReAMem (*phgGloHdl, ulReqSiz);
    if ((VISMEMHDL) NULL == mhNewHdl) return (NULL);
    *phgGloHdl = mhNewHdl;

    /********************************************************************/
    /********************************************************************/
    lpMemPtr = GloMemLck (*phgGloHdl);
    if (NULL == lpMemPtr) {
        *phgGloHdl = (VISMEMHDL) GloReAMem (*phgGloHdl, ulOrgSiz);
        return (NULL);
    }

    /********************************************************************/
    /********************************************************************/
    return (lpMemPtr);

}

LPVOID  GloMemLck (VISMEMHDL hgGloHdl)
{
    return (VISMemLck (hgGloHdl));
}

VISMEMHDL   GloMemUnL (VISMEMHDL hgGloHdl) 
{
    return (VISMemLckRel (hgGloHdl));
}

VISMEMHDL   GloUnLRel (VISMEMHDL hgGloHdl)
{

    /********************************************************************/
    /********************************************************************/
    if ((VISMEMHDL) NULL != hgGloHdl) {
        GloMemUnL (hgGloHdl);
        GloAloRel (hgGloHdl);
    }

    /********************************************************************/
    /********************************************************************/
    return ((VISMEMHDL) NULL);

}

VISMEMHDL   GloAloRel (VISMEMHDL hgGloHdl) 
{
    return (((VISMEMHDL) NULL != hgGloHdl) ? VISMemAloRel (hgGloHdl) : 0L);
}

/************************************************************************/
/*              Based Memory Allocation Functions                       */
/************************************************************************/
LPVOID  BasAdrAlo (LPVOID lpFarAdr, WORD usMemLen)
{
    return (VISBasAlo (lpFarAdr, usMemLen));
}

LPVOID  BasAdrRel (LPVOID lpFarAdr)
{
    return (VISBasAloRel (lpFarAdr));
}

/************************************************************************/
/*                          Utility Functions                           */
/************************************************************************/
void huge * hmemmove (void huge *lpDstBlk, void huge *lpSrcBlk, DWORD ulBytCnt)
{
    DWORD       ulBlkSiz;
    #define     BLKSIZMAX 0x0400L       /* 1Kb - no selector overlap    */

    /********************************************************************/
    /* hmemmove: Copy (dst, src, cnt) - correct copy on overlap         */
    /********************************************************************/
    if (lpDstBlk == lpSrcBlk) return (lpDstBlk);
    if (lpDstBlk > lpSrcBlk) while (ulBytCnt) {             /* Reverse  */
        ulBlkSiz = min (ulBytCnt, BLKSIZMAX);        
        ulBytCnt -= ulBlkSiz;
        _fmemmove (((char huge *)lpDstBlk)+ulBytCnt, 
            ((char huge *)lpSrcBlk)+ulBytCnt, (WORD) ulBlkSiz);
    }
    else while (ulBytCnt) {
        ulBlkSiz = min (ulBytCnt, BLKSIZMAX);        
        _fmemmove (((char huge *)lpDstBlk)+ulBytCnt, 
            ((char huge *)lpSrcBlk)+ulBytCnt, (WORD) ulBlkSiz);
        ulBytCnt -= ulBlkSiz;
    }
    /********************************************************************/
    /********************************************************************/
    return (lpDstBlk);

}

