/************************************************************************/
/* Index Rebuild File Loader: FilLod.c                  V2.00  08/15/95 */
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

#include <string.h>                     /* String manipulation funcs    */
#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <stdio.h>                      /* Standard I/O                 */
#include <fcntl.h>                      /* Flags used in open/ sopen    */
#include <io.h>                         /* Low level file I/O           */
  
/************************************************************************/
/*                          External References                         */
/************************************************************************/
extern  TBXGLO  TBxGlo;

/************************************************************************/
/*                          Forward References                          */
/************************************************************************/
DWORD   RebSrcToD (short, short, LPSTR, WORD, VBIREC FAR *, WORD, RBIBLK FAR *);
DWORD   CopSrcVox (short, short, LPSTR, WORD, DWORD FAR *, DWORD FAR *);
DWORD   CopSrcTxt (short, short, LPSTR, WORD, DWORD FAR *);

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL RebIdxIni (RBIBLK FAR *lpRebBlk)
{
    _fmemset (lpRebBlk, 0, sizeof (*lpRebBlk));
    return (0);
}

DWORD FAR PASCAL RebIdxFil (short sSrcHdl, short sDstHdl, LPCSTR szSrcRsv, 
                 LPCSTR szDstRsv, LPSTR lpWrkBuf, WORD usBufSiz, RBIBLK FAR *lpRbIBlk)
{
    VISMEMHDL   hHdrMem;
    VISMEMHDL   hIdxMem;
    VBSHDR FAR *lpVBSHdr;
    VBIREC FAR *lpIdxRec;
    DWORD       ulDirLen;
    DWORD       ulBytCnt;
    unsigned    uiErrCod;    
    WORD        usi;

    /********************************************************************/
    /* Initialize poll callback display procedures                      */
    /********************************************************************/
    if (NULL == lpRbIBlk->lpPolDsp) lpRbIBlk->lpPolDsp = MsgDspPol;

    /********************************************************************/
    /* Load header and index entries                                    */
    /********************************************************************/
    if (0 != LodVBSHdr (sSrcHdl, &hHdrMem, &hIdxMem, FIOENCNON))
        return ((DWORD) -1);
    lpVBSHdr = (VBSHDR FAR *) GloMemLck (hHdrMem);
    if (lpVBSHdr->ulIdxTot > VBSMAXIDX) lpVBSHdr->ulIdxTot = VBSMAXIDX;

    /********************************************************************/
    /* Insert / Append / Delete index entries                           */
    /********************************************************************/
    if (lpRbIBlk->sShfCnt) {
        if (!lpRbIBlk->usAt_Pos) lpRbIBlk->usAt_Pos = 1;
        if (lpRbIBlk->sShfCnt < 0) 
            lpRbIBlk->usAt_Pos += abs (lpRbIBlk->sShfCnt);
        lpRbIBlk->usAt_Pos = 
            min (lpRbIBlk->usAt_Pos, (WORD) lpVBSHdr->ulIdxTot + 1); 
        if (-1 == ShfIdxBlk (lpVBSHdr, &hIdxMem, lpRbIBlk->usAt_Pos, 
            abs (lpRbIBlk->sShfCnt), lpRbIBlk->sShfCnt)) return ((DWORD) -1);
    }
    lpIdxRec = (VBIREC FAR *) GloMemLck (hIdxMem);
    ulDirLen = (DWORD) sizeof (VBSHDR) + (DWORD) lpVBSHdr->ulIdxTot * sizeof (VBIREC);   /* Bytes used (file)    */

    /********************************************************************/
    /* Initialize destination file position                             */
    /********************************************************************/
    if (-1L == FilSetPos (sDstHdl, ulDirLen, SEEK_SET)) return ((DWORD) -1L);

    /********************************************************************/
    /********************************************************************/
    lpVBSHdr->ulIdxUse = 0L;
    lpVBSHdr->ulBytUse = ulDirLen;
    for (usi=0; usi<lpVBSHdr->ulIdxTot; usi++) {
        ulBytCnt = RebSrcToD (sSrcHdl, sDstHdl, lpWrkBuf, usBufSiz, 
            &lpIdxRec[usi], usi + 1, lpRbIBlk);
        if (ulBytCnt) lpVBSHdr->ulIdxUse++;
        lpVBSHdr->ulBytUse += ulBytCnt;
        lpRbIBlk->lpPolDsp (NULL, (DWORD) usi, lpVBSHdr->ulIdxTot);
    }        
    if (lpRbIBlk->ulSmpFrq) lpVBSHdr->ulSmpFrq = lpRbIBlk->ulSmpFrq;

    /********************************************************************/
    /* Update destination file header & index records                   */
    /********************************************************************/
    if (-1L == FilSetPos (sDstHdl, 0, SEEK_SET)) return ((DWORD) -1L);
    if (-1L == Wr_FilFwd (sDstHdl, lpVBSHdr, sizeof (VBSHDR), 
        FIOENCNON, &uiErrCod)) return ((DWORD) -1L);
    if (-1L == Wr_FilFwd (sDstHdl, lpIdxRec, (WORD) (ulDirLen - sizeof (VBSHDR)), 
        FIOENCNON, &uiErrCod)) return ((DWORD) -1L);

    /********************************************************************/
    /********************************************************************/
    ulBytCnt = lpVBSHdr->ulBytUse;

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (hHdrMem);
    GloMemUnL (hIdxMem);
    RelVBSHdr (hHdrMem, hIdxMem);

    /********************************************************************/
    /********************************************************************/
    return (ulBytCnt);

}

DWORD   RebSrcToD (short sSrcHdl, short sDstHdl, LPSTR lpWrkBuf, WORD usBufSiz,
        VBIREC FAR *lpIdxRec, WORD usCurIdx, RBIBLK FAR *lpRbIBlk)
{
    DWORD   ulVoxCnt = 0L;
    DWORD   ulTxtCnt = 0L;
    short   sRepHdl;

    /********************************************************************/
    /* Merge vox from original sSrcFil or replacement file              */
    /********************************************************************/
    if ((usCurIdx != lpRbIBlk->usAt_Pos) || !(*lpRbIBlk->ucRepVox)) {
        if (lpIdxRec->ulVoxOff) ulVoxCnt = CopSrcVox (sSrcHdl, sDstHdl, 
            lpWrkBuf, usBufSiz, &lpIdxRec->ulVoxOff, &lpIdxRec->ulVoxLen);
    } else {
        if (-1 != (sRepHdl = FilHdlOpn (lpRbIBlk->ucRepVox, O_BINARY | O_RDONLY))) {
            lpIdxRec->ulVoxOff = 0L;
            lpIdxRec->ulVoxLen = FilGetLen (sRepHdl);
            ulVoxCnt = CopSrcVox (sRepHdl, sDstHdl, lpWrkBuf, usBufSiz,
                       &lpIdxRec->ulVoxOff, &lpIdxRec->ulVoxLen);
            FilHdlCls (sRepHdl);
        }
        else IDXERRMSG ("Cannot Open Source File: %s\n", lpRbIBlk->ucRepVox);
    }

    /********************************************************************/
    /* Merge text from original sSrcFil or replacement file             */
    /********************************************************************/
    if ((usCurIdx != lpRbIBlk->usAt_Pos) || !(*lpRbIBlk->ucRepTxt)) {
        if (lpIdxRec->ulTxtOff) ulTxtCnt = CopSrcTxt (sSrcHdl, sDstHdl, 
            lpWrkBuf, usBufSiz, &lpIdxRec->ulTxtOff);
    } else {
        if (-1 != (sRepHdl = FilHdlOpn (lpRbIBlk->ucRepTxt, O_BINARY | O_RDONLY))) {
            lpIdxRec->ulTxtOff = 0L;
            ulTxtCnt = CopSrcTxt (sRepHdl, sDstHdl, lpWrkBuf, usBufSiz,
                       &lpIdxRec->ulTxtOff);
            FilHdlCls (sRepHdl);
        }
        else IDXERRMSG ("Cannot Open Source File: %s\n", lpRbIBlk->ucRepTxt);
    }

    /********************************************************************/
    /********************************************************************/
    return (ulVoxCnt + ulTxtCnt);

}

DWORD   CopSrcVox (short sSrcHdl, short sDstHdl, LPSTR lpWrkBuf, WORD usBufSiz,
        DWORD FAR *pulVoxOff, DWORD FAR *pulVoxLen)
{
    unsigned    uiErrCod;    

    /********************************************************************/
    /* Copy Vox portion of file                                         */
    /********************************************************************/
    if ( (-1L != FilSetPos (sSrcHdl, *pulVoxOff, SEEK_SET))
      && (-1L != (*pulVoxOff = FilGetPos (sDstHdl)))
      && (-1L != (*pulVoxLen = FilCop (sSrcHdl, sDstHdl, lpWrkBuf, 
      usBufSiz, *pulVoxLen, FIOENCNON, &uiErrCod, NULL, 0L)))) {
        if (!*pulVoxLen) *pulVoxOff = 0L;
    }
    else *pulVoxOff = *pulVoxLen = 0L;

    /********************************************************************/
    /********************************************************************/
    return (*pulVoxLen);

}

DWORD   CopSrcTxt (short sSrcHdl, short sDstHdl, LPSTR lpWrkBuf, WORD usBufSiz,
        DWORD FAR *pulTxtOff)
{
    unsigned    uiErrCod;    
    WORD        usTxtLen;

    /********************************************************************/
    /* Copy Text portion of file                                        */
    /********************************************************************/
    if ( (-1L != FilSetPos (sSrcHdl, *pulTxtOff, SEEK_SET))
      && (-1L != (*pulTxtOff = FilGetPos (sDstHdl)))
      && (-1  != (usTxtLen = Rd_FilFwd (sSrcHdl, lpWrkBuf, 
      min (usBufSiz - 1, VBSTXTMAX - 1), FIOENCNON, &uiErrCod)))) {
        lpWrkBuf[usTxtLen] = '\0';
        usTxtLen = Wr_FilFwd (sDstHdl, lpWrkBuf, _fstrlen (lpWrkBuf) + 1, 
            FIOENCNON, &uiErrCod);
        if ((-1 == usTxtLen) || (0 == usTxtLen)) *pulTxtOff = 0L;
    }                
    else *pulTxtOff = usTxtLen = 0;

    /********************************************************************/
    /********************************************************************/
    return ((DWORD) usTxtLen);

}

