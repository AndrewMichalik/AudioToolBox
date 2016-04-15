/************************************************************************/
/* Index List File Loader: FilLod.c                     V2.00  08/15/95 */
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
/*                          Forward References                          */
/************************************************************************/
DWORD   LstSrcVox (short, LPSTR, WORD, DWORD, DWORD, IDXERRPRC);
DWORD   LstSrcTxt (short, LPSTR, WORD, DWORD, IDXERRPRC);

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL LstIdxIni (LSIBLK FAR *lpLstBlk)
{
    _fmemset (lpLstBlk, 0, sizeof (*lpLstBlk));
    return (0);
}

DWORD FAR PASCAL LstIdxFil (short sSrcHdl, LPCSTR szSrcRsv, LPSTR lpWrkBuf, 
                 WORD usBufSiz, LSIBLK FAR *lpLstBlk)
{
    VISMEMHDL   hHdrMem;
    VISMEMHDL   hIdxMem;
    VBSHDR FAR *lpVBSHdr;
    VBIREC FAR *lpIdxRec;
    WORD        usi;

    /********************************************************************/
    /* Initialize default message display routine                       */
    /********************************************************************/
    if (!lpLstBlk->lpMsgDsp) lpLstBlk->lpMsgDsp = MsgDspUsr;

    /********************************************************************/
    /* Load header and index entries                                    */
    /********************************************************************/
    if (0 != LodVBSHdr (sSrcHdl, &hHdrMem, &hIdxMem, FIOENCNON))
        return ((DWORD) -1);
    lpVBSHdr = (VBSHDR FAR *) GloMemLck (hHdrMem);
    lpIdxRec = (VBIREC FAR *) GloMemLck (hIdxMem);

    /********************************************************************/
    /* List general header information (if requested)                   */
    /********************************************************************/
    if (!(lpLstBlk->usLstFlg & GENINHFLG)) {
        lpLstBlk->lpMsgDsp ("-------------------------------------------------------------------- General\n");
        lpLstBlk->lpMsgDsp ("Maximum prompt count: %ld\n", lpVBSHdr->ulIdxTot);
        lpLstBlk->lpMsgDsp ("Sample Frequency: %ld\n", lpVBSHdr->ulSmpFrq);
        lpLstBlk->lpMsgDsp ("Active Indexes: %ld\n", lpVBSHdr->ulIdxUse);
        lpLstBlk->lpMsgDsp ("Reserved #1: %ld\n", lpVBSHdr->uldummya);
        lpLstBlk->lpMsgDsp ("Bytes used: %ld\n", lpVBSHdr->ulBytUse);
        lpLstBlk->lpMsgDsp ("Reserved #2: %ld\n", lpVBSHdr->uldummyb);
    }

    /********************************************************************/
    /********************************************************************/
    if (!(lpLstBlk->usLstFlg & VOXINHFLG) || !(lpLstBlk->usLstFlg & TXTINHFLG)) {
        for (usi=0; usi<lpVBSHdr->ulIdxTot; usi++) {
            lpLstBlk->lpMsgDsp ("----------------------------------------------------------------------- %4d\n", usi+1);
            if (!(lpLstBlk->usLstFlg & VOXINHFLG)) LstSrcVox (sSrcHdl,  
                lpWrkBuf, usBufSiz, lpIdxRec[usi].ulVoxOff, 
                lpIdxRec[usi].ulVoxLen, lpLstBlk->lpMsgDsp);
            if (!(lpLstBlk->usLstFlg & TXTINHFLG)) LstSrcTxt (sSrcHdl,  
                lpWrkBuf, usBufSiz, lpIdxRec[usi].ulTxtOff, lpLstBlk->lpMsgDsp);
        }        
    }        
    lpLstBlk->lpMsgDsp ("----------------------------------------------------------------------------\n\n");

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (hHdrMem);
    GloMemUnL (hIdxMem);
    RelVBSHdr (hHdrMem, hIdxMem);

    /********************************************************************/
    /********************************************************************/
    return (0L);

}

DWORD   LstSrcVox (short sSrcHdl, LPSTR lpWrkBuf, WORD usBufSiz,
        DWORD ulVoxOff, DWORD ulVoxLen, IDXERRPRC lpMsgDsp)
{
    /********************************************************************/
    /* List Vox portion of file                                         */
    /********************************************************************/
    if (ulVoxOff) 
         lpMsgDsp ("Vox Offset: %ld, Length: %ld\n", ulVoxOff, ulVoxLen);
    else lpMsgDsp ("Vox Inactive\n");

    /********************************************************************/
    /********************************************************************/
    return (0L);

}

DWORD   LstSrcTxt (short sSrcHdl, LPSTR lpWrkBuf, WORD usBufSiz,
        DWORD ulTxtOff, IDXERRPRC lpMsgDsp)
{
    unsigned    uiErrCod;    
    WORD        usTxtLen;

    /********************************************************************/
    /* Copy Text portion of file                                        */
    /********************************************************************/
    if (ulTxtOff 
      && (-1L != _lseek (sSrcHdl, ulTxtOff, SEEK_SET))
      && (-1L != (ulTxtOff = _tell (sSrcHdl)))
      && (-1  != (usTxtLen = Rd_FilFwd (sSrcHdl, lpWrkBuf, 
      min (usBufSiz - 1, VBSTXTMAX - 1), FIOENCNON, &uiErrCod)))) {
        lpWrkBuf[usTxtLen] = '\0';
        lpMsgDsp ("%Fs\n", (LPSTR) lpWrkBuf);
    }                
    else lpMsgDsp ("Txt Inactive\n");

    /********************************************************************/
    /********************************************************************/
    return (0L);

}
