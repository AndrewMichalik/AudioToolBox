/************************************************************************/
/* Indexed File Utility Initialization: IdxIni.c        V2.00  08/15/95 */
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
#include "..\..\fiodev\filutl.h"        /* File utilities definitions   */
#include "..\..\pcmdev\genpcm.h"        /* PCM/APCM conv routine defs   */
#include "..\..\regdev\tbxreg.h"        /* User Registration defs       */
#include "..\..\regdev\tbxlic.h"        /* User Licensing defs          */
#include "..\..\os_dev\cfgsup.h"        /* Configuration support        */
#include "genidx.h"                     /* Indexed file defs            */
#include "libsup.h"                     /* Indexed file lib supp        */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */
#include <sys\stat.h>                   /* File status types            */

/************************************************************************/
/*                  Global & External References                        */
/************************************************************************/
TBXGLO  TBxGlo = {                      /* ToolBox Application Globals  */
    "IdxFil",                           /* Library name                 */
    "03.00",                            /* Library version              */
    {'s','r','0','0','z','h','a','0'},  /* Operating/Demo Only Key      */
//    {'s','r','0','0','j','4','a','0'},  /* License Key: S/N 1000        */
    {'1','0','0','0'},                  /* Op/Demo sequence number      */
    {'0','0','0','0'},                  /* Security check sum           */
    REGDEMKEY,                          /* usRegKey                     */
    ERR___DEB,                          /* Debug errors flag            */
    (HANDLE) NULL,                      /* Global instance handle       */
    (HANDLE) NULL,                      /* License dialog window handle */
};

/************************************************************************/
/************************************************************************/
#if (defined (W31)) /****************************************************/

/************************************************************************/
/************************************************************************/
int FAR PASCAL LibMain (HANDLE hLibIns, WORD usDatSeg, WORD usHeapSz,
               LPSTR lpCmdLin)
{

    /********************************************************************/
    /********************************************************************/
    if (usHeapSz != 0) UnlockData(0);
    TBxGlo.hLibIns = hLibIns;

    /********************************************************************/
    /********************************************************************/
    return (1);

}

/************************************************************************/
/************************************************************************/
int FAR PASCAL WEP (int nParam)
{
//ajm 10/01/96
if (TBxGlo.hLicDlg) SendMessage (TBxGlo.hLicDlg, WM_CLOSE, 0, 0L);
    return (1);
}
#endif /*****************************************************************/

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL IdxFilVer ()
{
    return (IDXVERNUM);
}
WORD FAR PASCAL IdxFilIni (WORD usReqTyp, DWORD ulPrm001, DWORD ulPrm002)
{
    return (SI_IDXNO_ERR);
}
WORD FAR PASCAL IdxFilTrm ()
{
    return (0);
}

/************************************************************************/
/*                  Configuration Parameter Support                     */
/************************************************************************/
WORD FAR PASCAL IdxCfgIni (IDXBLK FAR *lpIdxBlk)
{
    _fmemset (lpIdxBlk, 0, sizeof (*lpIdxBlk));
    return (0);
}

WORD FAR PASCAL IdxCfgLod (LPSTR szCfgFil, VISMEMHDL FAR *pmhCfgMem)
{
    LPVOID       lpCfgBuf;
    struct _stat stFilSta;

    /********************************************************************/
    /********************************************************************/
    if (0 != FilGetSta (szCfgFil, &stFilSta)) return ((WORD) -1);
    if (NULL == (lpCfgBuf = GloAloLck (GMEM_MOVEABLE, pmhCfgMem, 
        min (stFilSta.st_size, CFGMAXBUF)))) return ((WORD) -1);
    
    /********************************************************************/
    /* Load szMapFil and compress all printables into LF term records   */
    /********************************************************************/
    if (CfgFilLod (szCfgFil, lpCfgBuf, CFGMAXBUF)) return ((WORD) -1);  

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (*pmhCfgMem);
    return (0);

}

WORD FAR PASCAL IdxCfgRel (VISMEMHDL FAR *pmhCfgMem)
{
    return (((VISMEMHDL) NULL) != (*pmhCfgMem = GloAloRel (*pmhCfgMem)));
}

DWORD FAR PASCAL IdxCapQry (PCMTYP usPCMTyp, DWORD ulCapTyp, DWORD ulRsv001)
{
    return ((DWORD) -1L);
}

/************************************************************************/
/************************************************************************/
WORD    FAR PASCAL IdxSetDeb (WORD usDebFlg)
{
    WORD    usDebOrg = TBxGlo.usDebFlg;
    TBxGlo.usDebFlg = usDebFlg;
    return (usDebOrg);
}

LPVOID  FAR PASCAL IdxSetDsp (MSGDSPPRC lpMsgDsp)
{
    MSGDSPPRC   lpMsgOrg = MsgDspPrc;
    MsgDspPrc = lpMsgDsp;
    return (lpMsgOrg);
}

/************************************************************************/
/*                  Configuration File Parameter Support                */
/************************************************************************/
int GetCfgFil (LPSTR szCfgFil, VISMEMHDL FAR *pmhCfgMem)
{
    LPVOID       lpCfgBuf;
    struct _stat stFilSta;

    /********************************************************************/
    /********************************************************************/
    if (0 != FilGetSta (szCfgFil, &stFilSta)) return (-1);
    if (NULL == (lpCfgBuf = GloAloLck (GMEM_MOVEABLE, pmhCfgMem, 
        min (stFilSta.st_size, CFGMAXBUF)))) return (-1);
    
    /********************************************************************/
    /* Load szMapFil and compress all printables into LF term records   */
    /********************************************************************/
    if (CfgFilLod (szCfgFil, lpCfgBuf, CFGMAXBUF)) return (-1);  

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (*pmhCfgMem);
    return (0);

}

int RelCfgFil (VISMEMHDL FAR *pmhCfgMem)
{
    return ((int) (*pmhCfgMem = GloAloRel (*pmhCfgMem)));
}

/************************************************************************/
/*                  Registration and Accelerator Support                */
/************************************************************************/
#include "..\..\regdev\tbxreg.c"            /* User Registration code   */
#include "..\..\regdev\tbxlic.c"            /* Licensing functions      */
#if (defined (W31)) /****************************************************/
    #define _hInst  TBxGlo.hLibIns    
    #include "..\..\regdev\kescb.c"         /* Registration key code    */
#endif /*****************************************************************/
