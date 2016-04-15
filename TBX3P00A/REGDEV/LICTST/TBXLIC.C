/************************************************************************/
/* ToolBox Licensing: TBxLic.c                          V2.00  04/15/96 */
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
#include "..\..\os_dev\cfgsup.h"        /* Configuration support        */
#include "tbxlic.h"                     /* ToolBox licensing defs       */
#include "licsup.h"                     /* Licensing support defs       */
#include "licdlg.h"                     /* Licensing dialog defs        */
#include "lictst.h"

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */
#include <stdio.h>                      /* Standard I/O                 */
#include <ctype.h>						/* Char classification funcs    */
#include <sys\stat.h>                   /* File status types            */

/************************************************************************/
/*                          External References                         */
/************************************************************************/
extern  TBXGLO  TBxGlo;                 /* ToolBox Application Globals  */

/************************************************************************/
/*                      Forward References                              */
/************************************************************************/
static  long    _fatol  (char FAR *);

/************************************************************************/
/************************************************************************/
WORD    ChkLicFil (char far *szCfgFil, char far *pcSeqNumArr, WORD far *pusCmpCod)
{
    VISMEMHDL   mhCfgMem;
    LICDLGBLK   lbLicBlk;
    char        szRegSec[LICSTRLEN+1];
    char        szRegNam[LICSTRLEN+1];
    char        szRegCmp[LICSTRLEN+1];
    char        szRegSer[LICSTRLEN+1];
    char        szRegIns[LICSTRLEN+1];
    DWORD       ulRegIns = 0L;

    /********************************************************************/
    /********************************************************************/
    *pusCmpCod = 0;

    /********************************************************************/
    /* Initialize keyword strings                                       */
    /********************************************************************/
    LicStrXlt (szRegSec, "ShhkEhKndbitbb", LICSTRLEN);             // ToolBoxLicensee
    LicStrXlt (szRegNam, "Rtbu", LICSTRLEN);                        // User
    LicStrXlt (szRegCmp, "Dhjwfi~", LICSTRLEN);                     // Company
    LicStrXlt (szRegSer, "TbunfkIrjebu", LICSTRLEN);                // SerialNumber
    LicStrXlt (szRegIns, "NitsfkkfsnhiDofufdsbuntsndt", LICSTRLEN); // InstallationCharacteristics

    /********************************************************************/
    /* Generate License Display decoded strings                         */
    /********************************************************************/
    // About Audio ToolBox
    LicStrXlt (lbLicBlk.szTtlTxt, "Fehrs'Frcnh'ShhkEh", LICSTRLEN);
    // This copy is licensed to:
    LicStrXlt (lbLicBlk.szLicTxt, "Sont'dhw~'nt'kndbitbc'sh=", LICSTRLEN);
    // Audio ToolBox (tm) by Voice Information Systems, Inc.
    LicStrXlt (lbLicBlk.szPrdNam, "Frcnh'ShhkEh'/sj.'e~'Qhndb'Niahujfsnhi'T~tsbjt+'Nid)", LICSTRLEN);
    // Copyright © 1987-1996 A. J. Michalik  U.S. Pat. Pending
    LicStrXlt (lbLicBlk.szCopRgt, "Dhw~un`os'®'6>?0*6>>1'F)'M)'Jndofknl''R)T)'Wfs)'Wbicni`", LICSTRLEN);
    // 1-800-234-VISI / +310-392-8780 / www.vinfo.com
    LicStrXlt (lbLicBlk.szConAdr, "6*?77*543*QNTN'(',467*4>5*?0?7'('ppp)qniah)dhj", LICSTRLEN);

    /********************************************************************/
    /* Load license file; display copyright even if file not found.     */
    /********************************************************************/
    if (LicFilLod (szCfgFil, &mhCfgMem)) {
        if (TBxGlo.usDebFlg & KEY___DEB) 
            MsgDspUsr ("File I/O Error: %Fs\n", (LPSTR) szCfgFil);
        LicDlgDsp ("", "", "", &lbLicBlk);
        *pusCmpCod = LICCMPFIO;
        return (REGDEMKEY);
    }

    /********************************************************************/
    /* Get licensing strings                                            */
    /********************************************************************/
    CfgStrGet (mhCfgMem, szRegSec, szRegNam, "", szRegNam, LICSTRLEN);
    CfgStrGet (mhCfgMem, szRegSec, szRegCmp, "", szRegCmp, LICSTRLEN);
    CfgStrGet (mhCfgMem, szRegSec, szRegSer, "", szRegSer, LICSTRLEN);
    CfgStrGet (mhCfgMem, szRegSec, szRegIns, "", szRegIns, LICSTRLEN);

    /********************************************************************/
    /* Release license file data                                        */
    /********************************************************************/
    LicFilRel (&mhCfgMem);

    /********************************************************************/
    /* Display licensing information to user                            */
    /********************************************************************/
    if (!pcSeqNumArr || !_fstrlen (pcSeqNumArr))
        LicDlgDsp (szRegNam, szRegCmp, szRegSer, &lbLicBlk);

    /********************************************************************/
    /* Check installation number checksum                               */
    /********************************************************************/
    ulRegIns = ChkRegIns (0, ulRegIns, szRegNam);
    ulRegIns = ChkRegIns (0, ulRegIns, szRegCmp);
    ulRegIns = ChkRegIns (0, ulRegIns, szRegSer);
    if (ulRegIns != (DWORD) _fatol (szRegIns)) {
        if (TBxGlo.usDebFlg & KEY___DEB) 
            MsgDspUsr ("Characteristics = %lx.\n", ulRegIns);
        *pusCmpCod = LICCMPINS;
        return (REGDEMKEY);
    }

    /********************************************************************/
    /* Check for valid serial number                                    */
    /********************************************************************/
    if (!ChkRegSer (0, szRegSer) || !ChkRegSer (1, szRegSer)) {
        if (TBxGlo.usDebFlg & KEY___DEB) 
            MsgDspUsr ("Number = %lx.\n", ChkRegSer (1, szRegSer));
        *pusCmpCod = LICCMPSER;
        return (REGDEMKEY);
    }

    /********************************************************************/
    /********************************************************************/
    return (REGFULKEY);
}

/************************************************************************/
/*                      Support Functions                               */
/************************************************************************/
long _fatol  (char FAR * lpString)
{
    static char szWrkBuf[LICSTRLEN];

    /********************************************************************/
    /********************************************************************/
    _fstrncpy (szWrkBuf, lpString, min (LICSTRLEN, _fstrlen (lpString) + 1));

    /********************************************************************/
    /********************************************************************/
    return (atol (szWrkBuf));
}
#if (defined (W31)) /****************************************************/
    #include "licdlg.c"                 /* Licensing dialog functions   */
#endif /*****************************************************************/
#if (defined (DOS)) /****************************************************/
    void PrtCtrTxt (LPSTR szTxtStr)
    {
        WORD    usi;
        if (!(usi = szTxtStr ? _fstrlen (szTxtStr) : 0)) return;
        usi = (usi < 80) ? ((80 - usi) / 2) : 0;       
        while (usi--) printf (" ");
        printf ("%Fs\n", (LPSTR) szTxtStr);
    } 
    int LicDlgDsp (LPSTR szRegNam, LPSTR szRegCmp, LPSTR szRegSer, LICDLGBLK FAR *lpDlgBlk)
    {
        LICDLGBLK   lbLicBlk;
        /****************************************************************/
        /* Generate "Licensed To" decoded strings                       */
        /****************************************************************/
        // This copy (S/N 
        LicStrXlt (lbLicBlk.szLicTxt, "Sont'dhw~'/T(I'", LICSTRLEN);                         
        _fstrcat (lbLicBlk.szLicTxt, szRegSer);
        // ) is licensed to:
        LicStrXlt (&lbLicBlk.szLicTxt[_fstrlen (lbLicBlk.szLicTxt)], ".'nt'kndbitbc'sh=", LICSTRLEN); 

        /****************************************************************/
        /* Generate product name and copyright info decoded strings     */
        /****************************************************************/
        // Audio ToolBox (tm) by VISI, 1-800-234-VISI / +310-392-8780 / www.vinfo.com
        LicStrXlt (lbLicBlk.szPrdNam, "Frcnh'ShhkEh'/sj.'e~'QNTN+'6*?77*543*QNTN'(',467*4>5*?0?7'('ppp)qniah)dhj", LICSTRLEN);
        // Copyright (c) 1987-1996 A. J. Michalik  U.S. Pat. Pending
        LicStrXlt (lbLicBlk.szCopRgt, "Dhw~un`os'/d.'6>?0*6>>1'F)'M)'Jndofknl''R)T)'Wfs)'Wbicni`", LICSTRLEN);

        /****************************************************************/
        /* Print product name and copyright info                        */
        /****************************************************************/
        PrtCtrTxt (lbLicBlk.szPrdNam);
        PrtCtrTxt (lbLicBlk.szCopRgt);

        /****************************************************************/
        /* Print registration info (if any)                             */
        /****************************************************************/
        PrtCtrTxt (lbLicBlk.szLicTxt);
        PrtCtrTxt (szRegNam);
        PrtCtrTxt (szRegCmp);
        printf ("\n");

        /****************************************************************/
        /****************************************************************/
        return (0);
    }
#endif /*****************************************************************/

#include "licsup.c"                     /* Licensing support functions  */

