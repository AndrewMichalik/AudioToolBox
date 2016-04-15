/************************************************************************/
/* Sound Chop Utility Initialization: ChpIni.c          V2.00  08/15/95 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
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
#include "..\..\pcmdev\pcmsup.h"        /* PCM/APCM xlate lib defs      */
#include "..\..\effdev\geneff.h"        /* Sound Effects definitions    */
#include "..\..\regdev\tbxreg.h"        /* User Registration defs       */
#include "..\..\regdev\tbxlic.h"        /* User Licensing defs          */
#include "..\..\os_dev\cfgsup.h"        /* Configuration support        */
#include "genchp.h"                     /* PCM conversion defs          */
#include "libsup.h"                     /* PCM conversion lib supp      */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */
#include <sys\stat.h>                   /* File status types            */

/************************************************************************/
/*                  Global & External References                        */
/************************************************************************/
TBXGLO TBxGlo = {                       /* ToolBox Application Globals  */
    "SndChp",                           /* Library name                 */
    "03.00",                            /* Library version              */
    {'s','r','0','0','z','h','a','0'},  /* Operating/Demo Only Key      */
//    {'0','u','6','0','v','b','a','0'},  /* Evaluation Key: 3/31/94     */
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
WORD FAR PASCAL ChpSndVer ()
{
    return (CHPVERNUM);
}
WORD FAR PASCAL ChpSndIni (WORD usReqTyp, DWORD ulPrm001, DWORD ulPrm002)
{
    WORD    usRetCod = SI_CHPNO_ERR;
    WORD    usCmpCod = 0;

    /********************************************************************/
    /* Initialize PCM Conversion (DOS & Win support)                    */
    /********************************************************************/
#if (defined (DOS)) /****************************************************/
    PCMResIni (0);
#endif /*****************************************************************/
#if (defined (W31)) /****************************************************/
    PCMDLLIni (0, 0L, (DWORD) (LPVOID) "t2c09p03");
#endif /*****************************************************************/

    /********************************************************************/
    /* Check the version numbers of support modules (Win DLL's)         */
    /********************************************************************/
#if (defined (W31)) /****************************************************/
    if (EFFVERNUM != EffSupVer()) {
        if (TBxGlo.usDebFlg & INI___DEB) MsgDspUsr (SI_MODVERMIS, SI_EFFDLLSUP);
        usRetCod |= SI_CHPVERERR;
    }
    if (PCMVERNUM != PCMSupVer()) {
        if (TBxGlo.usDebFlg & INI___DEB) MsgDspUsr (SI_MODVERMIS, SI_PCMDLLSUP);
        usRetCod |= SI_CHPVERERR;
    }
#endif /*****************************************************************/

//ajm test
//TBxGlo.usRegKey = REGFULKEY;
//return (SI_CHPKEYERR);

    /********************************************************************/
    /* If (0 == usReqTyp), check for accelerator/license/customer key   */
    /********************************************************************/
    if (!usReqTyp && (!ulPrm001 || !_fstrlen ((LPSTR) ulPrm001))
                  && (!ulPrm002 || !_fstrlen ((LPSTR) ulPrm002))) {
        /****************************************************************/
        /* Check for accelerator key and/or AppKey/UsrKey combo         */
        /****************************************************************/
        if (REGDEMKEY == ChkRegKey (TBxGlo.RelKeyArr, TBxGlo.SeqNumArr, &usCmpCod)) {
            switch (usCmpCod) {
                case 0xFFFE:
                    if (TBxGlo.usDebFlg & INI___DEB) 
                        MsgDspUsr (SI_INSMEMERR);
                    break;
                case 0xFFFD:
                    if (TBxGlo.usDebFlg & INI___DEB) 
                        MsgDspUsr (SI_DOSMEMERR);
                    break;
                default:
                    if (TBxGlo.usDebFlg & INI___DEB) 
                        MsgDspUsr (SI_ACCKEYNON);
            } 
            return (usRetCod | SI_CHPKEYERR);
        } 
        /****************************************************************/
        /* Load registration key from physical key [1] or string.       */ 
        /****************************************************************/
        if (!usCmpCod) TBxGlo.usRegKey = GetEncKey (TBxGlo.SeqNumArr, 1, &usCmpCod);
        else TBxGlo.usRegKey = ChkRegKey (TBxGlo.RelKeyArr, TBxGlo.SeqNumArr, &usCmpCod);
        return (usRetCod | SI_CHPNO_ERR);
    }
    if (!usReqTyp && ulPrm001 && _fstrlen ((LPSTR) ulPrm001)) {
        /****************************************************************/
        /* Check license file key, return error.                        */
        /****************************************************************/
        TBxGlo.usRegKey = ChkLicFil ((LPSTR) ulPrm001, NULL, &usCmpCod);
        return (usRetCod | (usCmpCod ? SI_CHPKEYERR : SI_CHPNO_ERR));
    }
    if (!usReqTyp && ulPrm002 && _fstrlen ((LPSTR) ulPrm002)) {
        /****************************************************************/
        /* Check customer key, do not return error for better security. */
        /****************************************************************/
        TBxGlo.usRegKey = ChkRegKey ((LPSTR) ulPrm002, TBxGlo.SeqNumArr, &usCmpCod);
    }

    /********************************************************************/
    /********************************************************************/
    return (usRetCod | SI_CHPNO_ERR);

}
WORD FAR PASCAL ChpSndTrm ()
{
    /********************************************************************/
    /********************************************************************/
    return (0);
}

/************************************************************************/
/*                  Configuration Parameter Support                     */
/************************************************************************/
WORD FAR PASCAL ChpCfgIni (CHPBLK FAR *lpChpBlk)
{
    _fmemset (lpChpBlk, 0, sizeof (*lpChpBlk));
    return (0);
}

WORD FAR PASCAL ChpCfgLod (LPSTR szCfgFil, VISMEMHDL FAR *pmhCfgMem)
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

WORD FAR PASCAL ChpCfgRel (VISMEMHDL FAR *pmhCfgMem)
{
    return (((VISMEMHDL) NULL) != (*pmhCfgMem = GloAloRel (*pmhCfgMem)));
}

DWORD FAR PASCAL ChpCapQry (PCMTYP usPCMTyp, DWORD ulCapTyp, DWORD ulRsv001)
{
    return (PCMCapQry (usPCMTyp, ulCapTyp, 0L));
}

/************************************************************************/
/************************************************************************/
WORD    FAR PASCAL ChpSetDeb (WORD usDebFlg)
{
    WORD    usDebOrg = TBxGlo.usDebFlg;
    TBxGlo.usDebFlg = usDebFlg;
    return (usDebOrg);
}

LPVOID  FAR PASCAL ChpSetDsp (MSGDSPPRC lpMsgDsp)
{
    MSGDSPPRC   lpMsgOrg = MsgDspPrc;
    MsgDspPrc = lpMsgDsp;
    return (lpMsgOrg);
}

/************************************************************************/
/*                  Registration and Accelerator Support                */
/************************************************************************/
#include "..\..\regdev\tbxreg.c"            /* User Registration code   */
#include "..\..\regdev\tbxlic.c"            /* Licensing functions      */
#if (defined (W31)) /****************************************************/
    #define _hInst          TBxGlo.hLibIns    
    #include "..\..\regdev\kescb.c"         /* Registration key code    */
#endif /*****************************************************************/

