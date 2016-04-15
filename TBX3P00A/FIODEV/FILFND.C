/************************************************************************/
/* File Find Support: FilFnd.c                          V2.00  02/01/93 */
/* Copyright (c) 1987-1993 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "filutl.h"                     /* File utilities definitions   */
#include "..\os_dev\dosmsg.h"           /* User message support defs    */

#include <stdio.h>                      /* Standard I/O                 */
#include <stdlib.h>                     /* Standard library defs        */
#include <string.h>                     /* String manipulation funcs    */
#include <direct.h>                     /* DOS directory operations     */
#include <malloc.h>                     /* Memory allocation functions  */
#include <dos.h>                        /* DOS low-level routines       */

/************************************************************************/
/************************************************************************/
WORD    GetFstFil (char *szFilSpc, WORD usSpcTyp, char *szFulNam, 
        LSTSPC *pLstSpc, FNDSRTCBK FndSrtCbk)
{
    static  struct  find_t FilInf;
    static  char  * pcPtrLst[LSTPTRCNT]; 
    WORD            usNxtPos;

    /********************************************************************/
    /********************************************************************/
    if (0 != _dos_findfirst (_strupr (szFilSpc), _A_NORMAL, &FilInf)) {
        return (0);
    }

    /********************************************************************/
    /* Initialize list specification block                              */
    /********************************************************************/
    pLstSpc->szNamBuf = NULL;
    pLstSpc->usBufSiz = LSTBUFSIZ;
    pLstSpc->paPtrArr = pcPtrLst;
    pLstSpc->usIdxCur = 0;
    pLstSpc->usIdxCnt = 0;

    /********************************************************************/
    /* Allocate space for list                                          */
    /* Copy first file name, allow space for null terminator            */
    /********************************************************************/
    if (NULL == (pLstSpc->szNamBuf = malloc (pLstSpc->usBufSiz))) {
        MsgDspUsr ("Insufficient file list memory. Operation aborted.\n");
        return (0);
    }

    pLstSpc->paPtrArr[pLstSpc->usIdxCnt++] = strcpy (pLstSpc->szNamBuf, _strlwr(FilInf.name));
    usNxtPos = strlen (pLstSpc->szNamBuf) + 1;

    /********************************************************************/
    /********************************************************************/
    while ((0 == _dos_findnext (&FilInf)) && (pLstSpc->usIdxCnt < LSTPTRCNT)) {
        if (usNxtPos + strlen (FilInf.name) >= pLstSpc->usBufSiz) break;
        pLstSpc->paPtrArr[pLstSpc->usIdxCnt++] 
            = strcpy (&pLstSpc->szNamBuf[usNxtPos], _strlwr (FilInf.name));
        usNxtPos += strlen (FilInf.name) + 1;
    }

    /********************************************************************/
    /* Sort filenames if requested                                      */
    /********************************************************************/
    if (FndSrtCbk) qsort 
        (pLstSpc->paPtrArr, pLstSpc->usIdxCnt, sizeof (*pLstSpc->paPtrArr), FndSrtCbk);

    /********************************************************************/
    /********************************************************************/
    strcat (GetFulPth (szFilSpc, szFulNam, NULL), pLstSpc->paPtrArr[0]);

    /********************************************************************/
    /********************************************************************/
    return (pLstSpc->usIdxCnt);

}
 
WORD    GetNxtFil (char *szFulNam, LSTSPC *pLstSpc)
{
    static char szFilDrv[_MAX_DRIVE], szFilDir[_MAX_DIR];   //Stk:   3 + 256
    static char szFilNam[_MAX_FNAME], szFilExt[_MAX_EXT];   //     256 + 256

    /********************************************************************/
    /********************************************************************/
    if ((pLstSpc->usIdxCur + 1) >= pLstSpc->usIdxCnt) return (0);
    if (NULL == pLstSpc->paPtrArr[++pLstSpc->usIdxCur]) return (0);

    /********************************************************************/
    /********************************************************************/
    _splitpath (szFulNam, szFilDrv, szFilDir, szFilNam, szFilExt);
    strcat (strcat (strcpy (szFulNam, szFilDrv), szFilDir), pLstSpc->paPtrArr[pLstSpc->usIdxCur]);

    /********************************************************************/
    /********************************************************************/
    return (strlen (szFulNam));

}

void EndFilLst (LSTSPC *pLstSpc)
{

    free (pLstSpc->szNamBuf);

}

char *  GetWldNam (const char *szSrcSpc, const char *szDstSpc, char *szNewSpc)
{
    static char szSrcDrv[_MAX_DRIVE], szSrcDir[_MAX_DIR];   //Stk:   3 + 256
    static char szSrcNam[_MAX_FNAME], szSrcExt[_MAX_EXT];   //     256 + 256
    static char szDstDrv[_MAX_DRIVE], szDstDir[_MAX_DIR];   //       3 + 256
    static char szDstNam[_MAX_FNAME], szDstExt[_MAX_EXT];   //     256 + 256

    /********************************************************************/
    /********************************************************************/
    _splitpath (szSrcSpc, szSrcDrv, szSrcDir, szSrcNam, szSrcExt);
    _splitpath (szDstSpc, szDstDrv, szDstDir, szDstNam, szDstExt);
    if (!(strlen (szDstNam) + strlen (szDstExt))) {
        strcpy (szDstNam, szSrcNam);
        strcpy (szDstExt, szSrcExt);
    }
    else {
        if (strchr (szDstNam, '*')) strcpy (szDstNam, szSrcNam);
        if (strchr (szDstExt, '*')) strcpy (szDstExt, szSrcExt);
    }
    _makepath  (szNewSpc, szDstDrv, szDstDir, szDstNam, szDstExt);

    return (szNewSpc);

}


char *  GetFulPth (char *szFilSpc, char *szFilPth, char *szFulNam)
{
    static char szFilDrv[_MAX_DRIVE], szFilDir[_MAX_DIR];   //Stk:   3 + 256
    static char szHomDrv[_MAX_DRIVE], szHomDir[_MAX_DIR];   //       3 + 256
    static char szDumNam[_MAX_FNAME], szDumExt[_MAX_EXT];   //     256 + 256
    static char szHomSpc[_MAX_PATH];                        //     260

    /********************************************************************/
    /********************************************************************/
    _splitpath (szFilSpc, szFilDrv, szFilDir, szDumNam, szDumExt);
    if (NULL != szFulNam) strcat (strcpy (szFulNam, szDumNam), szDumExt);
    if (0 == (strlen (szFilDrv) + strlen(szFilDir))) {
        xgetdcwd (szFilPth, _MAX_PATH);
        return (szFilPth);
    }

    xgetdcwd (szHomSpc, _MAX_PATH);
    _splitpath (szHomSpc, szHomDrv, szHomDir, szDumNam, szDumExt);

    /********************************************************************/
    /********************************************************************/
    if (strlen (szFilDrv) && _stricmp (szFilDrv, szHomDrv)) {
        xsetdrive (szFilDrv[0], &szHomDrv[0]);
        xgetdcwd (szHomSpc, _MAX_PATH);
        _splitpath (szHomSpc, szDumNam, szHomDir, szDumNam, szDumExt);
        xchdir (szFilDir);
        xgetdcwd (szFilPth, _MAX_PATH);
        xchdir (szHomDir);
        xsetdrive (szHomDrv[0], &szFilDrv[0]);
    } 
    else {
        xchdir (szFilDir);
        xgetdcwd (szFilPth, _MAX_PATH);
        xchdir (szHomDir);
    }

    /********************************************************************/
    /********************************************************************/
    return (szFilPth);

}

/************************************************************************/
/*              Drive/ Directory Utility Functions                      */
/************************************************************************/
char	get_cur_drive (void);
void	set_cur_drive (char);

int     xchdir (char *dirname)
{
    WORD    usNamLen = strlen (dirname);
    int     iRetCod;

    /********************************************************************/
    /* Trim trailing '\' if present & not root directory                */
    /********************************************************************/
    if ((usNamLen > 1) && ('\\' == dirname[usNamLen-1]) && (':' != dirname[usNamLen-2])) {
        dirname[usNamLen-1] = '\0';
        iRetCod = _chdir (dirname);   
        dirname[usNamLen-1] = '\\';
    }
    else iRetCod = _chdir (dirname);   

    /********************************************************************/
    /********************************************************************/
    return (iRetCod);   

}

char *  _cdecl xgetdcwd (char *szWrkBuf, int nMaxLen)
{
    WORD    usPthLen;

    /********************************************************************/
    /********************************************************************/
    if (nMaxLen <= 1) return (NULL);
    usPthLen = strlen (_getdcwd (0, szWrkBuf, nMaxLen - 1));

    /********************************************************************/
    /********************************************************************/
    if ((usPthLen > 0) && ('\\' != szWrkBuf[usPthLen-1])) strcat (szWrkBuf, "\\");
    return (szWrkBuf);
}

char	get_cur_drive () 
{
    static unsigned nCurDrv;
    
    /********************************************************************/
    /********************************************************************/
    _dos_getdrive (&nCurDrv);
    if ((nCurDrv < 1) || (nCurDrv > 26)) return (0);
    return ((char) 'A' + (char) nCurDrv - (char) 1);

}
void	set_cur_drive (char cNewDrv)
{
    static unsigned nDrvCnt;

    /********************************************************************/
    /********************************************************************/
    cNewDrv = (char) (1 + toupper (cNewDrv) - 'A');
    if ((cNewDrv < 1) || (cNewDrv > 26)) return;

    _dos_setdrive (cNewDrv, &nDrvCnt);
    return;

}

WORD    xsetdrive (char cNewDrv, char *pcCurDrv) 
{
    /********************************************************************/
    /********************************************************************/
    if (pcCurDrv) *pcCurDrv = get_cur_drive ();

    /********************************************************************/
    /********************************************************************/
    set_cur_drive (cNewDrv);
    return (0);
}



