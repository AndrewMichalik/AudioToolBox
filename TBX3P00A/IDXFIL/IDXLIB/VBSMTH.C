/************************************************************************/
/* VFEdit VBase Index Math: VBSMth.c                    V2.00  07/15/94 */
/* Copyright (c) 1987-1994 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#if (defined (W31)) /****************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#include "..\..\os_dev\winmem.h"        /* Generic memory supp defs     */
#endif /*****************************************************************/
#if (defined (DOS)) /****************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "..\..\os_dev\dosmem.h"        /* Generic memory support defs  */
#endif /*****************************************************************/
#include "..\..\fiodev\genfio.h"        /* Generic File I/O definitions */
#include "..\..\fiodev\fiosup.h"        /* File I/O support definitions */
#include "..\..\fiodev\filutl.h"        /* File utilities definitions   */
#include "vbsfil.h"                     /* VBase file format defs       */

#include <string.h>                     /* String manipulation funcs    */
#include <stdlib.h>                     /* Misc string/math/error funcs */

/************************************************************************/
/*                      External References                             */
/************************************************************************/
extern  FIOGLO  FIOGlo;                 /* File I/O Library Globals     */
  
/************************************************************************/
/*                      Forward References                              */
/************************************************************************/
short   ShfIdxBlk (VBSHDR FAR *, VISMEMHDL FAR *, WORD, WORD, short);

/************************************************************************/
/************************************************************************/
FIORET  VBSIdxShf (VISMEMHDL mhHdrMem, VISMEMHDL FAR *pmhIdxMem,
        WORD usShfIdx, WORD usShfCnt, short sDirFlg)
{
    DWORD       ulShfPnt;
    DWORD       ulShfAmt;
    VBSHDR FAR *lpVBSHdr;

    /********************************************************************/
    /* Shift memory to expand/ delete required index points             */
    /********************************************************************/
    if (usShfCnt == 0) return (FIORETSUC);
    if ((usShfIdx == 0) || (sDirFlg == 0)) return (FIORETERR);

    /********************************************************************/
    /********************************************************************/
    if ((VBSHDR FAR *) NULL == (lpVBSHdr = (VBSHDR FAR *) GloMemLck (mhHdrMem)))
        return (FIORETERR);

    /********************************************************************/
    /* Insure that shift does not cause index to exceed maximum         */    
    /********************************************************************/
    if ((sDirFlg > 0) && ((lpVBSHdr->ulIdxTot + usShfCnt) > VBSMAXIDX)) {
        GloMemUnL (mhHdrMem);
        return (FIORETERR);
    }  

    /********************************************************************/
    /* Right Shift: Shift point appears at new position                 */
    /* Left  Shift: Shift point appears at new position, ulShfPnt       */
    /*              could be end of file!                               */
    /********************************************************************/
    ulShfPnt = (DWORD) sizeof(VBSHDR) + (DWORD) (sizeof (VBIREC) * (usShfIdx-1));
    ulShfAmt = (DWORD) (sizeof (VBIREC) * usShfCnt);
    if (ShfIdxBlk (lpVBSHdr, pmhIdxMem, usShfIdx, usShfCnt, sDirFlg)) {
        GloMemUnL (mhHdrMem);
        return (FIORETBAD);
    }

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (mhHdrMem);
    return (FIORETSUC);
}


short   VBSIdxCmp (VBSHDR FAR *lpVBSHdr, VBIREC FAR *lpIdxArr,
        DWORD ulShfPnt, DWORD ulShfAmt, short sDirFlg)
{
    WORD    usi;

    lpVBSHdr->ulIdxUse = 0L;

    /********************************************************************/
    /* Compute new VBase array values:                                  */
    /*      Recompute index offsets & lengths                           */
    /*      Update count of indexes used                                */
    /*      Update count of bytes used                                  */
    /*                                                                  */
    /* If the snippet file offset is >= shift point, move offset.       */
    /* For expansion, specific expanded snippet is unknown unless       */
    /*      file offset + length is > shift point.                      */
    /* For truncation, reduce size of indexes if (offset + length)      */
    /*      > (ulShfPnt - ulShfAmt) by intersection effected            */
    /*                                                                  */
    /* If the snippet file offset is >= shift point, move offset.       */
    /********************************************************************/
    if (sDirFlg >= 0) for (usi = 0; usi < (WORD) lpVBSHdr->ulIdxTot; usi++) {
        if (lpIdxArr[usi].ulVoxOff != 0L) {
            if (lpIdxArr[usi].ulVoxOff >= ulShfPnt) lpIdxArr[usi].ulVoxOff += ulShfAmt;    
            else if ((lpIdxArr[usi].ulVoxOff + lpIdxArr[usi].ulVoxLen) > ulShfPnt) 
                lpIdxArr[usi].ulVoxLen += ulShfAmt;
        } else lpIdxArr[usi].ulVoxLen = 0L;
        if ((lpIdxArr[usi].ulTxtOff != 0L) && (lpIdxArr[usi].ulTxtOff >= ulShfPnt))
            lpIdxArr[usi].ulTxtOff += ulShfAmt;
        if ((lpIdxArr[usi].ulVoxOff != 0L) || (lpIdxArr[usi].ulTxtOff != 0L))
            lpVBSHdr->ulIdxUse++;
    }
    else for (usi = 0; usi < (WORD) lpVBSHdr->ulIdxTot; usi++) {
        if (lpIdxArr[usi].ulVoxOff != 0L) {
            if (lpIdxArr[usi].ulVoxOff >= ulShfPnt) lpIdxArr[usi].ulVoxOff -= ulShfAmt;    
            else if ((lpIdxArr[usi].ulVoxOff + lpIdxArr[usi].ulVoxLen)
              > (ulShfPnt - ulShfAmt)) {
                DWORD   ulVoxDlt = min (ulShfPnt - lpIdxArr[usi].ulVoxOff, ulShfAmt);
                if (lpIdxArr[usi].ulVoxLen > ulVoxDlt) lpIdxArr[usi].ulVoxLen -= ulVoxDlt;
                else lpIdxArr[usi].ulVoxLen = 0L;
            }
        } else lpIdxArr[usi].ulVoxLen = 0L;
        if ((lpIdxArr[usi].ulTxtOff != 0L) && (lpIdxArr[usi].ulTxtOff >= ulShfPnt)) 
            lpIdxArr[usi].ulTxtOff -= ulShfAmt;
        if ((lpIdxArr[usi].ulVoxOff != 0L) || (lpIdxArr[usi].ulTxtOff != 0L))
            lpVBSHdr->ulIdxUse++;
    }

    /********************************************************************/
	/* Update bytes used field											*/
    /********************************************************************/
    if (sDirFlg >= 0) lpVBSHdr->ulBytUse = lpVBSHdr->ulBytUse + ulShfAmt;
        else lpVBSHdr->ulBytUse = lpVBSHdr->ulBytUse - ulShfAmt;
    
    /********************************************************************/
    /********************************************************************/
    return (0);

}

short   ShfIdxBlk (VBSHDR FAR *lpVBSHdr, VISMEMHDL FAR *pmhIdxMem, 
        WORD usShfIdx, WORD usShfCnt, short sDirFlg)
{
    VBIREC FAR *lpIdxRec;
    WORD        usArrIdx;

    /********************************************************************/
    /* Shift VBase header and index memory block to insert or           */
    /* delete index array entries:                                      */
    /*      Change the number of indexes                                */
    /*      Update the index in use count                               */
    /********************************************************************/
    
    /********************************************************************/
    /********************************************************************/
    if ((WORD) (usArrIdx = usShfIdx - 1) > (WORD) lpVBSHdr->ulIdxTot) return (-1);

    /********************************************************************/
    /* Shift Index Block to expand/ delete required index points        */
    /* Right Shift: Shift point appears at new position                 */
    /* Left  Shift: Shift point appears at new position, ulShfPnt       */
    /*              could be end of array!                              */
    /********************************************************************/
    if (sDirFlg >= 0) {
        /****************************************************************/
        /* Create Index                                                 */
        /****************************************************************/
        if ((VISMEMHDL) NULL == (*pmhIdxMem = GloReAMem (*pmhIdxMem, (DWORD) 
        sizeof (VBIREC) * (lpVBSHdr->ulIdxTot + (DWORD) usShfCnt)))) {
            return (-1);
        }
        if ((VBIREC FAR *) NULL == (lpIdxRec = (VBIREC FAR *)
            GloMemLck (*pmhIdxMem))) return (-1);

        /****************************************************************/
        /* Mem Copy (dst, src, cnt) - correct copy on overlap           */
        /****************************************************************/
        _fmemmove (&lpIdxRec[usArrIdx + usShfCnt], &lpIdxRec[usArrIdx],
            ((WORD) lpVBSHdr->ulIdxTot - usArrIdx) * sizeof (VBIREC));
        _fmemset ((LPSTR) &lpIdxRec[usArrIdx], 0x00, usShfCnt * sizeof (VBIREC));

        /****************************************************************/
        /* Update VBase header information                              */
        /****************************************************************/
        lpVBSHdr->ulIdxTot = lpVBSHdr->ulIdxTot + (DWORD) usShfCnt;
        GloMemUnL (*pmhIdxMem);

    }
    else {
        /****************************************************************/
        /* Delete Index                                                 */
        /****************************************************************/
        if (usShfCnt > usArrIdx) return (-1);
        if ((DWORD) usShfCnt > lpVBSHdr->ulIdxTot) return (-1);
        if ((VBIREC FAR *) NULL == (lpIdxRec = (VBIREC FAR *)
            GloMemLck (*pmhIdxMem))) return (-1);

        /****************************************************************/
        /* Mem Copy (dst, src, cnt) - correct copy on overlap           */
        /****************************************************************/
        _fmemmove (&lpIdxRec[usArrIdx - usShfCnt], &lpIdxRec[usArrIdx],
            ((WORD) lpVBSHdr->ulIdxTot - usArrIdx) * sizeof (VBIREC));

        /****************************************************************/
        /* Update VBase header information                              */
        /****************************************************************/
        lpVBSHdr->ulIdxTot = lpVBSHdr->ulIdxTot - (DWORD) usShfCnt;

        /****************************************************************/
        /****************************************************************/
        GloMemUnL (*pmhIdxMem);
        if ((VISMEMHDL) NULL == (*pmhIdxMem = GloReAMem (*pmhIdxMem, (DWORD)
            sizeof (VBIREC) * lpVBSHdr->ulIdxTot))) {
            return (-1);
        }

    }    

    /********************************************************************/
    /********************************************************************/
    return (0);

}



