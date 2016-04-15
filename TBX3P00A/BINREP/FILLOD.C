/************************************************************************/
/* Binary File Replacement Loader: FilLod.c             V1.30  11/20/91 */
/* Copyright (c) 1987-1991 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "..\os_dev\dosmsg.h"           /* User message support defs    */
#include "..\fiodev\filutl.h"           /* File utilities definitions   */
#include "binrep.h"                     /* Binary file replacement defs */

#include <string.h>                     /* String manipulation funcs    */
#include <ctype.h>                      /* Char classification funcs    */
#include <stdio.h>                      /* Standard I/O                 */
#include <io.h>                         /* Low level file I/O           */

/************************************************************************/
/************************************************************************/
WORD    FndArr (LPSTR, WORD, LPSTR, WORD);

/************************************************************************/
/************************************************************************/
BOOL FAR PASCAL ArrChkFil (LPCSTR szFilLst, REPBLK FAR *lpRepBlk)
{
    char    cUsrRsp;

    /****************************************************************/
    /****************************************************************/
    MsgDspUsr ("%s, ", szFilLst);

    /****************************************************************/
    /****************************************************************/
    if (lpRepBlk->usSrcSiz == lpRepBlk->usTrgSiz) return (TRUE);

    /****************************************************************/
    /****************************************************************/
    if ((NULL != _fstrstr (szFilLst, ".exe")) || (NULL != _fstrstr (szFilLst, ".com"))) {
        cUsrRsp = ' ';
        MsgDspUsr ("WARNING! Probable executable file. ");
        while ((cUsrRsp != 'y') && (cUsrRsp != 'n')) {
            MsgDspUsr ("Size change OK (y/n)? ");
            scanf ("%1c", &cUsrRsp);
            fflush (stdin);
            cUsrRsp = (char) tolower (cUsrRsp);
        }
        return ('y' == cUsrRsp);
    }

    /****************************************************************/
    /****************************************************************/
    return (TRUE);

}

BOOL FAR PASCAL ArrEndFil (DWORD ulRepCnt, REPBLK FAR *lpRepBlk)
{
    if (ulRepCnt) printf ("%ld replaced\n", ulRepCnt);
    else printf ("\r                                        \r");
    return (TRUE);
}

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL ArrRepFil (short sSrcHdl, short sDstHdl, LPCSTR szSrcFil, 
                 LPCSTR szDstFil, LPSTR lpWrkBuf, WORD usBufSiz, REPBLK FAR *lpRepBlk)
{
    WORD        usRdBCnt;
    WORD        usWrBCnt;
    WORD        usSkpCnt;
    WORD        usNeqCnt;
    DWORD       ulRepCnt = 0L;
    unsigned    uiErrCod;

    /****************************************************************/
    /* If no bytes requested, return 0 replacements                 */
    /****************************************************************/
    if (lpRepBlk->usSrcSiz == 0) return (0L);
    if ((long) lpRepBlk->usSrcSiz > FilGetLen (sSrcHdl)) return (0L);
    if ((lpRepBlk->usSrcSiz > usBufSiz) || (lpRepBlk->usTrgSiz > usBufSiz)) 
        return ((DWORD) -1L);

    /****************************************************************/
    /* Read source buffer, search for source data                   */
    /****************************************************************/
    while (TRUE) {
        usRdBCnt = Rd_FilFwd (sSrcHdl, lpWrkBuf, usBufSiz, FIOENCNON, &uiErrCod);
        if ((usRdBCnt == -1) || (usRdBCnt == 0)) break;
        if (usRdBCnt < lpRepBlk->usSrcSiz) {
            usWrBCnt = Wr_FilFwd (sDstHdl, lpWrkBuf, usRdBCnt, FIOENCNON, &uiErrCod);
            break;
        }
        usSkpCnt = 0;

        /************************************************************/
        /************************************************************/
        while (TRUE) {
            if ((usSkpCnt + lpRepBlk->usSrcSiz) > usRdBCnt) {
                FilSetPos (sSrcHdl, (long) usSkpCnt - (long) usRdBCnt, SEEK_CUR);
                break;
            }
            usNeqCnt = FndArr (&lpWrkBuf[usSkpCnt], usRdBCnt - usSkpCnt,
                lpRepBlk->pbSrcArr, lpRepBlk->usSrcSiz);        
            if ((usSkpCnt + usNeqCnt) >= usRdBCnt) {
                usNeqCnt = usNeqCnt - lpRepBlk->usSrcSiz + 1;
                FilSetPos (sSrcHdl, 1L - (long) lpRepBlk->usSrcSiz, SEEK_CUR);
                usWrBCnt = Wr_FilFwd (sDstHdl, &lpWrkBuf[usSkpCnt], usNeqCnt, FIOENCNON, &uiErrCod);
                break;
            }
            usWrBCnt = Wr_FilFwd (sDstHdl, &lpWrkBuf[usSkpCnt], usNeqCnt, FIOENCNON, &uiErrCod);
            usWrBCnt = Wr_FilFwd (sDstHdl, lpRepBlk->pbTrgArr, lpRepBlk->usTrgSiz, FIOENCNON, &uiErrCod);
            if ((usWrBCnt == -1) || (usWrBCnt != lpRepBlk->usTrgSiz)) break;
            usSkpCnt = usSkpCnt + usNeqCnt + lpRepBlk->usSrcSiz;
            ulRepCnt++;
        }
    }

    /****************************************************************/
    /****************************************************************/
    if ((usRdBCnt == -1) || (usWrBCnt == -1)) return ((DWORD) -1L);

    /****************************************************************/
    /****************************************************************/
    lpRepBlk->ulTotCnt += ulRepCnt;
    return (ulRepCnt);
}


WORD    FndArr (LPSTR pcWrkBuf, WORD usBufSiz, LPSTR pcSrcArr, WORD usSrcSiz)
{
    LPSTR   pcFirst;

    pcFirst = pcWrkBuf;
    while ((usSrcSiz + (WORD) (pcFirst - pcWrkBuf)) <= usBufSiz) {
        pcFirst = _fmemchr (pcFirst, (int) *pcSrcArr, usBufSiz - (pcFirst - pcWrkBuf));
        if (pcFirst == NULL) return (usBufSiz);
        if (0 == _fmemcmp (pcFirst, pcSrcArr, usSrcSiz)) return (pcFirst - pcWrkBuf);
        pcFirst++;
    }
    return (usBufSiz);
}

