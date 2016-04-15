/************************************************************************/
/* Generic Sound Effects Support: GenEff.c              V2.00  03/15/95 */
/* Copyright (c) 1987-1995 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#include "..\os_dev\winmem.h"           /* Generic memory supp defs     */
#include "..\os_dev\winmsg.h"           /* User message support         */
#include "geneff.h"                     /* Sound Effects definitions    */
#include "effmsg.h"                     /* Effects support error defs   */
#include "effsup.h"                     /* Effects support definitions  */

/************************************************************************/
/*                      Global variables                                */
/************************************************************************/
EFFGLO EffGlo = {                       /* Effects Library Globals      */
    SI_EFFLIBNAM,                       /* Library name                 */
    {'0','0','0','0','0','0','0','0'},  /* Operating/Demo Only Key      */
    {'1','0','0','0'},                  /* Op/Demo sequence number      */
    NULL,                               /* Global instance handle       */
};

/************************************************************************/
/************************************************************************/
int FAR PASCAL LibMain (HANDLE hLibIns, WORD usDatSeg, WORD usHeapSz,
               LPSTR lpCmdLin)
{

    /********************************************************************/
    /********************************************************************/
    if (usHeapSz != 0) UnlockData(0);
    EffGlo.hLibIns = hLibIns;

    /********************************************************************/
    /********************************************************************/
    EffResIni (EffGlo.hLibIns);

    /********************************************************************/
    /********************************************************************/
    return (1);

}

int FAR PASCAL WEP (int nParam)
{
    EffResTrm (EffGlo.hLibIns);
    return (1);
}

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL EffDLLIni (WORD usReqTyp, DWORD ulPrm001, DWORD ulPrm002) 
{
    /********************************************************************/
    /********************************************************************/
    if (usReqTyp == 0) {
        // Check for accelerator key
    }

    return (0);
}

WORD FAR PASCAL EffSupVer ()
{
    /********************************************************************/
    /********************************************************************/
    return (EFFVERNUM);
}

/************************************************************************/
/************************************************************************/
WORD    EffResIni (HANDLE hLibIns)
{
    /********************************************************************/
    /********************************************************************/
    MsgDspPrc = EffDspPrc;              /* Re-assignable msg output     */
    return (0);
}

WORD    EffResTrm (HANDLE hLibIns)
{
    /********************************************************************/
    /********************************************************************/
    return (0);
}

/************************************************************************/
/************************************************************************/
int FAR PASCAL  EffDspPrc (HWND hWnd, LPCSTR lpMsgTxt, LPCSTR lpTtlTxt, UINT uiStyFlg)
{
    return (MessageBox (hWnd, lpMsgTxt, EffGlo.szLibNam, uiStyFlg));
}


