/************************************************************************/
/* Generic Frequency Effects Utility: GenEff.c          V2.00  03/01/93 */
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
#include "geneff.h"                     /* Frequency effects suppurt    */
#include "libsup.h"                     /* Frequency effects lib supp   */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */
#include <stdio.h>                      /* Standard I/O                 */
  
/************************************************************************/
/*                  Global & External References                        */
/************************************************************************/
extern  char  * MsgUseIni;
extern  char  * MsgDspUse;
extern  char  * MsgDspHlp;
extern  CFGBLK  cbCfgBlk;               /* Configuration file block     */
extern  FRQBLK  fbFrqBlk;               /* Function communication block */

/************************************************************************/
/*                          Forward References                          */
/************************************************************************/
BOOL    FAR PASCAL  RepChkFil (LPCSTR, LPVOID); 
DWORD   FAR PASCAL  RepPCMFil (short, short, LPCSTR, LPCSTR, LPSTR, WORD, LPVOID); 
BOOL    FAR PASCAL  RepEndFil (DWORD, LPVOID); 
void    UseErrEnd   (char *);
void    DspHlpEnd   (void);

/************************************************************************/
/************************************************************************/
void main (int argc, char **argv, char **envp)
{
    char    szSrcFil[FIOMAXNAM] = {'\0'};
    char    szTrgSpc[FIOMAXNAM] = {'\0'};
    char   *pcWrkPtr;

    /********************************************************************/
    /********************************************************************/
    printf (MsgUseIni);

    /********************************************************************/
    /* Check argument count. Skip program name                          */
    /********************************************************************/
    if (strncmp (*++argv, "-h", 2) == 0) DspHlpEnd ();
    if (argc < 2) UseErrEnd (NULL);

    /********************************************************************/
    /* Get name of source file                                          */
    /********************************************************************/
    _fstrncpy (szSrcFil, *argv++, FIOMAXNAM);
    argc = argc - 2;                    /* Indicate name & 1 arg used   */

    /********************************************************************/
    /* Get name of new file name (if specified)                         */
    /********************************************************************/
    if ((0 != argc) && (NULL == strchr (*argv, '-'))) {
        _fstrncpy (szTrgSpc, *argv++, FIOMAXNAM);
        argc--;
    }

    /********************************************************************/
    /* Initialize and allocate effects resources                        */
    /********************************************************************/
    if (EffFrqIni (NULL, &pcWrkPtr)) UseErrEnd (pcWrkPtr);
                                          
    /********************************************************************/
    /* Get dash parameters                                              */
    /********************************************************************/
    if (pcWrkPtr = GetDshPrm (argc, argv, &cbCfgBlk, &fbFrqBlk)) 
        UseErrEnd (pcWrkPtr);

    /********************************************************************/
    /********************************************************************/
    BinFilRep (szSrcFil, szTrgSpc, cbCfgBlk.usRepFlg, RepChkFil, RepPCMFil, 
        RepEndFil, NULL, &fbFrqBlk);
    printf ("%ld converted\n", fbFrqBlk.ulTotCnt);

    /********************************************************************/
    /* Free effects resources                                           */
    /********************************************************************/
    EffFrqTrm ();

    /********************************************************************/
    /********************************************************************/
    exit (0);

}

/************************************************************************/
/************************************************************************/
BOOL FAR PASCAL RepChkFil (LPCSTR szFilLst, FRQBLK FAR *lpFrqBlk)
{
    MsgDspUsr ("%Fs ", szFilLst);
    return (EffChkFil (szFilLst, lpFrqBlk));
}

DWORD FAR PASCAL RepPCMFil (short sSrcHdl, short sDstHdl, LPCSTR szSrcFil, 
                 LPCSTR szDstFil, LPSTR lpWrkBuf, WORD usBufSiz, FRQBLK FAR *lpFrqBlk)
{
    return (EffPCMFil (sSrcHdl, sDstHdl, szSrcFil, szDstFil, lpWrkBuf, usBufSiz, lpFrqBlk));
}

BOOL FAR PASCAL RepEndFil (DWORD ulRepCnt, FRQBLK FAR *lpFrqBlk)
{
    MsgDspUsr ("\n");
    return (EffEndFil (ulRepCnt, lpFrqBlk));
}

/************************************************************************/
/************************************************************************/
void    UseErrEnd (char *szErrTxt)
{
    if (szErrTxt != NULL) printf ("Bad Switch: %s\n", szErrTxt);
    printf (MsgDspUse);
    exit (USEERREND);
}

void    DspHlpEnd (void)
{
    /********************************************************************/
    /********************************************************************/
    printf ("\n");
    printf (MsgDspUse);
    printf (MsgDspHlp);
    exit (0);
}

