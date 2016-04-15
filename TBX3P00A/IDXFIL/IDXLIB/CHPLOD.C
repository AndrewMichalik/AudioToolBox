/************************************************************************/
/* Index Chop File Loader: FilLod.c                     V2.00  08/15/95 */
/* Copyright (c) 1987-1995 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#if (defined (W31)) /****************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#include "..\..\os_dev\winmem.h"        /* Generic memory supp defs     */
#include "..\..\os_dev\winmsg.h"        /* User message support defs    */
#endif /*****************************************************************/
#if (defined (DOS)) /****************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "..\..\os_dev\dosmem.h"        /* Generic memory support defs  */
#include "..\..\os_dev\dosmsg.h"        /* User message support defs    */
#endif /*****************************************************************/
#include "..\..\fiodev\genfio.h"        /* Generic File I/O definitions */
#include "..\..\fiodev\filutl.h"        /* File utilities definitions   */
#include "vbsfil.h"                     /* File utilities definitions   */
#include "genidx.h"                     /* Indexed file defs            */
#include "libsup.h"                     /* Indexed file lib supp        */

#include <stdio.h>                      /* Standard I/O                 */
#include <io.h>                         /* Low level file I/O           */
#include <fcntl.h>                      /* Flags used in open/ sopen    */
#include <sys\types.h>                  /* File status and time types   */
#include <sys\stat.h>                   /* File status types            */
#include <string.h>                     /* String manipulation funcs    */
#include <stdlib.h>                     /* Misc string/math/error funcs */
  
/************************************************************************/
/*                          External References                         */
/************************************************************************/
extern  TBXGLO  TBxGlo;

/************************************************************************/
/************************************************************************/
DWORD           ChpSrcToD (short, char *, char *, LPSTR, WORD, VBIREC FAR *,
                DWORD, CHICHKCBK, CHIENDCBK, DWORD, CHIBLK *);
BOOL FAR PASCAL ChpPolCBk (FIOPOL, DWORD, DWORD);

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL ChpIdxIni (CHIBLK FAR *lpChpBlk)
{
    _fmemset (lpChpBlk, 0, sizeof (*lpChpBlk));
    return (0);
}

WORD FAR PASCAL ChpIdxFil (short sSrcHdl, LPSTR szVoxSpc, LPSTR szTxtSpc, 
                CHICHKCBK lpChkCBk, LPVOID lpRsv001, CHIENDCBK lpEndCBk, 
                DWORD ulCBkDat, CHIBLK FAR *lpChIBlk)
{
    VISMEMHDL   mhHdrMem;
    VISMEMHDL   mhIdxMem;
    VISMEMHDL   mhWrkBuf;
    VBSHDR FAR *lpVBSHdr;
    VBIREC FAR *lpIdxRec;
    LPVOID      lpWrkBuf;
    WORD        usBufSiz;
    WORD        usIdxCur;

    /********************************************************************/
    /* Allocate memory in multiples of 1024 and < 32Kbyte               */
    /* Allow 1024 bytes headroom                                        */
    /********************************************************************/
    usBufSiz = (WORD) min ((GloMemMax () / 1024L) * 1024L, 0x8000L);
    if (NULL == (lpWrkBuf = GloAloLck (GMEM_MOVEABLE, &mhWrkBuf, usBufSiz))) {
        return ((WORD) -1);
    }

    /********************************************************************/
    /* Load header and index entries                                    */
    /********************************************************************/
    if (0 != LodVBSHdr (sSrcHdl, &mhHdrMem, &mhIdxMem, FIOENCNON)) {
        GloUnLRel (mhWrkBuf);
        return ((WORD) -1);
    }
    lpVBSHdr = (VBSHDR FAR *) GloMemLck (mhHdrMem);
    lpIdxRec = (VBIREC FAR *) GloMemLck (mhIdxMem);
               
    /********************************************************************/
    /* Get index from / to array range                                  */                
    /********************************************************************/
    if (lpChIBlk->ulIdxPos) lpChIBlk->ulIdxPos--;
    if (lpChIBlk->ulIdxCnt) lpVBSHdr->ulIdxTot 
        = min (lpChIBlk->ulIdxPos + lpChIBlk->ulIdxCnt, lpVBSHdr->ulIdxTot);

    /********************************************************************/
    /* Extract index entries                                            */
    /********************************************************************/
    for (usIdxCur = (WORD) lpChIBlk->ulIdxPos; usIdxCur<lpVBSHdr->ulIdxTot; usIdxCur++) {
        ChpSrcToD (sSrcHdl, szVoxSpc, szTxtSpc, lpWrkBuf, usBufSiz, 
            &lpIdxRec[usIdxCur], max (0L, lpChIBlk->lNamOff + usIdxCur + 1L),
            lpChkCBk, lpEndCBk, ulCBkDat, lpChIBlk);
    }

    /********************************************************************/
    /********************************************************************/
    GloUnLRel (mhWrkBuf);
    GloMemUnL (mhHdrMem);
    GloMemUnL (mhIdxMem);
    RelVBSHdr (mhHdrMem, mhIdxMem);

    /********************************************************************/
    /********************************************************************/
    return (0);

}

/************************************************************************/
/************************************************************************/
DWORD   ChpSrcToD (short sSrcHdl, char *szVoxSpc, char *szTxtSpc, 
        LPSTR lpWrkBuf, WORD usBufSiz, VBIREC FAR * lpIdxRec, 
        DWORD ulNamNum, CHICHKCBK lpChkCBk, CHIENDCBK lpEndCBk, 
        DWORD ulCBkDat, CHIBLK *lpChIBlk)
{
    char        szDstPth[_MAX_DRIVE + _MAX_DIR]; 
    char        szDstNam[_MAX_FNAME + _MAX_EXT];
    char        szDstFil[_MAX_PATH];
    static char szTxtBuf[VBSTXTMAX];
    WORD        usBytCnt;
    DWORD       ulTxtLen;
    short       sDstHdl;
    WORD        usChr;
    unsigned    uiErrCod;

    /********************************************************************/
    /* Parse and open Vox output file                                   */
    /********************************************************************/
    GetFulPth (szVoxSpc, szDstPth, szDstNam);
    sprintf (szDstFil, "%s%04ld%s", szDstPth, ulNamNum, szDstNam);
    if ((NULL != lpChkCBk) && !lpChkCBk (szDstFil, ulCBkDat)) return (0L);

    /********************************************************************/
    /* Create destination vox file                                      */
    /********************************************************************/
    if (-1 == (sDstHdl = FilHdlCre (szDstFil,
        O_BINARY | O_CREAT | O_TRUNC | O_WRONLY, S_IREAD | S_IWRITE))) {
        IDXERRMSG ("Cannot Open Target File: %Fs\n", (LPSTR) szDstFil);
        return (0L);
    }

    /********************************************************************/
    /* Copy Vox portion of file                                         */
    /********************************************************************/
    FilSetPos (sSrcHdl, lpIdxRec->ulVoxOff, SEEK_SET);
    lpIdxRec->ulVoxLen = FilCop (sSrcHdl, sDstHdl, lpWrkBuf, usBufSiz, 
        lpIdxRec->ulVoxLen, FIOENCNON, &uiErrCod, ChpPolCBk, 
        (DWORD) (LPVOID) lpChIBlk->lpPolDsp);
    FilHdlCls (sDstHdl);
    if ((NULL != lpEndCBk) && !lpEndCBk (lpIdxRec->ulVoxLen, ulCBkDat)) 
        return (lpIdxRec->ulVoxLen);

    /********************************************************************/
    /* Parse and open Text output file                                  */
    /********************************************************************/
    if (0L == lpIdxRec->ulTxtOff) return (lpIdxRec->ulVoxLen);
    GetFulPth (szTxtSpc, szDstPth, szDstNam);
    sprintf (szDstFil, "%s%04ld%s", szDstPth, ulNamNum, szDstNam);
    if ((NULL != lpChkCBk) && !lpChkCBk (szDstFil, ulCBkDat)) 
        return (lpIdxRec->ulVoxLen);

    /********************************************************************/
    /* Create destination text file                                     */
    /********************************************************************/
    if (-1 == (sDstHdl = FilHdlCre (szDstFil,
        O_BINARY | O_CREAT | O_TRUNC | O_WRONLY, S_IREAD | S_IWRITE))) {
        IDXERRMSG ("Cannot Open Target File: %Fs\n", (LPSTR) szDstFil);
        return (lpIdxRec->ulVoxLen);
    }

    /********************************************************************/
    /* Copy Text portion of file                                        */
    /********************************************************************/
    FilSetPos (sSrcHdl, lpIdxRec->ulTxtOff, SEEK_SET);
    usBytCnt = Rd_FilFwd (sSrcHdl, szTxtBuf, VBSTXTMAX-1, FIOENCNON, &uiErrCod);
    if ((-1 == usBytCnt) || (0 == usBytCnt)) {
        FilHdlCls (sDstHdl);
        return (lpIdxRec->ulVoxLen);
    }

    /********************************************************************/
    /* Scan string for terminator; write to text file (sans terminator) */
    /********************************************************************/
    for (usChr=0; usChr<usBytCnt; usChr++) 
        if ('\0' == szTxtBuf[usChr]) break;
    ulTxtLen = (DWORD) Wr_FilFwd (sDstHdl, szTxtBuf, usChr, FIOENCNON, 
        &uiErrCod) + 1L;
    FilHdlCls (sDstHdl);
    if (NULL != lpEndCBk) lpEndCBk (ulTxtLen, ulCBkDat);

    /********************************************************************/
    /********************************************************************/
    return (lpIdxRec->ulVoxLen + ulTxtLen);

}

BOOL FAR PASCAL ChpPolCBk (FIOPOL usPolReq, DWORD ulFilPos, DWORD ulUsrPol)
{
    IDXPOLPRC lpPolDsp = (IDXPOLPRC) ulUsrPol;

    /********************************************************************/
    /********************************************************************/
    if (lpPolDsp) lpPolDsp (NULL, 0L, 0L);

    /********************************************************************/
    /********************************************************************/
    return (TRUE);

}
  

