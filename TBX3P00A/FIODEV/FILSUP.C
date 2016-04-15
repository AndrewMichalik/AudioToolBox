/************************************************************************/
/* File I/O support: FilSup.c                           V2.00  07/15/94 */
/* Copyright (c) 1987-1994 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#include "filutl.h"                     /* File utilities definitions   */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */
#include <stdio.h>                      /* Standard I/O                 */
#include <fcntl.h>                      /* Flags used in open/ sopen    */
#include <sys\stat.h>                   /* File status types            */
#include <errno.h>                      /* errno value constants        */
#include <dos.h>                        /* DOS low-level routines       */

/************************************************************************/
/************************************************************************/
void        _fsplitpath (LPSTR, LPSTR, LPSTR, LPSTR, LPSTR);
unsigned    _fdos_getftime (int, unsigned far *, unsigned far *);

/************************************************************************/
/************************************************************************/
long FAR PASCAL FilCop (short sSrcHdl, short sDstHdl, LPSTR lpWrkBuf, 
                WORD usBufSiz, DWORD ulReqCnt, WORD usEncFmt, 
                unsigned int far *lpErrCod, FIOPOLPRC fpPolPrc, 
                DWORD ulPolDat)
{
    WORD        usTmpCnt = 0;
    DWORD       ulXfrCnt = 0L;

    /********************************************************************/
    /* If no bytes requested or available, return OK                    */
    /* Future: check current file position for available bytes          */
    /********************************************************************/
    ulReqCnt = min (ulReqCnt, (DWORD) FilGetLen (sSrcHdl));
    if (0L == ulReqCnt) return (ulReqCnt);

    /********************************************************************/
    /* Show/Hide Progress indicator for time consuming operations       */
    /********************************************************************/
    if (fpPolPrc) fpPolPrc (FIOPOLBEG, ulReqCnt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    while (-1 != usTmpCnt) {
        usTmpCnt = Rd_FilFwd (sSrcHdl, lpWrkBuf,
            (WORD) ((ulReqCnt <= (DWORD) usBufSiz) ? ulReqCnt : usBufSiz), 
            usEncFmt, lpErrCod);
        if ((-1 == usTmpCnt) || (0 == usTmpCnt)) break;
        usTmpCnt = Wr_FilFwd (sDstHdl, lpWrkBuf, usTmpCnt, usEncFmt, lpErrCod);
        if ((-1 == usTmpCnt) || (0 == usTmpCnt)) break;
        ulReqCnt -= (DWORD) usTmpCnt;
        ulXfrCnt += (DWORD) usTmpCnt;
        if (fpPolPrc && !fpPolPrc (FIOPOLCNT,  ulXfrCnt, ulPolDat)) {
            usTmpCnt = (WORD) -1;
            *lpErrCod = FIO_E_ABT;
            break;
        }
    }
    if (fpPolPrc) fpPolPrc (FIOPOLEND,  ulXfrCnt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    if (-1 == usTmpCnt) return (-1L);
    return (ulXfrCnt);

}

long FAR PASCAL FilCRv (short sSrcHdl, short sDstHdl, LPSTR lpWrkBuf, 
                WORD usBufSiz, DWORD ulReqCnt, WORD usEncFmt, 
                unsigned int far *lpErrCod, FIOPOLPRC fpPolPrc,
                DWORD ulPolDat)
{
    WORD        usTmpCnt = 0;
    DWORD       ulXfrCnt = 0L;

    /********************************************************************/
    /* If no bytes requested or available, return OK                    */
    /* Future: check current file position for available bytes          */
    /********************************************************************/
    ulReqCnt = min (ulReqCnt, (DWORD) FilGetLen (sSrcHdl));
    if (0L == ulReqCnt) return (ulReqCnt);

    /********************************************************************/
    /* Show/Hide Progress indicator for time consuming operations       */
    /********************************************************************/
    if (fpPolPrc) fpPolPrc (FIOPOLBEG, ulReqCnt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    while (-1 != usTmpCnt) {
        usTmpCnt = Rd_FilRev (sSrcHdl, lpWrkBuf,
            (WORD) ((ulReqCnt <= (DWORD) usBufSiz) ? ulReqCnt : usBufSiz), 
            usEncFmt, lpErrCod);
        if ((-1 == usTmpCnt) || (0 == usTmpCnt)) break;
        usTmpCnt = Wr_FilRev (sDstHdl, lpWrkBuf, usTmpCnt, usEncFmt, lpErrCod);
        if ((-1 == usTmpCnt) || (0 == usTmpCnt)) break;
        ulReqCnt -= (DWORD) usTmpCnt;
        ulXfrCnt += (DWORD) usTmpCnt;
        if (fpPolPrc && !fpPolPrc (FIOPOLCNT,  ulXfrCnt, ulPolDat)) {
            usTmpCnt = (WORD) -1;
            *lpErrCod = FIO_E_ABT;
            break;
        }
    }
    if (fpPolPrc) fpPolPrc (FIOPOLEND,  ulXfrCnt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    if (-1 == usTmpCnt) return (-1L);
    return (ulXfrCnt);

}

long FAR PASCAL FilDup (short sSrcHdl, short sDstHdl, LPSTR lpWrkBuf, 
                WORD usBufSiz, WORD usEncFmt, unsigned int far *lpErrCod, 
                FIOPOLPRC fpPolPrc, DWORD ulPolDat)
{
    WORD        usTmpCnt = 0;
    DWORD       ulXfrCnt = 0L;
    DWORD       ulReqCnt = FilGetLen (sSrcHdl);

    /********************************************************************/
    /********************************************************************/
    if ((0L == ulReqCnt) || (-1L == ulReqCnt)) return (ulReqCnt);

    /********************************************************************/
    /* Show/Hide Progress indicator for time consuming operations       */
    /********************************************************************/
    if (fpPolPrc) fpPolPrc (FIOPOLBEG, ulReqCnt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    while (usTmpCnt != -1) {
        usTmpCnt = Rd_FilFwd (sSrcHdl, lpWrkBuf, usBufSiz, usEncFmt, lpErrCod);
        if ((-1 == usTmpCnt) || (0 == usTmpCnt)) break;
        usTmpCnt = Wr_FilFwd (sDstHdl, lpWrkBuf, usTmpCnt, usEncFmt, lpErrCod);
        if ((-1 == usTmpCnt) || (0 == usTmpCnt)) break;
        ulXfrCnt += (DWORD) usTmpCnt;
        if (fpPolPrc && !fpPolPrc (FIOPOLCNT, ulXfrCnt, ulPolDat)) {
            usTmpCnt = (WORD) -1;
            *lpErrCod = FIO_E_ABT;
            break;
        }
    }
    if (fpPolPrc) fpPolPrc (FIOPOLEND, ulXfrCnt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    if (-1 == usTmpCnt) return (-1L);
    return (ulXfrCnt);

}
  
long FAR PASCAL FilMov (const char *szSrcFil, const char *szDstFil, LPSTR lpWrkBuf,
                WORD usBufSiz, BOOL bDatSav, unsigned int far *lpErrCod, 
                FIOPOLPRC fpPolPrc, DWORD ulPolDat)
{
    char    szSrcDrv[_MAX_DRIVE], szSrcDir[_MAX_DIR]; 
    char    szSrcNam[_MAX_FNAME], szSrcExt[_MAX_EXT];
    char    szDstDrv[_MAX_DRIVE], szDstDir[_MAX_DIR]; 
    char    szDstNam[_MAX_FNAME], szDstExt[_MAX_EXT];
    short   sSrcHdl;
    short   sDstHdl;
    int     file_date;
    int     file_time;

    /********************************************************************/
    /* Split names of source and destination files                      */
    /********************************************************************/
    _fsplitpath ((char *) szSrcFil, szSrcDrv, szSrcDir, szSrcNam, szSrcExt);
    _fsplitpath ((char *) szDstFil, szDstDrv, szDstDir, szDstNam, szDstExt);

    if (_fstricmp (szSrcDrv, szDstDrv) | _fstricmp (szSrcDir, szDstDir)) {
        /****************************************************************/
        /****************************************************************/
        if (-1 == (sSrcHdl = FilHdlOpn (szSrcFil, O_BINARY | O_RDONLY))) {
            return (-1L);
        }
        if (bDatSav) _fdos_getftime (sSrcHdl, &file_date, &file_time);    

        if (-1 == (sDstHdl = FilHdlCre (szDstFil,
            O_BINARY | O_CREAT | O_TRUNC | O_WRONLY, S_IREAD | S_IWRITE))) {
            FilHdlCls (sSrcHdl);
            *lpErrCod = EACCES;
            return (-1L);
        }

        if (-1L == FilDup (sSrcHdl, sDstHdl, lpWrkBuf, usBufSiz, FIOENCNON, 
          lpErrCod, fpPolPrc, ulPolDat)) {
            FilHdlCls (sSrcHdl);
            FilHdlCls (sDstHdl);
            return (-1L);
        }

        /****************************************************************/
        /****************************************************************/
        if (bDatSav) _dos_setftime (sDstHdl, file_date, file_time);    
        FilHdlCls (sSrcHdl);
        FilHdlCls (sDstHdl);
        remove (szSrcFil);
    }
    else {
        remove (szDstFil);
        rename (szSrcFil, szDstFil);           /* Rename Src to Dst    */
    }

    return (0L);

}

long FAR PASCAL FilShf (short sSrcHdl, short sDstHdl, LPSTR lpWrkBuf, 
                WORD usBufSiz, DWORD ulShfPnt, DWORD ulShfAmt, FIODIR fdDirFlg,
                unsigned int far *lpErrCod, FIOPOLPRC fpPolPrc, DWORD ulPolDat)
{
    DWORD   ulSrcLen;

    /********************************************************************/
    /* If shift (non-counting#) > file length (counting#), assume       */
    /* size change required, no bytes copied.                            */
    /* Check if required to adjust for shift past end of file.          */
    /********************************************************************/
    if (ulShfPnt > (ulSrcLen = FilGetLen (sSrcHdl))) {
        if (FIODIRFWD == fdDirFlg) ulShfAmt += (ulShfPnt - ulSrcLen);
        if (FIODIRREV == fdDirFlg) ulShfAmt -= min (ulShfAmt, ulShfPnt - ulSrcLen);
        ulShfPnt = ulSrcLen;
    }

    /********************************************************************/
    /* If no bytes requested, return OK                                 */
    /********************************************************************/
    if (ulShfAmt == 0L) return (ulShfAmt);

    /********************************************************************/
    /* For shift left, ulShfPnt re-appears shifted left by ulShfAmt     */
    /* Could truncate file if ulShfPnt = ulShfAmt = EOF (filelength)    */
    /********************************************************************/
    if (FIODIRREV == fdDirFlg) {
        if (ulShfAmt > ulShfPnt) ulShfAmt = ulShfPnt;
        FilSetPos (sSrcHdl, ulShfPnt, SEEK_SET);
        FilSetPos (sDstHdl, ulShfPnt - ulShfAmt, SEEK_SET);
        if (-1L == FilCop (sSrcHdl, sDstHdl, lpWrkBuf, usBufSiz, 
            ulSrcLen - ulShfPnt, FIOENCNON, lpErrCod, fpPolPrc, ulPolDat))
            return (-1L);
        if (0L == FilSiz (sDstHdl, ulSrcLen, - (long) ulShfAmt, lpErrCod, fpPolPrc, ulPolDat))
            return (-1L);
    }  
    else if (FIODIRFWD == fdDirFlg) {
        if (0L == FilSiz (sDstHdl, ulSrcLen, +ulShfAmt, lpErrCod, fpPolPrc, ulPolDat))
            return (-1L);
        FilSetPos (sSrcHdl, ulSrcLen, SEEK_SET);
        FilSetPos (sDstHdl, ulSrcLen + ulShfAmt, SEEK_SET);
        if (-1L == FilCRv (sSrcHdl, sDstHdl, lpWrkBuf, usBufSiz, ulSrcLen - ulShfPnt,
          FIOENCNON, lpErrCod, fpPolPrc, ulPolDat)) {
            FilSiz (sDstHdl, ulSrcLen, 0L, lpErrCod, fpPolPrc, ulPolDat);
            return (-1L);
        }
    }
    else ulShfAmt = 0L;

    /********************************************************************/
    /********************************************************************/
    return (ulShfAmt);

}

long FAR PASCAL FilSiz (short sFilHdl, long lOrgLen, long lDltLen, 
                unsigned int far *lpErrCod, FIOPOLPRC fpPolPrc, DWORD ulPolDat)
{
    long    lShfLen = 0L;

    /********************************************************************/
    /* Change file size by moving and writing 0 bytes.                  */
    /* Caller will fill, so do not fill with '\0'.                      */
    /********************************************************************/
    if (-1L == FilSetPos (sFilHdl, lOrgLen + lDltLen, SEEK_SET)) {
        *lpErrCod = EBADF;
        return (0L);
    }

//    /********************************************************************/
//    /* Show/Hide Progress indicator for time consuming operations       */
//    /********************************************************************/
//    if (fpPolPrc) fpPolPrc (FIOPOLBEG, labs (lDltLen), ulPolDat);

    /********************************************************************/
    /* DOS may take quite some time, but will not indicate progress     */
    /********************************************************************/
    if (0 == Wr_FilFwd (sFilHdl, NULL, 0, FIOENCNON, lpErrCod)) {
        return (lDltLen);
// ajm 07/95 - serious speed improvement without commit!
//        /****************************************************************/
//        /* Check if commit failed                                       */
//        /****************************************************************/
//        if (fpPolPrc) fpPolPrc (FIOPOLCNT, labs (lDltLen) / 2L, ulPolDat);
//        if (0 == (*lpErrCod = Wr_FilCom (sFilHdl))) lShfLen = lDltLen;
//        if (fpPolPrc) fpPolPrc (FIOPOLEND, labs (lDltLen), ulPolDat);
//        return (lShfLen);
    }

//    /********************************************************************/
//    /* Do not poll during recovery                                      */
//    /********************************************************************/
//    if (fpPolPrc) fpPolPrc (FIOPOLEND, labs (lDltLen), ulPolDat);

    /********************************************************************/
    /* Return code indicates error; attempt to recover                  */
    /********************************************************************/
    if ((*lpErrCod) && (ENOSPC == *lpErrCod)) {
        /****************************************************************/
        /* Not enough space, try to reset to original size              */
        /****************************************************************/
        if (-1L == FilSetPos (sFilHdl, lOrgLen, SEEK_SET)) return (0L);
        if (0 == Wr_FilFwd (sFilHdl, NULL, 0, FIOENCNON, lpErrCod)) {
            Wr_FilCom (sFilHdl);                    /* Commit           */
            *lpErrCod = ENOSPC;                     /* Reset no space   */
            return (0L);                            /* File recovered   */
        }
        *lpErrCod = ENOSPC;                         /* Reset no space   */
    }

    /********************************************************************/
    /********************************************************************/
    return (0L);                            

}

/************************************************************************/
/************************************************************************/
void _fsplitpath (LPSTR path, LPSTR drive, LPSTR dir, LPSTR fname, LPSTR ext)
{
    static  char    szSrcPth[_MAX_PATH];
    static  char    szDstDrv[_MAX_DRIVE], szDstDir[_MAX_DIR]; 
    static  char    szDstNam[_MAX_FNAME], szDstExt[_MAX_EXT];
    
    /********************************************************************/
    /********************************************************************/
    _fstrncpy (szSrcPth, path, _MAX_PATH);
    _splitpath (szSrcPth, szDstDrv, szDstDir, szDstNam, szDstExt);

    /********************************************************************/
    /********************************************************************/
    _fstrncpy (szDstDrv, drive, _MAX_DRIVE);
	_fstrncpy (szDstDir, dir, _MAX_DIR); 
    _fstrncpy (szDstNam, fname, _MAX_FNAME);
	_fstrncpy (szDstExt, ext, _MAX_EXT);

    /********************************************************************/
    /********************************************************************/
	return;
}

unsigned _fdos_getftime (int handle, unsigned far *date, unsigned far *time)
{
    unsigned    nRetCod;
    static int  file_date;
    static int  file_time;

    /********************************************************************/
    /********************************************************************/
    if (!(nRetCod = _dos_getftime (handle, &file_date, &file_time))) {    
        *date = file_date;
        *time = file_time;
    }
    return (nRetCod);

}
