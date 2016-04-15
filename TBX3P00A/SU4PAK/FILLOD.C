/************************************************************************/
/* Summa Four Pack File Loader: FilLod.c                V1.00  09/10/92 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "su4hdr.h"                     /* Summa Four header support    */
#include "su4pak.h"                     /* Summa Four pack support      */
#include "..\fiodev\filutl.h"           /* File utilities definitions   */
#include "..\os_dev\dosmsg.h"           /* User message support defs    */
#include "..\regdev\tbxreg.h"           /* User Registration defs       */
#include "..\regdev\tbxlic.h"           /* User Licensing defs          */

#include <string.h>                     /* String manipulation funcs    */
#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <io.h>                         /* Low level file I/O           */
  
/************************************************************************/
/*                  Forward & External References                       */
/************************************************************************/
char  * MsgUseIni =  
"\n\
    Voice Information Systems (1-800-234-VISI) Summa Four Packing Utility\n\
             v3.00a.1 (C) Copyright Andrew J. Michalik 1987-1996\n\n";

char  * MsgDspUse =  
"Usage: Su4Pak [-help] VoxFile [Su4File] [-a -b -d -i -l]\n";

char  * MsgDspHlp = "\
       VoxFile: Source filename.\n\
       Su4File: Target filename.\n\
       -a     : Convert from u-Law to A-law.\n\
       -b     : Backup file creation inhibited.\n\
       -dSSSSS: Description string SSSSS (31 characters max).\n\
       -iNNNN : Starting prompt ID number (default = 0, incremented).\n\
       -lNNN  : Library code number, 0 to 255 (default = 0).\n\
Notes: Environment TMP= specifies temp work files directory.\n";

/************************************************************************/
/************************************************************************/
REPBLK  rbRepBlk = {                    /* Function communication block */
        0L,                             /* ulFilCnt                     */
};

/************************************************************************/
/************************************************************************/
DWORD FAR IniSu4Hdr (SU4HDR FAR *pshSu4Hdr)
{
    pshSu4Hdr->ulHdrFmt = DW_SWP(SU4HDRFMT);
    pshSu4Hdr->ulHdrSiz = DW_SWP(sizeof (SU4HDR));
    pshSu4Hdr->ulHdrVer = DW_SWP(SU4HDRVER);
    pshSu4Hdr->ulFil_ID = DW_SWP(0);
    pshSu4Hdr->ulLibCod = DW_SWP(0);
    pshSu4Hdr->ulEncFmt = DW_SWP(SU4PCMU08);
    pshSu4Hdr->ulSmpFrq = DW_SWP(SU4DEFFRQ);
    pshSu4Hdr->ulRsv001 = DW_SWP(0);
    memset (pshSu4Hdr->ucFilDes, 0, SU4DESLEN);
    return (0L);
}

BOOL FAR PASCAL Su4ChkFil (LPCSTR szFilLst, REPBLK FAR *lpRepBlk)
{
    MsgDspUsr ("%Fs ", szFilLst);
    return (TRUE);
}

BOOL FAR PASCAL Su4EndFil (DWORD ulRepCnt, REPBLK FAR *lpRepBlk)
{
    MsgDspUsr ("\n");
    return (FALSE);
}

DWORD FAR PASCAL Su4RepFil (short sSrcHdl, short sDstHdl, LPCSTR szSrcFil, 
                 LPCSTR szDstFil, LPSTR lpWrkBuf, WORD usBufSiz, REPBLK FAR *lpRepBlk)
{
    WORD        usBytCnt;
    DWORD       ulXfrCnt = 0L;
    unsigned    uiErrCod;

    /********************************************************************/
    /********************************************************************/
    usBytCnt = Wr_FilFwd (sDstHdl, &lpRepBlk->shSu4Hdr, 
        sizeof (lpRepBlk->shSu4Hdr), FIOENCNON, &uiErrCod);
    if ((-1 == usBytCnt) || (0 == usBytCnt)) return (0L);

    ulXfrCnt = Su4CopCvt (sSrcHdl, sDstHdl, lpWrkBuf, usBufSiz, 
        SU4PCMU08, (WORD) DW_SWP(lpRepBlk->shSu4Hdr.ulEncFmt), &uiErrCod);        
    ulXfrCnt += (DWORD) usBytCnt;

    /********************************************************************/
    /********************************************************************/
    lpRepBlk->shSu4Hdr.ulFil_ID 
        = DW_SWP(1 + DW_SWP(lpRepBlk->shSu4Hdr.ulFil_ID));
    lpRepBlk->ulFilCnt++;
    return (ulXfrCnt);

}

