/************************************************************************/
/* Frequency Effects Initialization: EffIni.c           V2.00  01/10/93 */
/* Copyright (c) 1987-1995 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#if (defined (W31)) /****************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#include "..\os_dev\winmem.h"           /* Generic memory supp defs     */
#include "..\os_dev\winmsg.h"           /* User message support defs    */
#endif /*****************************************************************/
#if (defined (DOS)) /****************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "..\os_dev\dosmem.h"           /* Generic memory support defs  */
#include "..\os_dev\dosmsg.h"           /* User message support defs    */
#endif /*****************************************************************/
#include "..\fiodev\filutl.h"           /* File utilities definitions   */
#include "..\pcmdev\genpcm.h"           /* PCM/APCM conv routine defs   */
#include "..\pcmdev\pcmsup.h"           /* PCM/APCM xlate lib defs      */
#include "..\regdev\tbxreg.h"           /* User Registration defs       */
#include "..\os_dev\cfgsup.h"           /* Configuration support        */
#include "geneff.h"                     /* Frequency effects support    */
#include "libsup.h"                     /* Frequency effects lib supp   */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */
#include <sys\stat.h>                   /* File status types            */

/************************************************************************/
/*                  Global & External References                        */
/************************************************************************/
TBXGLO  TBxGlo = {                      /* ToolBox Application Globals  */
    "FrqEff",                           /* Application name             */
    "02.00",                            /* Library version              */
    {'s','r','0','0','z','h','a','0'},  /* Operating/Demo Only Key      */
//    {'0','u','6','0','v','b','a','0'},  /* Evaluation Key: 3/31/94     */
    {'1','0','0','0'},                  /* Op/Demo sequence number      */
    {'0','0','0','0'},                  /* Security check sum           */
    REGDEMKEY,                          /* usRegKey                     */
    ERR___DEB,                          /* Debug errors flag            */
};

/************************************************************************/
/*                          Forward References                          */
/************************************************************************/
int GetCfgFil (LPSTR, VISMEMHDL FAR *);
int RelCfgFil (VISMEMHDL FAR *);

/************************************************************************/
/************************************************************************/
#if (defined (W31)) /****************************************************/

/************************************************************************/
/************************************************************************/
HANDLE  hLibInstance;                   /* Global instance handle       */

/************************************************************************/
/************************************************************************/
int FAR PASCAL LibMain (HANDLE hLibIns, WORD usDatSeg, WORD usHeapSz,
               LPSTR lpCmdLin)
{

    /********************************************************************/
    /********************************************************************/
    if (usHeapSz != 0) UnlockData(0);
    hLibInstance = hLibIns;

    /********************************************************************/
    /********************************************************************/
    return (1);

}

/************************************************************************/
/************************************************************************/
int FAR PASCAL WEP (int nParam)
{

    return (1);

}
#endif /*****************************************************************/

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL EffFrqIni (LPSTR szFilNam, LPSTR FAR *ppcErrMsg)
{
    /********************************************************************/
    /* Initialize PCM Conversion (DOS support)                          */
    /********************************************************************/
#if (defined (W31)) /****************************************************/
    PCMDLLIni (0, 0L, (DWORD) (LPVOID) "t2c09p03");
#endif /*****************************************************************/
#if (defined (DOS)) /****************************************************/
    PCMResIni (0);
#endif /*****************************************************************/

    /********************************************************************/
    /********************************************************************/
//    if (REGDEMKEY == ChkRegKey (TBxGlo.RelKeyArr, TBxGlo.SeqNumArr)) {
//        MsgDspUsr ("Accelerator Key not installed, 8 sec limit.\n");
//    } else {
//        TBxGlo.usRegKey = GetEncKey (TBxGlo.SeqNumArr, 1);
//    }
TBxGlo.usRegKey = REGFULKEY;

//    /********************************************************************/
//    /* Get check sum; if invalid, skip file name input                  */
//    /********************************************************************/
//    GetChkSum (argv, TBxGlo.ChkSumArr);
//    if _fstrcmp (REGCHKSUM, TBxGlo.ChkSumArr) goto FakRun;

    /********************************************************************/
    /********************************************************************/
    return (0);

}
WORD FAR PASCAL EffFrqTrm ()
{
    /********************************************************************/
    /********************************************************************/
    return (0);
}

/************************************************************************/
/************************************************************************/
WORD    PASCAL FAR  EffSetDeb (WORD usDebFlg)
{
    return (TBxGlo.usDebFlg = usDebFlg);
}

WORD    PASCAL FAR  CvtSetDsp (LPVOID lpPrcPtr)
{
    return (0);
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
#include "..\regdev\tbxreg.c"               /* User Registration code   */
#if (defined (W31)) /****************************************************/
    HANDLE  hLibInstance = NULL;            /* Global instance handle   */
    #include "..\regdev\kescb.c"            /* Registration key code    */
#endif /*****************************************************************/

