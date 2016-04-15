/************************************************************************/
/* Summa Four Packing Utility: Su4Pak.c                 V1.00  08/10/93 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "..\fiodev\filutl.h"           /* File utilities definitions   */
#include "..\os_dev\dosmsg.h"           /* User message support defs    */
#include "su4hdr.h"                     /* Summa Four header support    */
#include "su4pak.h"                     /* Summa Four pack support      */

#include <string.h>                     /* String manipulation funcs    */
#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <stdio.h>                      /* Standard I/O                 */
  
/************************************************************************/
/*                  Forward & External References                       */
/************************************************************************/
extern  char  * MsgUseIni;
extern  char  * MsgDspUse;
extern  char  * MsgDspHlp;

/************************************************************************/
/************************************************************************/
void    UseErrEnd (char *);
void    DspHlpEnd (void);

/************************************************************************/
/************************************************************************/
TBXGLO TBxGlo = {                       /* ToolBox Application Globals  */
    "Su4Pak",                           /* Application name             */
    {'0','0','0','0','0','0','0','0'},  /* Operating/Demo Only Key      */
    {'1','0','0','0'},                  /* Op/Demo sequence number      */
};
extern  REPBLK  rbRepBlk;

/************************************************************************/
/************************************************************************/
void main (int argc, char **argv, char **envp)
{
    char    szCurDir[FIOMAXNAM] = {'\0'};
    char    szAppDir[FIOMAXNAM] = {'\0'};
    char    szSrcFil[FIOMAXNAM] = {'\0'};
    char    szTrgSpc[FIOMAXNAM] = {'\0'};
    WORD    usRepFlg = 0;
    WORD    usi;

    /********************************************************************/
    /********************************************************************/
    printf (MsgUseIni);

    /********************************************************************/
    /* Get current working directory (with trailing slash).             */
    /* Get application executable directory; remove program file name.  */
    /********************************************************************/
    xgetdcwd (szCurDir, FIOMAXNAM - 1);
    usi = _fstrlen (_fstrncpy (szAppDir, *argv, FIOMAXNAM));
    while (usi--) if ('\\' == szAppDir[usi]) {
        szAppDir[usi+1] = '\0';        
        break;
    }    

    /********************************************************************/
    /* Check argument count. Skip program name                          */
    /********************************************************************/
    if (strncmp (*++argv, "-h", 2) == 0) DspHlpEnd ();
    if (argc < 2) UseErrEnd (NULL);

    /********************************************************************/
    /* Get name of source file                                          */
    /********************************************************************/
    strncpy (szSrcFil, *argv++, FIOMAXNAM);
    argc = argc - 2;                    /* Indicate name & 1 arg used   */

    /********************************************************************/
    /* Get name of new file name (if specified)                         */
    /********************************************************************/
    if ((0 != argc) && (NULL == strchr (*argv, '-'))) {
        strncpy (szTrgSpc, *argv++, FIOMAXNAM);
        argc--;
    }

    /********************************************************************/
    /********************************************************************/
    IniSu4Hdr (&rbRepBlk.shSu4Hdr);

    /********************************************************************/
    /********************************************************************/
    while (0 < argc--) {
        if (!strcmp (*argv, "-a")) rbRepBlk.shSu4Hdr.ulEncFmt = DW_SWP(SU4PCMA08);
          else if (!strcmp (*argv, "-b")) usRepFlg |= BFRBKIFLG; 
            else if (!strncmp (*argv, "-d", 2)) strncpy (rbRepBlk.shSu4Hdr.ucFilDes, &(*argv)[2], SU4DESLEN);
              else if (!strncmp (*argv, "-i", 2)) rbRepBlk.shSu4Hdr.ulFil_ID = DW_SWP((DWORD) atol (&(*argv)[2]));
                else if (!strncmp (*argv, "-l", 2)) rbRepBlk.shSu4Hdr.ulLibCod = DW_SWP((DWORD) atol (&(*argv)[2]));
                  else UseErrEnd (*argv);
        *argv++;
    }

    /********************************************************************/
    /********************************************************************/
    BinFilRep (szSrcFil, szTrgSpc, usRepFlg, Su4ChkFil, Su4RepFil, Su4EndFil, NULL, &rbRepBlk);
    printf ("%ld converted\n", rbRepBlk.ulFilCnt);
    exit (0);

}

void    UseErrEnd (char *SError)
{
    if (SError != NULL) printf ("Bad Switch: %s\n", SError);
    printf (MsgDspUse);
    exit (USEERREND);
}

void    DspHlpEnd (void)
{
    printf (MsgDspUse);
    printf (MsgDspHlp);
    exit (0);

}


