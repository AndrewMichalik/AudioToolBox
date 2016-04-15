/************************************************************************/
/* ToolBox VBase File Routines (DOS): VBSFil.c          V2.00  09/10/92 */
/* Copyright (c) 1987-1992 Andrew J. Michalik                           */
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
#include "vbsfil.h"                     /* VBase file format defs       */

#include <string.h>                     /* String manipulation funcs    */
#include <stdio.h>                      /* Standard I/O                 */
#include <io.h>                         /* Low level file I/O           */
#include <errno.h>                      /* errno value constants        */
  
/************************************************************************/
/************************************************************************/
#define SI_FIOINVFMT   "Invalid File Format\n"     
#define SI_FIOLCKORO   "File Locked or Read-only\n"
#define SI_FIOINVHDL   "Invalid File Handle\n"     
#define SI_LOCINSMEM   "Insufficient Memory\n"     
#define SI_FIOINSSPC   "Insufficient Disk Space\n"
#define SI_FIONOOWRT   "Cannot Overwrite File\n"

/************************************************************************/
/*                  VBase File Load Support Routines                    */
/************************************************************************/
WORD FAR PASCAL IniVBSHdr (VISMEMHDL FAR *lphHdrMem, VISMEMHDL FAR *lphIdxMem,
                WORD usIdxCnt, DWORD ulSmpFrq)
{
    VBSHDR FAR *lpVBSHdr;
    VBIREC FAR *lpIdxRec;
                
    /********************************************************************/
    /* Allocate global memory for file header                           */
    /********************************************************************/
    lpVBSHdr = (VBSHDR FAR *) GloAloLck (GMEM_MOVEABLE, lphHdrMem, sizeof (VBSHDR)); 
    if ((VBSHDR FAR *) NULL == lpVBSHdr) {
        MsgDspUsr (SI_LOCINSMEM);                   /* Insufficient Mem */
        return ((WORD) -1);
    }

    /********************************************************************/
    /********************************************************************/
    lpVBSHdr->ulIdxTot = (DWORD) usIdxCnt;      /* Prompt count max     */
    lpVBSHdr->ulSmpFrq = ulSmpFrq;              /* Sample frequency     */
    lpVBSHdr->ulIdxUse = 0L;                    /* Prompt count actv    */
    lpVBSHdr->uldummya = 0L;                    /* Dummy                */
    lpVBSHdr->ulBytUse = (DWORD) sizeof (VBSHDR)
        + (DWORD) usIdxCnt * sizeof (VBIREC);   /* Bytes used (file)    */
    lpVBSHdr->uldummyb = 0L;                    /* Dummy                */

    GloMemUnL (*lphHdrMem);

    /********************************************************************/
    /* Allocate mem for whole array in multiples of sizeof (VBIREC)     */
    /********************************************************************/
    lpIdxRec = (VBIREC FAR *) GloAloLck (GMEM_MOVEABLE, lphIdxMem, (sizeof (VBIREC) * (WORD) usIdxCnt));
    if ((VBIREC FAR *) NULL == lpIdxRec) {
        MsgDspUsr (SI_LOCINSMEM);                   /* Insufficient Mem */
        return ((WORD) -1);
    }
    _fmemset ((LPSTR) lpIdxRec, 0x00, sizeof (VBIREC) * usIdxCnt);

    GloMemUnL (*lphIdxMem);

    /********************************************************************/
    /********************************************************************/
    return (0);

}

FIORET FAR PASCAL LodVBSHdr (short sSrcHdl, VISMEMHDL FAR *lphHdrMem, 
                  VISMEMHDL FAR *lphIdxMem, WORD usEncFmt)
{

    DWORD       ulFilLen;
    VBSHDR FAR *lpVBSHdr;
    VBIREC FAR *lpIdxRec;
    DWORD       ulIdxCnt;
    DWORD       ulMinLen;
    unsigned    uiErrCod;

    /********************************************************************/
    /********************************************************************/
    if ((long) (ulFilLen = FilGetLen (sSrcHdl)) < (long) sizeof (VBSHDR)) {
        MsgDspUsr (SI_FIOINVFMT);               /* Invalid File Format  */
        return (FIORETERR);
    }

    /********************************************************************/
    /* Allocate global memory for file header                           */
    /********************************************************************/
    lpVBSHdr = (VBSHDR FAR *) GloAloLck (GMEM_MOVEABLE, lphHdrMem, sizeof (VBSHDR)); 
    if ((VBSHDR FAR *) NULL == lpVBSHdr) {
        MsgDspUsr (SI_LOCINSMEM);                   /* Insufficient Mem */
        return ((WORD) -1);
    }

    /********************************************************************/
    /* Read VBase Header Information                                    */
    /********************************************************************/
    if (sizeof (VBSHDR) != Rd_FilFwd (sSrcHdl, (LPSTR) lpVBSHdr,
      sizeof (VBSHDR), usEncFmt, &uiErrCod)) {
        GloMemUnL (*lphHdrMem);
        if (EACCES == uiErrCod) MsgDspUsr (SI_FIOLCKORO);           /* No Access    */
         else if (EBADF == uiErrCod) MsgDspUsr (SI_FIOINVHDL);      /* Bad Handle   */
          else MsgDspUsr (SI_FIOINVFMT);                            /* Invalid Fmt  */
        return (FIORETERR);
    }
 
    /********************************************************************/
    /* Check and set sample frequency                                   */
    /********************************************************************/
    if ((lpVBSHdr->ulSmpFrq > VBSMAXFRQ) || (lpVBSHdr->ulSmpFrq <= 0))    
         lpVBSHdr->ulSmpFrq = VBSDEFFRQ;

    /********************************************************************/
    /* Perform test for valid format                                    */
    /********************************************************************/
    ulFilLen = FilGetLen (sSrcHdl);
    ulIdxCnt = lpVBSHdr->ulIdxTot;
    ulMinLen = (DWORD) sizeof(VBSHDR) + ((DWORD) sizeof (VBIREC) * ulIdxCnt);         
    GloMemUnL (*lphHdrMem);

    if ((0L == ulIdxCnt) || (ulIdxCnt > (DWORD) VBSMAXIDX)
      || (-1L == ulFilLen) || (ulMinLen > ulFilLen)) {
        MsgDspUsr (SI_FIOINVFMT);                   /* Invalid File Fmt */
        return (FIORETERR);
    }

    /********************************************************************/
    /* Allocate mem for whole array in multiples of sizeof (VBIREC)     */
    /********************************************************************/
    lpIdxRec = (VBIREC FAR *) GloAloLck (GMEM_MOVEABLE, lphIdxMem, (sizeof (VBIREC) * (WORD) ulIdxCnt));
    if ((VBIREC FAR *) NULL == lpIdxRec) {
        MsgDspUsr (SI_LOCINSMEM);                   /* Insufficient Mem */
        return ((WORD) -1);
    }

    /********************************************************************/
    /* Read indexes; if read not complete, return error                 */
    /********************************************************************/
    if (-1L == Rd_FilFwd (sSrcHdl, (LPSTR) lpIdxRec,
      sizeof (VBIREC) * (WORD) ulIdxCnt, usEncFmt, &uiErrCod)) {
        GloMemUnL (*lphIdxMem);
        if (EACCES == uiErrCod) MsgDspUsr (SI_FIOLCKORO);       /* No Access    */
         else if (EBADF == uiErrCod) MsgDspUsr (SI_FIOINVHDL);  /* Bad Handle   */
          else MsgDspUsr (SI_FIOINVFMT);                        /* Invalid Fmt  */
        return (FIORETERR);
    }
    GloMemUnL (*lphIdxMem);

    /********************************************************************/
    /********************************************************************/
    return (FIORETSUC);

}

FIORET FAR PASCAL WrtVBSHdr (short sDstHdl, VBSHDR FAR *lpVBSHdr, VBIREC FAR *lpIdxRec,
                  WORD usEncFmt)
{

    unsigned    uiErrCod;
    WORD        usIdxSiz;

    /********************************************************************/
    /********************************************************************/
    usIdxSiz = sizeof (VBIREC) * (WORD) lpVBSHdr->ulIdxTot;

    /********************************************************************/
    /* Write VBase Header Information                                   */
    /********************************************************************/
    if (sizeof (VBSHDR) != Wr_FilFwd (sDstHdl, (LPSTR) lpVBSHdr,
      sizeof (VBSHDR), usEncFmt, &uiErrCod)) {
        if (EACCES == uiErrCod) MsgDspUsr (SI_FIOLCKORO);          /* No Access    */
         else if (EBADF == uiErrCod) MsgDspUsr (SI_FIOINVHDL);     /* Bad Handle   */
          else if (ENOSPC == uiErrCod) MsgDspUsr (SI_FIOINSSPC);   /* Ins Dsk Spc  */
           else MsgDspUsr (SI_FIONOOWRT);                          /* No Overwrite */
        return (FIORETERR);
    }

    /********************************************************************/
    /* Write VBase Index Information                                    */
    /********************************************************************/
    if (usIdxSiz != (WORD) Wr_FilFwd (sDstHdl, (LPSTR) lpIdxRec,
      usIdxSiz, usEncFmt, &uiErrCod)) {
        if (EACCES == uiErrCod) MsgDspUsr (SI_FIOLCKORO);          /* No Access    */
         else if (EBADF == uiErrCod)  MsgDspUsr (SI_FIOINVHDL);    /* Bad Handle   */
          else if (ENOSPC == uiErrCod) MsgDspUsr (SI_FIOINSSPC);   /* Ins Dsk Spc  */
           else MsgDspUsr (SI_FIONOOWRT);                          /* No Overwrite */
        return (FIORETERR);
    }

    /********************************************************************/
    /********************************************************************/
    return (FIORETSUC);

}

WORD FAR PASCAL RelVBSHdr (VISMEMHDL hHdrMem, VISMEMHDL hIdxMem)
{

    /********************************************************************/
    /********************************************************************/
    GloAloRel (hHdrMem);
    GloAloRel (hIdxMem);

    /********************************************************************/
    /********************************************************************/
    return (0);

}
