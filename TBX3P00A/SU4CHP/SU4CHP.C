/************************************************************************/
/* Summa Four Packing Utility: Su4Pak.c                 V1.00  09/10/92 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "..\fiodev\filutl.h"           /* File utilities definitions   */
#include "..\os_dev\dosmsg.h"           /* User message support defs    */
#include "su4hdr.h"                     /* Summa Four header support    */
#include "su4chp.h"                     /* Summa Four pack support      */

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
    "Su4Chp",                           /* Application name             */
    {'0','0','0','0','0','0','0','0'},  /* Operating/Demo Only Key      */
    {'1','0','0','0'},                  /* Op/Demo sequence number      */
};
extern  REPBLK  rbRepBlk;               /* Function communication block */

/************************************************************************/
/************************************************************************/
void main (int argc, char **argv, char **envp)
{
    char    szCurDir[FIOMAXNAM] = {'\0'};
    char    szAppDir[FIOMAXNAM] = {'\0'};
    char    szSrcFil[FIOMAXNAM] = {'\0'};
    char    szVoxSpc[FIOMAXNAM] = {"*.vox"};
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
    /* Get Vox file string (if specified)                               */
    /********************************************************************/
    if ((0 != argc) && (NULL == strchr (*argv, '-'))) {
        strncpy (szVoxSpc, *argv++, FIOMAXNAM);
        argc--;
    }

    /********************************************************************/
    /* Get Hdr file string (if specified)                               */
    /********************************************************************/
    if ((0 != argc) && (NULL == strchr (*argv, '-'))) {
        strncpy (rbRepBlk.szHdrSpc, *argv++, FIOMAXNAM);
        argc--;
    }
    else strncpy (rbRepBlk.szHdrSpc, "*.hdr", FIOMAXNAM);

    /********************************************************************/
    /********************************************************************/
    while (0 < argc--) {
        if (!strncmp (*argv, "-a", 2)) rbRepBlk.usSrcFmt = SU4PCMA08; 
          else UseErrEnd (*argv);
        *argv++;
    }

    /********************************************************************/
    /********************************************************************/
    BinFilRep (szSrcFil, szVoxSpc, 0, Su4ChkFil, Su4RepVox, Su4EndFil, NULL, &rbRepBlk);
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