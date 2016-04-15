/************************************************************************/
/* Binary File Replacement support: FilRep.c            V1.30  09/20/92 */
/* Copyright (c) 1987-1991 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "filutl.h"                     /* File utilities definitions   */
#include "..\os_dev\dosmem.h"           /* Generic memory support defs  */
#include "..\os_dev\dosmsg.h"           /* User message support defs    */

#include <stdio.h>                      /* Standard I/O                 */
#include <io.h>                         /* Low level file I/O           */
#include <fcntl.h>                      /* Flags used in open/ sopen    */
#include <sys\types.h>                  /* File status and time types   */
#include <sys\stat.h>                   /* File status types            */
#include <string.h>                     /* String manipulation funcs    */
#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <direct.h>                     /* DOS directory operations     */
#include <dos.h>                        /* DOS low-level routines       */
  
/************************************************************************/
/************************************************************************/
WORD FAR PASCAL BinFilRep (char *szSrcSpc, char *szTrgSpc, WORD usRepFlg, 
                BFRCHKFNC pfChkFnc, BFRREPFNC pfRepFnc, BFRENDFNC pfEndFnc, 
                FNDSRTCBK FndSrtCbk, void far *lpRepBlk)
{
    VISMEMHDL       hWrkBuf;
    static char     szTrgPth[_MAX_DRIVE + _MAX_DIR]; 
    static char     szTrgNam[_MAX_FNAME + _MAX_EXT];
    static char     szSrcFil[_MAX_PATH];
    static LSTSPC   lsLstSpc;
    LPSTR           lpWrkBuf;
    DWORD           ulBufSiz;
    DWORD           ulRepCnt;
    #define         REPBUFDEF   32L * 1024

    /********************************************************************/
    /********************************************************************/
    if (strlen (szTrgSpc)) {
        GetFulPth (szTrgSpc, szTrgPth, szTrgNam);
        szTrgSpc = strcat (szTrgPth, szTrgNam);
    }

    /********************************************************************/
    /********************************************************************/
    if (0 == GetFstFil (szSrcSpc, 0, szSrcFil, &lsLstSpc, FndSrtCbk)) {
        return ((WORD) -1);
    }

    /********************************************************************/
    /* Allocate memory in multiples of 512 and < 64Kbyte                */
    /* Allow 2048 bytes headroom                                        */
    /********************************************************************/
    if ((ulBufSiz = (GloMemMax () / 512L) * 512L) <= 2048L) {
        MsgDspUsr ("Insufficient buffer memory. Operation aborted.\n");
        EndFilLst (&lsLstSpc);
        return ((WORD) -1);
    }
    ulBufSiz = min (REPBUFDEF, ulBufSiz - 2048L);
    if (NULL == (lpWrkBuf = GloAloLck (GMEM_MOVEABLE, &hWrkBuf, ulBufSiz))) {
        MsgDspUsr ("Insufficient buffer memory. Operation aborted.\n");
        EndFilLst (&lsLstSpc);
        return ((WORD) -1);
    }

    /********************************************************************/
    /********************************************************************/
    do {
        if (pfChkFnc (lsLstSpc.paPtrArr[lsLstSpc.usIdxCur], lpRepBlk)) {
            ulRepCnt = RepSrcToD (szSrcFil, szTrgSpc, usRepFlg, lpWrkBuf, 
                (WORD) ulBufSiz, pfRepFnc, lpRepBlk);
            pfEndFnc (ulRepCnt, lpRepBlk);
        }
    } while (0 != GetNxtFil (szSrcFil, &lsLstSpc));

    hWrkBuf = GloUnLRel (hWrkBuf);
    EndFilLst (&lsLstSpc);

    /********************************************************************/
    /********************************************************************/
    return (0);

}

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL RepSrcToD (const char *szSrcFil, const char *szDstFil, WORD usRepFlg, 
                 LPSTR lpWrkBuf, WORD usBufSiz, BFRREPFNC pfRepFnc, 
                 void far *lpRepBlk)
{
    short           sSrcHdl;
    short           sDstHdl;
    static  char    szBakFil[FIOMAXNAM];
    static  char    szTmpFil[FIOMAXNAM];
    char          * pbWrkPtr;
    DWORD           ulRepCnt = 0L;
    static   int    file_date;
    static   int    file_time;
    unsigned int    uiErrCod;

    /********************************************************************/
    /********************************************************************/
    sSrcHdl = FilHdlOpn (szSrcFil, O_BINARY | O_RDONLY);
    if (-1 == sSrcHdl) {
        MsgDspUsr ("Cannot Open Source File: %s\n", szSrcFil);
        return (0L);
    }
    if (usRepFlg & BFRDATFLG) _dos_getftime (sSrcHdl, &file_date, &file_time);    

    /********************************************************************/
    /* If null destination, create a temporary file in the current      */
    /* working directory.                                               */
    /* If not null, check for "*" wildcard or drive/dir only spec       */
    /********************************************************************/
    if (!strlen (szDstFil)) {
        _getcwd (szTmpFil, FIOMAXNAM);
        if (NULL == (pbWrkPtr = _tempnam (szTmpFil, "B_"))) {
            FilHdlCls (sSrcHdl);
            MsgDspUsr ("Cannot Create Temporary File: %s\n", szTmpFil);
            return (0L);
        }
        strcpy (szTmpFil, pbWrkPtr);
        free (pbWrkPtr);
    }
    else {
        GetWldNam (szSrcFil, szDstFil, szTmpFil);

        /****************************************************************/
        /****************************************************************/
        if (0 == _stricmp (szSrcFil, szTmpFil)) {
            FilHdlCls (sSrcHdl);
            MsgDspUsr ("Cannot Overwrite Source File: %s\n", szSrcFil);
            return (0L);
        }
    }

    /********************************************************************/
    /* Open a zero length target file                                   */
    /********************************************************************/
    sDstHdl = FilHdlCre (szTmpFil, O_BINARY | O_CREAT | O_TRUNC | O_WRONLY,
        S_IREAD | S_IWRITE);
    if (-1 == sDstHdl) {
        FilHdlCls (sSrcHdl);
        MsgDspUsr ("Cannot Open Target File: %s\n", szTmpFil);
        return (0L);
    }

    /********************************************************************/
    /* Perform replacement                                              */
    /* If save date flag set, set Dst file date and time to equal Src   */
    /********************************************************************/
    ulRepCnt = pfRepFnc (sSrcHdl, sDstHdl, szSrcFil, szDstFil, lpWrkBuf, usBufSiz, lpRepBlk);
    if (usRepFlg & BFRDATFLG) _dos_setftime (sDstHdl, file_date, file_time);    
    FilHdlCls (sSrcHdl);
    FilHdlCls (sDstHdl);

    if (-1L == ulRepCnt) {
        MsgDspUsr ("Cannot replace to target file: %s\n", szTmpFil);
        return (0L);
    }

    /********************************************************************/
    /* If target specified AND no replacements, delete empty dest file  */ 
    /* If no target specified AND no replacements, delete temp file     */ 
    /********************************************************************/
    if (0L == ulRepCnt) {
        remove (szTmpFil);                      /* Delete empty file    */
        return (0L);
    }
    if (*szDstFil != '\0') return (ulRepCnt);   /* Done (explicit dest) */

    /********************************************************************/
    /* If no target specified AND backup requested, save backup         */ 
    /********************************************************************/
    if (!(usRepFlg & BFRBKIFLG)) {
        strcpy (szBakFil, szSrcFil);
        if ((pbWrkPtr = strrchr (szBakFil, '.')) != NULL) strcpy (pbWrkPtr, ".BK1");
            else strcat (szBakFil, ".BK1");
        remove (szBakFil);                          /* Del old backups  */

        if (rename (szSrcFil, szBakFil)) {          /* Rename original  */
            MsgDspUsr ("Cannot create backup: %s\n", szBakFil);
            remove (szTmpFil);                      /* Delete temp file */
            return (0L);
        }
    }

    /********************************************************************/
    /* Rename original, or copy if in different drive/directory         */
    /********************************************************************/
    if (FilMov (szTmpFil, szSrcFil, lpWrkBuf, usBufSiz, usRepFlg & BFRDATFLG, &uiErrCod, NULL, 0L)) {
        MsgDspUsr ("Cannot copy to file: %s, attempting recovery...\n", szSrcFil);
        if (!(usRepFlg & BFRBKIFLG)) {
            remove (szSrcFil);                      /* Delete bad copy  */
            rename (szBakFil, szSrcFil);            /* Un-rename orig   */
        }
        else rename (szTmpFil, szSrcFil);           /* Simple rename    */
        ulRepCnt = 0L;
    }

    /********************************************************************/
    /********************************************************************/
    return (ulRepCnt);

}


