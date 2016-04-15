/************************************************************************/
/* Binary File Replace: BinRep                          V1.20  11/20/91 */
/* Copyright (c) 1987-1991 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "..\os_dev\dosmsg.h"           /* User message support defs    */
#include "..\fiodev\filutl.h"           /* File utilities definitions   */
#include "binrep.h"                     /* Binary file replacement defs */

#include <string.h>                     /* String manipulation funcs    */
#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <ctype.h>                      /* Char classification funcs    */
  
/************************************************************************/
/*                  Forward & External References                       */
/************************************************************************/
void    UseErrEnd (char *);
void    DspHlpEnd (void);
int     EscStrCvt (BYTE *, BYTE *, WORD *, WORD);

/************************************************************************/
/************************************************************************/
char  * MsgUseIni =  
"\n\
  Voice Information Systems (1-800-234-VISI) Binary File Replacement Utility\n\
             v3.00a.1 (C) Copyright Andrew J. Michalik 1987-1996\n\n";

/************************************************************************/
/************************************************************************/
static  BYTE    baSrcArr[MAXARRLEN];    /* Source replacement bytes     */
static  BYTE    baTrgArr[MAXARRLEN];    /* Dest   replacement bytes     */

static  REPBLK  rbRepBlk = {            /* Function communication block */
        baSrcArr,                       /* pbSrcArr                     */
        0,                              /* usSrcSiz                     */
        baTrgArr,                       /* pbTrgArr                     */
        0,                              /* usTrgSiz                     */
        0L,                             /* ulTotCnt                     */
};                                                                      

/************************************************************************/
/************************************************************************/
void main (int argc, char **argv, char **envp)
{
    char    szSrcFil[FIOMAXNAM] = {'\0'};
    char    szTrgSpc[FIOMAXNAM] = {'\0'};
    WORD    usRepFlg = 0;
    int     iRetCod;

    /********************************************************************/
    /********************************************************************/
    MsgDspUsr (MsgUseIni);

    /********************************************************************/
    /* Check argument count. Skip program name                          */
    /********************************************************************/
    if (argc <= 1) UseErrEnd (NULL);
    if (strncmp (*++argv, "-h", 2) == 0) DspHlpEnd ();
    if (argc <= 3) UseErrEnd (NULL);
    argc = argc - 4;                    /* Indicate name & 3 args used  */

    /********************************************************************/
    /* Convert "escaped" replacement string into character array        */
    /********************************************************************/
    if (0 != (iRetCod = EscStrCvt (baSrcArr, *argv++, &rbRepBlk.usSrcSiz, MAXARRLEN))) {
        if (iRetCod == 1) MsgDspUsr ("OldString argument too long\n");
        if (iRetCod == 2) MsgDspUsr ("Bad character in OldString argument\n");
        exit (BADOLDSTR);
    }
    if (0 != (iRetCod = EscStrCvt (baTrgArr, *argv++, &rbRepBlk.usTrgSiz, MAXARRLEN))) {
        if (iRetCod == 1) MsgDspUsr ("NewString argument too long\n");
        if (iRetCod == 2) MsgDspUsr ("Bad character in NewString argument\n");
        exit (BADNEWSTR);
    }

    /********************************************************************/
    /* Get name of source file                                          */
    /********************************************************************/
    strcpy (szSrcFil, *argv++);

    /********************************************************************/
    /* Get name of new file (if specified)                              */
    /********************************************************************/
    if ((0 != argc) && (NULL == strchr (*argv, '-'))) {
        strncpy (szTrgSpc, *argv++, FIOMAXNAM);
        argc--;
    }
    else *szTrgSpc = '\0';

    /********************************************************************/
    /* Get dash parameters                                              */
    /********************************************************************/
    while (0 < argc--) {
        if (!strncmp (*argv, "-b", 2)) usRepFlg |= BFRBKIFLG; 
          else if (!strncmp (*argv, "-d", 2)) usRepFlg |= BFRDATFLG; 
            else UseErrEnd (*argv);
        *argv++;
    }

    /********************************************************************/
    /********************************************************************/
    BinFilRep (szSrcFil, szTrgSpc, usRepFlg, ArrChkFil, ArrRepFil, ArrEndFil, 
        NULL, &rbRepBlk);
    MsgDspUsr ("%ld replaced\n", rbRepBlk.ulTotCnt);

    /********************************************************************/
    /********************************************************************/
    exit (0);

}

void    UseErrEnd (char *SError)
{
    if (SError != NULL) MsgDspUsr ("Bad Switch: %s\n", SError);
    MsgDspUsr ("Usage: bfr [-help] OldStr NewStr OldFile [NewFile] [-b -d]\n");
    exit (USEERREND);
}

void    DspHlpEnd (void)
{
    MsgDspUsr ("Usage: bfr [-help] OldStr NewStr OldFile [NewFile] [-b -d]\n");
    MsgDspUsr ("       OldStr: Original string.\n");
    MsgDspUsr ("       NewStr: Replacement string (\"\" = none).\n");
    MsgDspUsr ("       OldFile: Source filename.\n");
    MsgDspUsr ("       NewFile: Target filename, if different from OldFile.\n");
    MsgDspUsr ("       -b : Backup file creation inhibited.\n"); 
    MsgDspUsr ("       -d : Date preserved from OldFile.\n"); 
    MsgDspUsr ("Notes: Environment TMP= specifies temp work files directory.\n");
    MsgDspUsr ("       Use \\xx (xx = hex value) for special characters.\n");
    MsgDspUsr ("       \"\\\" = 5c, \" \" = 20\n");

    exit (0);

}

/************************************************************************/
/************************************************************************/
ahxtoi (char cHexChr)
{
    cHexChr = (char) toupper (cHexChr);
    if ((cHexChr >= '0') & (cHexChr <= '9')) return (cHexChr - 0x30);
    if ((cHexChr >= 'A') & (cHexChr <= 'F')) return (0x0a + cHexChr - 0x41);
    return (0);
}

EscStrCvt (BYTE *pbDstArr, BYTE *pbSrcArr, WORD *pusDstSiz, WORD usMaxSiz)
{
    char    HexVal[2]; 

    *pusDstSiz = 0;
    while (*pbSrcArr != '\0') {
        if (++*pusDstSiz > usMaxSiz) return (1);
        if (*pbSrcArr != '\\') *pbDstArr++ = *pbSrcArr++;
        else {
            if (!isxdigit (HexVal[0] = *++pbSrcArr)) return (2);
            if (!isxdigit (HexVal[1] = *++pbSrcArr)) return (2);
            pbSrcArr++;
            *pbDstArr = (BYTE) ahxtoi (HexVal[0]);
            *pbDstArr = (BYTE) ((*pbDstArr << 4) + ahxtoi (HexVal[1]));
            pbDstArr++;
        }            
    }

    return (0);

}    





