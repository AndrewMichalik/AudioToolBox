/************************************************************************/
/* Generic Windows Memory Support: WinMem.c             V2.00  07/20/93 */
/* Copyright (c) 1987-1993 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#include "winmem.h"                     /* Generic memory supp defs     */
#include "winmsg.h"                     /* User message support         */

#include <string.h>                     /* String manipulation funcs    */

/************************************************************************/
/************************************************************************/
#define SI_LOCNOLMEM    "Cannot Lock Memory."
#define SI_LOCINSMEM    "Insufficient Memory."
#define GLOHDROOM       4096L

/************************************************************************/
/*              Local Memory Allocation Functions                       */
/************************************************************************/
VOID NEAR * LocAloLck (VISMEMHDL FAR *phLocMem, WORD usReqSiz)
{

    VOID NEAR * pcRetPtr;    
                
    /********************************************************************/
    /* Allocate local memory                                            */
    /********************************************************************/
    if (NULL == (*phLocMem = (VISMEMHDL) (WORD) LocalAlloc (LMEM_MOVEABLE, usReqSiz))) {
        MsgDspUsr (SI_LOCINSMEM);
        return ((VOID *) NULL);
    }
    if (NULL == (pcRetPtr = LocalLock ((HGLOBAL) *phLocMem))) {
        LocalFree ((HGLOBAL) *phLocMem);
        MsgDspUsr (SI_LOCNOLMEM);
        return ((VOID *) NULL);
    }

    /********************************************************************/
    /********************************************************************/
    return (pcRetPtr);

}

VISMEMHDL  LocUnLRel (VISMEMHDL hLocMem)
{
    LocalUnlock ((HGLOBAL) hLocMem);
    return ((VISMEMHDL) (WORD) LocalFree ((HGLOBAL) hLocMem));
}                       

/************************************************************************/
/*              Global Memory Information Functions                     */
/************************************************************************/
DWORD       GloMemMax ()
{
    DWORD       ulTotMem;                       /* Total Memory         */
    if ((ulTotMem = GlobalCompact (0L)) <= GLOHDROOM) return (0L);
    return (ulTotMem - GLOHDROOM);
}

DWORD       GloMemSiz (VISMEMHDL hgGloHdl)
{
    return (GlobalSize ((HGLOBAL) hgGloHdl));
}

/************************************************************************/
/*              Global Memory Allocation Functions                      */
/************************************************************************/
VISMEMHDL   GloAloMem (WORD usAloFlg, DWORD ulReqSiz) 
{
    VISMEMHDL   hgGloMem;

    /********************************************************************/
    /********************************************************************/
    if (NULL == (hgGloMem = (VISMEMHDL) (WORD) GlobalAlloc (usAloFlg, ulReqSiz))) {
        MsgDspUsr (SI_LOCINSMEM);
        return (NULL);
    }
    return (hgGloMem);

}

VISMEMHDL   GloReAMem (VISMEMHDL mhMemHdl, DWORD ulReqSiz) 
{
    if (!mhMemHdl) return (GloAloMem (GMEM_MOVEABLE, ulReqSiz));
    else return ((VISMEMHDL) (WORD) GlobalReAlloc ((HGLOBAL) mhMemHdl, ulReqSiz, GMEM_MOVEABLE));
}

VISMEMHDL   GloAloBlk (WORD usAloFlg, DWORD ulBlkSiz, DWORD ulMinCnt, 
            DWORD ulMaxCnt, DWORD FAR *pulAllSiz)
{
    VISMEMHDL   hgGloMem;
    DWORD       ulLrgBlk;                       /* Largest contig block */
    DWORD       uli;

    /********************************************************************/
    /********************************************************************/
    if ((ulBlkSiz == 0L) || (ulMinCnt == 0L) || (ulMinCnt > ulMaxCnt)) return (NULL);

    /********************************************************************/
    /* Check total memory available in largest block; use smaller       */
    /* request if needed to allow GLOHDROOM bytes headroom.             */
    /* Compute uli for: Min Mem <= (mem = i * ulBlkSiz) <= Avail Mem    */
    /********************************************************************/
    if ((ulLrgBlk = GlobalCompact (ulBlkSiz * ulMaxCnt)) <= GLOHDROOM) {
        MsgDspUsr (SI_LOCINSMEM);
        return (NULL);
    }
    ulLrgBlk -= GLOHDROOM;

    /********************************************************************/
    /* Insure that minimum allocation is available                      */
    /********************************************************************/
    if ((uli = ulLrgBlk/ulBlkSiz) < ulMinCnt) {
        MsgDspUsr (SI_LOCINSMEM);
        return (NULL);
    }
    if (uli > ulMaxCnt) uli = ulMaxCnt;

    /********************************************************************/
    /* Since brain-dead Windows reports available memory incorrectly,   */
    /* try to allocate requested.                                       */
    /********************************************************************/
    while (NULL == (hgGloMem = GlobalAlloc (usAloFlg, uli * ulBlkSiz))) {
        /****************************************************************/
        /* Reduce allocation request until successful or below minimum  */
        /****************************************************************/
        if ((uli = (DWORD) (uli * .80)) < ulMinCnt) {
            MsgDspUsr (SI_LOCINSMEM); 
            return (NULL);
        }
    }

    /********************************************************************/
    /* Trim extra if allocation is larger do to paragraph alignment     */
    /********************************************************************/
    *pulAllSiz = (GloMemSiz (hgGloMem) / ulBlkSiz) * ulBlkSiz;
    return (hgGloMem);
  
}

LPVOID  GloAloLck (WORD usAloFlg, VISMEMHDL FAR *phgGloHdl, DWORD ulBufSiz)
{
    LPVOID  lpMemPtr;

    /********************************************************************/
    /********************************************************************/
    *phgGloHdl = GloAloMem (usAloFlg, ulBufSiz);
    if ((VISMEMHDL) NULL == *phgGloHdl) {
        MsgDspUsr (SI_LOCINSMEM); 
        return (NULL);
    }
    lpMemPtr = GloMemLck (*phgGloHdl);
    if (NULL == lpMemPtr) {
        *phgGloHdl = (VISMEMHDL) GloAloRel (*phgGloHdl);
        MsgDspUsr (SI_LOCNOLMEM);
        return (NULL);
    }

    /********************************************************************/
    /********************************************************************/
    return (lpMemPtr);

}

LPVOID  GloReALck (VISMEMHDL FAR *pmhMemHdl, DWORD ulReqSiz)
{
    DWORD       ulOrgSiz;
    LPVOID      lpMemPtr;
    VISMEMHDL   mhNewHdl;

    /********************************************************************/
    /********************************************************************/
    ulOrgSiz = GloMemSiz (*pmhMemHdl);

    /********************************************************************/
    /********************************************************************/
    mhNewHdl = (VISMEMHDL) GloReAMem (*pmhMemHdl, ulReqSiz);
    if ((VISMEMHDL) NULL == mhNewHdl) return (NULL);
    *pmhMemHdl = mhNewHdl;

    /********************************************************************/
    /********************************************************************/
    lpMemPtr = GloMemLck (*pmhMemHdl);
    if (NULL == lpMemPtr) {
        *pmhMemHdl = (VISMEMHDL) GloReAMem (*pmhMemHdl, ulOrgSiz);
        return (NULL);
    }

    /********************************************************************/
    /********************************************************************/
    return (lpMemPtr);

}

LPVOID  GloDupLck (WORD usAloFlg, VISMEMHDL FAR *phSrcMem)
{
    VISMEMHDL   hgDstMem;
    WORD        usMemSiz;
    LPVOID      lpSrcBlk;
    LPVOID      lpDstBlk;

    /********************************************************************/
    /* Allocate and duplicate and a chunk of global memory < 64K        */
    /********************************************************************/
    if (NULL == *phSrcMem) return ((LPVOID) NULL);

    /********************************************************************/
    /* Allocate a new memory block with the same size & flag settings   */
    /********************************************************************/
    usMemSiz = (WORD) GloMemSiz (*phSrcMem);
    if (NULL == (hgDstMem = GloAloMem (usAloFlg, (DWORD) usMemSiz))) {
        MsgDspUsr (SI_LOCINSMEM); 
        return ((LPVOID) NULL);  
    }

    /********************************************************************/
    /********************************************************************/
    if ((LPVOID) NULL == (lpSrcBlk = GloMemLck (*phSrcMem))) {
        GloAloRel (hgDstMem);
        MsgDspUsr (SI_LOCNOLMEM); 
        return ((LPVOID) NULL);
    }
    if ((LPVOID) NULL == (lpDstBlk = GloMemLck (hgDstMem))) {
        GloAloRel (hgDstMem);
        GloMemUnL (*phSrcMem);
        MsgDspUsr (SI_LOCNOLMEM); 
        return ((LPVOID) NULL);
    }
    _fmemcpy ((LPSTR) lpDstBlk, lpSrcBlk, usMemSiz);

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (*phSrcMem);
    *phSrcMem = hgDstMem;
    return (lpDstBlk);

}

LPVOID  GloMemLck (VISMEMHDL hgGloHdl)
{
    LPSTR   lpRetPtr;    

    /********************************************************************/
    /********************************************************************/
    if (NULL == hgGloHdl) return ((LPVOID) NULL);
    if ((LPSTR) NULL == (lpRetPtr = GlobalLock ((HGLOBAL) hgGloHdl))) {
        MsgDspUsr (SI_LOCNOLMEM);  
        return ((LPVOID) NULL);
    }
    return ((LPVOID) lpRetPtr);

}

VISMEMHDL   GloMemUnL (VISMEMHDL hgGloHdl) 
{
    if (NULL == hgGloHdl) return (NULL);
    return (GlobalUnlock ((HGLOBAL) hgGloHdl) ? hgGloHdl : NULL);
}

VISMEMHDL   GloUnLRel (VISMEMHDL hgGloHdl)
{
    /********************************************************************/
    /********************************************************************/
    if ((VISMEMHDL) NULL != hgGloHdl) {
        GloMemUnL (hgGloHdl);
        return (GloAloRel (hgGloHdl));
    }

    /********************************************************************/
    /********************************************************************/
    return ((VISMEMHDL) NULL);

}

VISMEMHDL   GloAloRel (VISMEMHDL hgGloHdl) 
{
    if ((VISMEMHDL) NULL != hgGloHdl) 
        return ((VISMEMHDL) (WORD) GlobalFree ((HGLOBAL) hgGloHdl));
    return ((VISMEMHDL) NULL);
}

/************************************************************************/
/*              Based Memory Allocation Functions                       */
/************************************************************************/
LPVOID  BasAdrAlo (LPVOID lpFarAdr, WORD usMemLen)
{
    WORD    usMemSel;

    /********************************************************************/
    /********************************************************************/
    if (!(usMemSel = AllocSelector (SELECTOROF(lpFarAdr)))) return (NULL);

    /********************************************************************/
    /********************************************************************/
    SetSelectorBase  (usMemSel, 
        GetSelectorBase (SELECTOROF(lpFarAdr)) + OFFSETOF(lpFarAdr));
    SetSelectorLimit (usMemSel, usMemLen);

    /********************************************************************/
    /********************************************************************/
    return ((void FAR *) ((DWORD) usMemSel << 16));

}

LPVOID      BasAdrRel (LPVOID lpFarAdr)
{
    if (lpFarAdr) FreeSelector (SELECTOROF (lpFarAdr));
    return (NULL);
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