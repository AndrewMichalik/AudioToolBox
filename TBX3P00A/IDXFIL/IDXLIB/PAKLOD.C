/************************************************************************/
/* Index Pack File Loader: FilLod.c                     V2.00  08/15/95 */
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
#include <conio.h>                      /* DOS low-level I/O            */
  
/************************************************************************/
/*                          External References                         */
/************************************************************************/
extern  TBXGLO  TBxGlo;

/************************************************************************/
/*                          Forward References                          */
/************************************************************************/
DWORD           PakSrcToD (char *, char *, short, VBIREC FAR *, PKICHKCBK, 
                PKIENDCBK, DWORD, LPVOID, WORD, IDXPOLPRC);
WORD FAR PASCAL PakPolCBk (FIOPOL, DWORD, DWORD);
int             FilSrtInc (const char **, const char **);
int             FilSrtDec (const char **, const char **);

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL PakIdxIni (PKIBLK FAR *lpPkIBlk)
{
    _fmemset (lpPkIBlk, 0, sizeof (*lpPkIBlk));
    return (0);
}

WORD FAR PASCAL PakIdxFil (LPSTR szVoxSpc, LPSTR szTxtSpc, LPSTR szDstSpc, 
                PKICHKCBK lpChkCBk, LPVOID lpRsv001, PKIENDCBK lpEndCBk, 
                DWORD ulCBkDat, PKIBLK FAR *lpPkIBlk)
{
    char        szTrgPth[_MAX_DRIVE + _MAX_DIR]; 
    char        szTrgNam[_MAX_FNAME + _MAX_EXT];
    char        szSrcFil[_MAX_PATH];
    LSTSPC      lsLstSpc;
    FNDSRTCBK   FndSrtCbk;
    VISMEMHDL   hHdrMem;
    VISMEMHDL   hIdxMem;
    VISMEMHDL   mhWrkBuf;
    VBSHDR FAR *lpVBSHdr;
    VBIREC FAR *lpIdxRec;
    LPVOID      lpWrkBuf;
    WORD        usBufSiz; 
    short       sDstHdl;
    DWORD       ulPakCnt;
    WORD        usIdxCnt;
    WORD        usIdxCur = 0;

    /********************************************************************/
    /********************************************************************/
    GetFulPth (szDstSpc, szTrgPth, szTrgNam);
    szDstSpc = strcat (szTrgPth, szTrgNam);

    /********************************************************************/
    /* Sort source file list if requested                               */
    /********************************************************************/
    FndSrtCbk = !lpPkIBlk->sSrtTyp ? 
        (FNDSRTCBK) NULL : ((lpPkIBlk->sSrtTyp > 0) ? FilSrtInc : FilSrtDec);
    if (0 == (usIdxCnt = GetFstFil (szVoxSpc, 0, szSrcFil, &lsLstSpc, 
        FndSrtCbk))) return ((WORD) -1);

    /********************************************************************/
    /* Allocate memory for indexed file headers                         */
    /********************************************************************/
    if (0 != IniVBSHdr (&hHdrMem, &hIdxMem, usIdxCnt, lpPkIBlk->ulSmpFrq))
        return ((WORD) -1);
    lpVBSHdr = (VBSHDR FAR *) GloMemLck (hHdrMem);
    lpIdxRec = (VBIREC FAR *) GloMemLck (hIdxMem);

    /********************************************************************/
    /* Allocate memory in multiples of 1024 and < 32Kbyte               */
    /* Allow 1024 bytes headroom                                        */
    /********************************************************************/
    usBufSiz = (WORD) min ((GloMemMax () / 1024L) * 1024L, 0x8000L);
    if (NULL == (lpWrkBuf = GloAloLck (GMEM_MOVEABLE, &mhWrkBuf, usBufSiz))) {
        GloMemUnL (hHdrMem);
        GloMemUnL (hIdxMem);
        RelVBSHdr (hHdrMem, hIdxMem);
        return ((WORD) -1);
    }

    /********************************************************************/
    /* Create destination file                                          */
    /********************************************************************/
    if (-1 == (sDstHdl = FilHdlCre (szDstSpc,
      O_BINARY | O_CREAT | O_TRUNC | O_WRONLY, S_IREAD | S_IWRITE))) {
        IDXERRMSG ("Cannot Open Target File: %Fs\n", (LPSTR) szDstSpc);
        return ((WORD) -1);
    }
    _chsize (sDstHdl, sizeof (VBSHDR) + (sizeof (VBIREC) * (WORD) lpVBSHdr->ulIdxTot));
    FilSetPos (sDstHdl, 0L, SEEK_END);

    /********************************************************************/
    /********************************************************************/
    do {
        ulPakCnt = PakSrcToD (szSrcFil, szTxtSpc, sDstHdl, &lpIdxRec[usIdxCur], 
            lpChkCBk, lpEndCBk, ulCBkDat, lpWrkBuf, usBufSiz, lpPkIBlk->lpPolDsp);
        if (ulPakCnt) {
            lpVBSHdr->ulIdxUse++;
            lpVBSHdr->ulBytUse += ulPakCnt;
        }
        usIdxCur++;
    } while (0 != GetNxtFil (szSrcFil, &lsLstSpc));

    /********************************************************************/
    /********************************************************************/
    FilSetPos (sDstHdl, 0, SEEK_SET);
    WrtVBSHdr (sDstHdl, lpVBSHdr, lpIdxRec, FIOENCNON);

    /********************************************************************/
    /********************************************************************/
    FilHdlCls (sDstHdl);
    EndFilLst (&lsLstSpc);

    /********************************************************************/
    /********************************************************************/
    mhWrkBuf = GloUnLRel (mhWrkBuf);
    GloMemUnL (hHdrMem);
    GloMemUnL (hIdxMem);
    RelVBSHdr (hHdrMem, hIdxMem);

    /********************************************************************/
    /********************************************************************/
    return (0);

}

/************************************************************************/
/************************************************************************/
DWORD   PakSrcToD (char *szVoxFil, char *szTxtSpc, short sDstHdl, 
        VBIREC FAR *lpIdxRec, PKICHKCBK lpChkCBk, PKIENDCBK lpEndCBk, 
        DWORD ulCBkDat, LPVOID lpWrkBuf, WORD usBufSiz, IDXPOLPRC lpPolDsp)
{
    short       sSrcHdl;
    char        szTxtFil[FIOMAXNAM];
    DWORD       ulTxtLen;
    BYTE        ucTrmChr = '\0';
    unsigned    uiErrCod;

    /********************************************************************/
    /********************************************************************/
    if ((NULL != lpChkCBk) && !lpChkCBk (szVoxFil, ulCBkDat)) return (0L);

    /********************************************************************/
    /********************************************************************/
    sSrcHdl = FilHdlOpn (szVoxFil, O_BINARY | O_RDONLY);
    if (sSrcHdl == -1) {
        IDXERRMSG ("Cannot Open Source File: %Fs\n", (LPSTR) szVoxFil);
        return (0L);
    }

    /********************************************************************/
    /* Copy Vox portion of file                                         */
    /********************************************************************/
    lpIdxRec->ulVoxOff = FilGetPos (sDstHdl);
    lpIdxRec->ulVoxLen = FilCop (sSrcHdl, sDstHdl, lpWrkBuf,
        usBufSiz, FilGetLen (sSrcHdl), FIOENCNON, &uiErrCod, 
        PakPolCBk, (DWORD) (LPVOID) lpPolDsp);
    FilHdlCls (sSrcHdl);

    /********************************************************************/
    /* Successful vox transfer?                                         */
    /********************************************************************/
    if (-1L == lpIdxRec->ulVoxLen) {
        lpIdxRec->ulVoxOff = lpIdxRec->ulVoxLen = 0L;
        return (0L);
    }
    if (!lpIdxRec->ulVoxLen) lpIdxRec->ulVoxOff = 0L;
    if ((NULL != lpEndCBk) && !lpEndCBk (lpIdxRec->ulVoxLen, ulCBkDat)) 
        return (lpIdxRec->ulVoxLen);
    
    /********************************************************************/
    /* Open Wildcard text file                                          */
    /********************************************************************/
    lpIdxRec->ulTxtOff = 0L;
    if (!strlen (szTxtSpc)) return (lpIdxRec->ulVoxLen);

    GetWldNam (szVoxFil, szTxtSpc, szTxtFil);
    if ((NULL != lpChkCBk) && !lpChkCBk (szTxtFil, ulCBkDat)) 
        return (lpIdxRec->ulVoxLen);
    if (-1 == (sSrcHdl = FilHdlOpn (szTxtFil, O_BINARY | O_RDONLY))) 
        return (lpIdxRec->ulVoxLen);
 
    /********************************************************************/
    /* Copy Text portion of file                                        */
    /********************************************************************/
    lpIdxRec->ulTxtOff = FilGetPos (sDstHdl);
    ulTxtLen = FilCop (sSrcHdl, sDstHdl, lpWrkBuf,
        usBufSiz, FilGetLen (sSrcHdl), FIOENCNON, &uiErrCod, 
        PakPolCBk, (DWORD) (LPVOID) lpPolDsp);
    FilHdlCls (sSrcHdl);

    /********************************************************************/
    /* Successful text transfer?                                        */
    /********************************************************************/
    if (-1L == ulTxtLen) {
        lpIdxRec->ulTxtOff = 0L;
        return (lpIdxRec->ulVoxLen);
    }
    ulTxtLen = ulTxtLen + (DWORD) Wr_FilFwd (sDstHdl, &ucTrmChr, 1, 
        FIOENCNON, &uiErrCod);
    if (NULL != lpEndCBk) lpEndCBk (ulTxtLen, ulCBkDat);

    /****************************************************************/
    /****************************************************************/
    return (lpIdxRec->ulVoxLen + ulTxtLen);

}

WORD FAR PASCAL PakPolCBk (FIOPOL usPolReq, DWORD ulFilPos, DWORD ulUsrPol)
{
    IDXPOLPRC lpPolDsp = (IDXPOLPRC) ulUsrPol;

    /********************************************************************/
    /********************************************************************/
    if (lpPolDsp) lpPolDsp (NULL, 0L, 0L);

    /********************************************************************/
    /********************************************************************/
    return (TRUE);

}
  
/************************************************************************/
/************************************************************************/
#if (defined (W31)) /****************************************************/
    #define MsgDspUsr
#endif /*****************************************************************/

int FilSrtInc (const char **pszFil__A, const char **pszFil__B)
{
    static  WORD ulCnt = 0;
    #define SRTDSPFRQ   500

    /********************************************************************/
    /********************************************************************/
    if (!ulCnt) MsgDspUsr ("  Sorting\r");
    if (!(++ulCnt % (SRTDSPFRQ/2))) {
        MsgDspUsr ((!(ulCnt % SRTDSPFRQ)) ? "\\" : "/");                       
        MsgDspUsr ("\r");      
    }
    return (_stricmp (*pszFil__A, *pszFil__B));
}

int FilSrtDec (const char **pszFil__A, const char **pszFil__B)
{
    static WORD ulCnt = 0;

    /********************************************************************/
    /********************************************************************/
    if (!ulCnt) MsgDspUsr ("  Sorting\r");
    if (!(++ulCnt % (SRTDSPFRQ/2))) {
        MsgDspUsr ((!(ulCnt % SRTDSPFRQ)) ? "\\" : "/");                       
        MsgDspUsr ("\r");      
    }
    return (_stricmp (*pszFil__B, *pszFil__A));
}

