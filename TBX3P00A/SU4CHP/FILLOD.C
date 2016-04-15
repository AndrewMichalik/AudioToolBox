/************************************************************************/
/* Summa Four Chop File Loader: FilLod.c                V1.00  12/10/92 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "..\fiodev\filutl.h"           /* File utilities definitions   */
#include "..\os_dev\dosmsg.h"           /* User message support defs    */
#include "..\regdev\tbxreg.h"           /* User Registration defs       */
#include "..\regdev\tbxlic.h"           /* User Licensing defs          */
#include "su4hdr.h"                     /* Summa Four header support    */
#include "su4chp.h"                     /* Summa Four chop support      */

#include <string.h>                     /* String manipulation funcs    */
#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <io.h>                         /* Low level file I/O           */
  
/************************************************************************/
/*                  Forward & External References                       */
/************************************************************************/
char  * MsgUseIni =  
"\n\
    Voice Information Systems (1-800-234-VISI) Summa Four Chopping Utility\n\
             v3.00a.1 (C) Copyright Andrew J. Michalik 1987-1996\n\n";

char  * MsgDspUse =  
"Usage: Su4Chp [-help] Su4File [VoxSpc HdrSpc] [-a]\n";

char  * MsgDspHlp = "\
       Su4File: Source filename.\n\
       VoxSpc:  Target audio  file specification (Default is *.vox).\n\
       HdrSpc:  Target header file specification (Default is *.hdr).\n\
       -a:      Convert from A-law to u-Law.\n\
Notes: Environment TMP= specifies temp work files directory.\n";

/************************************************************************/
/************************************************************************/
REPBLK  rbRepBlk = {                    /* Function communication block */
        SU4PCMU08,                      /* usSrcFmt                     */
        0L,                             /* ulFilCnt                     */
        "*.hdr",                        /* szHdrSpc                     */
};

/************************************************************************/
/************************************************************************/
DWORD far pascal Su4RepHdr (short, short, LPCSTR, LPCSTR, LPSTR, WORD, void far *); 

/************************************************************************/
/************************************************************************/
BOOL  FAR PASCAL Su4ChkFil (LPCSTR szFilLst, REPBLK FAR *lpRepBlk)
{
    MsgDspUsr ("%Fs ", szFilLst);
    return (TRUE);
}

BOOL  FAR PASCAL Su4EndFil (DWORD ulRepCnt, REPBLK FAR *lpRepBlk)
{
    MsgDspUsr ("\n");
    return (FALSE);
}

DWORD FAR PASCAL Su4RepVox (short sSrcHdl, short sDstHdl, LPCSTR szSrcFil, 
                 LPCSTR szDstFil, LPSTR lpWrkBuf, WORD usBufSiz, REPBLK FAR *lpRepBlk)
{
    WORD        usBytCnt;
    SU4HDR      shSu4Hdr;
    DWORD       ulXfrCnt = 0L;
    unsigned    uiErrCod;
    
    /********************************************************************/
    /* Open and check Su4 format input file                             */
    /********************************************************************/
    usBytCnt = Rd_FilFwd (sSrcHdl, &shSu4Hdr, sizeof (shSu4Hdr), FIOENCNON, &uiErrCod);
    if ((usBytCnt != sizeof (shSu4Hdr)) || (SU4HDRFMT != DW_SWP(shSu4Hdr.ulHdrFmt))) {
        MsgDspUsr ("Invalid header, skipping... "); 
        return (0L);
    }
    ulXfrCnt = Su4CopCvt (sSrcHdl, sDstHdl, lpWrkBuf, usBufSiz, 
        lpRepBlk->usSrcFmt, SU4PCMU08, &uiErrCod);

    /********************************************************************/
    /* Extract Header                                                   */
    /********************************************************************/
    ulXfrCnt += RepSrcToD (szSrcFil, lpRepBlk->szHdrSpc, 0, lpWrkBuf, 
                usBufSiz, Su4RepHdr, lpRepBlk);
    
    /********************************************************************/
    /********************************************************************/
    lpRepBlk->ulFilCnt++;
    return (ulXfrCnt);

}

DWORD far pascal Su4RepHdr (short sSrcHdl, short sDstHdl, LPCSTR szSrcFil, 
        LPCSTR szDstFil, LPSTR lpWrkBuf, WORD usBufSiz, REPBLK *lpRepBlk)
{
    WORD        usBytCnt;
    SU4HDR      shSu4Hdr;
    unsigned    uiErrCod;
    
    /********************************************************************/
    /* Open and check Su4 format file, write header to separate file    */
    /********************************************************************/
    usBytCnt = Rd_FilFwd (sSrcHdl, &shSu4Hdr, sizeof (shSu4Hdr), FIOENCNON, &uiErrCod);
    return (Wr_FilFwd (sDstHdl, &shSu4Hdr, usBytCnt, FIOENCNON, &uiErrCod));

}



