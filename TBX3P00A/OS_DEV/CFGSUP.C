/************************************************************************/
/* Configuration Param Config Support: CfgSup.c         V2.00  01/10/93 */
/* Copyright (c) 1987-1993 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#if (defined (W31)) /****************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#include "..\os_dev\winmem.h"           /* Generic memory supp defs     */
#endif /*****************************************************************/
#if (defined (DOS)) /****************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "..\os_dev\dosmem.h"           /* Generic memory support defs  */
#endif /*****************************************************************/
#include "..\fiodev\filutl.h"           /* File utilities definitions   */
#include "cfgsup.h"                     /* Configuration support        */

#include <stdlib.h>                     /* Standard library defs        */
#include <string.h>                     /* String manipulation          */
#include <io.h>                         /* Low level file I/O           */
#include <fcntl.h>                      /* Flags used in open/ sopen    */
#include <ctype.h>                      /* Char classification funcs    */

/************************************************************************/
/************************************************************************/
LPSTR   GetCfgSec (LPSTR, LPSTR);
LPSTR   CopCfgPrm (LPSTR, LPSTR, LPSTR, WORD);
LPSTR   strncpy_c (LPSTR, LPSTR, WORD, char);
long    _fstrtol  (LPSTR, LPSTR FAR *, int);
WORD    strnidx   (LPCSTR, LPCSTR, WORD);

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL CfgPrmMap (CFGMAP FAR *pcmCfgMap, LPSTR szPrmNam, 
                WORD FAR *pusCtlTyp)
{
    WORD    usWrkOff;

    /********************************************************************/
    /********************************************************************/
    usWrkOff = strnidx (pcmCfgMap->szNamLst, _fstrupr (szPrmNam),
        pcmCfgMap->usNamLen);
    if ((usWrkOff >= _fstrlen (pcmCfgMap->szNamLst))
        || (0 != usWrkOff % pcmCfgMap->usNamLen)) return ((WORD) -1);

    /********************************************************************/
    /********************************************************************/
    *pusCtlTyp = pcmCfgMap->usCfgVal[usWrkOff / pcmCfgMap->usNamLen];    
    return (0);

}

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL CfgFilLod (LPSTR szFilNam, LPSTR pcWrkBuf, WORD usBufSiz)
{
    short   sFilHdl; 
    int     iErrCod;                    /* File operation error code    */
    WORD    usBytCnt;                   /* Byte read count              */
    WORD    usSrcIdx = 0;
    LPSTR   pcSrcPtr;
    LPSTR   pcDstPtr = pcWrkBuf;                   

    /********************************************************************/
    /* Read file, allow space for final terminators                     */
    /********************************************************************/
    if (-1 == (sFilHdl = FilHdlOpn (szFilNam, O_BINARY | O_RDONLY))) return ((WORD) -1);
    usBytCnt = Rd_FilFwd (sFilHdl, pcWrkBuf, usBufSiz-2, FIOENCNON, &iErrCod);
    FilHdlCls (sFilHdl);
    if (0 == usBytCnt) return ((WORD) -1);

    /********************************************************************/
    /* Find first printable, non-white space char.                      */
    /********************************************************************/
    while ((usSrcIdx < usBytCnt) && !isgraph (pcWrkBuf[usSrcIdx])) usSrcIdx++;
    if (usSrcIdx == usBytCnt) return ((WORD) -1);

    /********************************************************************/
    /* Compress printables between CR/LF's into LF term recs            */
    /********************************************************************/
    while (usSrcIdx < usBytCnt) {
        while ((usSrcIdx < usBytCnt) && (CR_CHR != pcWrkBuf[usSrcIdx]) && (LF_CHR != pcWrkBuf[usSrcIdx]))
            if (isprint (*pcDstPtr = pcWrkBuf[usSrcIdx++])) pcDstPtr++;
        *pcDstPtr++ = LF_CHR;
        while ((usSrcIdx < usBytCnt) && !isgraph (pcWrkBuf[usSrcIdx])) usSrcIdx++;
    }
    *pcDstPtr++ = '\0';

    /********************************************************************/
    /* Remove all non-white space chars from record beginning to "=".   */
    /* Copy "=" sign and remove non-white space trailing "=".           */
    /********************************************************************/
    pcSrcPtr = pcDstPtr = pcWrkBuf;                   
    while (*pcSrcPtr) {
        while (*pcSrcPtr && (LF_CHR != *pcSrcPtr) && (EQ_CHR != *pcSrcPtr))              
            if (isgraph (*pcDstPtr = *pcSrcPtr++)) pcDstPtr++;
        if (EQ_CHR == *pcSrcPtr) *pcDstPtr++ = *pcSrcPtr++;
        while (*pcSrcPtr && (LF_CHR != *pcSrcPtr) && !isgraph (*pcSrcPtr)) pcSrcPtr++;
        while (*pcSrcPtr && (LF_CHR != (*pcDstPtr++ = *pcSrcPtr++)));
    }    
    *pcDstPtr++ = '\0';

    /********************************************************************/
    /* Remove all comments from ";" to LF                               */
    /********************************************************************/
    pcSrcPtr = pcDstPtr = pcWrkBuf;                   
    while (*pcSrcPtr) {
        while (*pcSrcPtr && (';' != *pcSrcPtr)) *pcDstPtr++ = *pcSrcPtr++;
        while (*pcSrcPtr && (LF_CHR != *pcSrcPtr)) *pcSrcPtr++;
    }    
    *pcDstPtr++ = '\0';

    /********************************************************************/
    /* Remove all trailing blanks from LF to first printable            */
    /********************************************************************/
    _fstrrev (pcSrcPtr = pcDstPtr = pcWrkBuf);                   
    while (*pcSrcPtr) {
        while (*pcSrcPtr && (LF_CHR != (*pcDstPtr++ = *pcSrcPtr++)));
        while (*pcSrcPtr && (LF_CHR != *pcSrcPtr) && !isgraph (*pcSrcPtr)) pcSrcPtr++;
    }    
    *pcDstPtr++ = '\0';
    _fstrrev (pcWrkBuf);                   
    
    /********************************************************************/
    /* Remove all empty records                                         */
    /********************************************************************/
    pcSrcPtr = pcDstPtr = pcWrkBuf;                   
    while (*pcSrcPtr) {
        while (*pcSrcPtr && (LF_CHR != (*pcDstPtr++ = *pcSrcPtr++)));
        while (*pcSrcPtr && (LF_CHR == *pcSrcPtr)) *pcSrcPtr++;
    }    
    *pcDstPtr++ = '\0';
    
    /********************************************************************/
    /********************************************************************/
    return (0);

}

long FAR PASCAL CfgLngGet (VISMEMHDL mhCfgMem, LPSTR szSecNam, LPSTR szPrmNam, 
                long lDefVal)
{
    LPSTR   pcCfgBuf = GloMemLck (mhCfgMem);
    char    szWrkStr[CFGMAXSTR];

    /********************************************************************/
    /********************************************************************/
    if (!pcCfgBuf || !(pcCfgBuf = GetCfgSec (pcCfgBuf, szSecNam))) {
        GloMemUnL (mhCfgMem);
        return (lDefVal);
    }
    if (!CopCfgPrm (pcCfgBuf, szPrmNam, szWrkStr, CFGMAXSTR)) {
        GloMemUnL (mhCfgMem);
        return (lDefVal);
    }
    GloMemUnL (mhCfgMem);

    /********************************************************************/
    /********************************************************************/
    return (_fstrtol (szWrkStr, NULL, 10));

}

float FAR PASCAL CfgFltGet (VISMEMHDL mhCfgMem, LPSTR szSecNam, LPSTR szPrmNam, 
                 float flDefVal)
{
    LPSTR   pcCfgBuf = GloMemLck (mhCfgMem);
    static  char    szWrkStr[CFGMAXSTR];

    /********************************************************************/
    /********************************************************************/
    if (!pcCfgBuf || !(pcCfgBuf = GetCfgSec (pcCfgBuf, szSecNam))) {
        GloMemUnL (mhCfgMem);
        return (flDefVal);
    }
    if (!CopCfgPrm (pcCfgBuf, szPrmNam, szWrkStr, CFGMAXSTR)) {
        GloMemUnL (mhCfgMem);
        return (flDefVal);
    }
    GloMemUnL (mhCfgMem);

    /********************************************************************/
    /********************************************************************/
    return ((float) atof (szWrkStr));

}

LPSTR FAR PASCAL CfgStrGet (VISMEMHDL mhCfgMem, LPSTR szSecNam, LPSTR szPrmNam, 
                 LPSTR lpDefVal, LPSTR lpDstStr, WORD usDstLen)
{
    LPSTR   pcCfgBuf = GloMemLck (mhCfgMem);

    /********************************************************************/
    /********************************************************************/
    if (!lpDstStr || !usDstLen) return (lpDefVal);

    /********************************************************************/
    /********************************************************************/
    if (!pcCfgBuf || !(pcCfgBuf = GetCfgSec (pcCfgBuf, szSecNam))) {
        GloMemUnL (mhCfgMem);
        return (lpDefVal ? _fstrncpy (lpDstStr, lpDefVal, usDstLen) : lpDefVal);
    }
    if (!CopCfgPrm (pcCfgBuf, szPrmNam, lpDstStr, usDstLen)) {
        GloMemUnL (mhCfgMem);
        return (lpDefVal ? _fstrncpy (lpDstStr, lpDefVal, usDstLen) : lpDefVal);
    }
    GloMemUnL (mhCfgMem);

    /********************************************************************/
    /********************************************************************/
    return (lpDstStr);

}

/************************************************************************/
/************************************************************************/
LPSTR   GetCfgSec (LPSTR pcWrkBuf, LPSTR szSecNam)
{
    char    ucWrkStr[CFGMAXSTR];
    char    ucSecStr[CFGMAXSTR];            /* Name & section marker    */ 
    WORD    usSecLen;                   
    
    /********************************************************************/
    /* Room for string with markers and null terminator?                */
    /* Yes - Then append section name separators.                       */
    /********************************************************************/
    if ((usSecLen = _fstrlen (szSecNam)) > CFGMAXSTR - 3) return (NULL);
    ucSecStr[0] = SB_CHR;
    _fstrcpy (&ucSecStr[1], szSecNam);
    ucSecStr[1 + usSecLen] = SE_CHR;
    ucSecStr[2 + usSecLen] = '\0';

    /********************************************************************/
    /*                      Find szSecNam Section                       */
    /********************************************************************/
    while (_fstrlen (strncpy_c (ucWrkStr, pcWrkBuf, CFGMAXSTR, LF_CHR))) {
        if (!_fstricmp (ucWrkStr, ucSecStr)) {
            pcWrkBuf += _fstrlen (ucWrkStr) + 1;
            return (pcWrkBuf);
        }
        pcWrkBuf += _fstrlen (ucWrkStr) + 1;
    }
    return (NULL);

}

LPSTR   CopCfgPrm (LPSTR pcWrkBuf, LPSTR szPrmNam, LPSTR pcPrmBuf, WORD usBufSiz)
{
    char    ucWrkStr[CFGMAXSTR];
    char    ucPrmStr[CFGMAXSTR];            /* Name & equals sign       */ 
    WORD    usPrmLen;                   

//    DWORD   uli;

//    /********************************************************************/
//    /* Is this a named or indexed parameter?                            */
//    /********************************************************************/
//    for (uli=0; uli < _fstrlen (szPrmNam); uli++) if (!isdigit(szPrmNam[uli])) break;
//
//    /********************************************************************/
//    /* Indexed: skip to indicated record, fail at next section          */
//    /********************************************************************/
//    if ('\0' == szPrmNam[uli]) {
//        uli =  _fstrtol (szPrmNam, NULL, 10);
//        while (_fstrlen (strncpy_c (ucWrkStr, pcWrkBuf, CFGMAXSTR, LF_CHR))) {
//            if (SB_CHR == *ucWrkStr) break; 
//            if (!uli--) return (_fstrncpy (pcPrmBuf, ucWrkStr, usBufSiz));
//            pcWrkBuf += _fstrlen (ucWrkStr) + 1;
//        }
//        return (NULL);
//    }

    /********************************************************************/
    /* Room for string with "=" and null terminator?                    */
    /* Yes - Then append section name separators.                       */
    /********************************************************************/
    if ((usPrmLen = _fstrlen (szPrmNam)) > CFGMAXSTR - 2) return (NULL);
    _fstrcpy (ucPrmStr, szPrmNam);
    ucPrmStr[usPrmLen++] = EQ_CHR;
    ucPrmStr[usPrmLen] = '\0';

    /********************************************************************/
    /* Named: Check each LF term record for name, fail at next section  */
    /********************************************************************/
    while (_fstrlen (strncpy_c (ucWrkStr, pcWrkBuf, CFGMAXSTR, LF_CHR))) {
        if (SB_CHR == *ucWrkStr) break; 
        if (!_fstrnicmp (ucWrkStr, ucPrmStr, usPrmLen)) 
            return (_fstrncpy (pcPrmBuf, ucWrkStr + usPrmLen, usBufSiz));
        pcWrkBuf += _fstrlen (ucWrkStr) + 1;
    }
    return (NULL);

}

/************************************************************************/
/************************************************************************/
LPSTR   strncpy_c (LPSTR szDstStr, LPSTR szSrcStr, WORD usMaxLen, char cTrmChr)
{

    WORD    usi = 0;

    /********************************************************************/
    /* Copy string up to NULL or term character; force NULL terminator  */
    /********************************************************************/
    while ((*szSrcStr != '\0') && (*szSrcStr != cTrmChr) 
        && (usi++ < usMaxLen-1)) *szDstStr++ = *szSrcStr++;
    *szDstStr = '\0';

    /********************************************************************/
    /********************************************************************/
    return (szDstStr-usi);

}

WORD strnidx (LPCSTR szDomain, LPCSTR szObject, WORD usObjLen)
{
    LPCSTR  pFirst;
    WORD    usDomLen = _fstrlen ((LPSTR) szDomain);

    /********************************************************************/
    /* Find the first offset of szObject within szDomain                */
    /********************************************************************/
    pFirst = szDomain;
    while ((usObjLen + (WORD) (pFirst - szDomain)) <= usDomLen) {
        pFirst = _fmemchr (pFirst, (int) *szObject, usDomLen - (pFirst - szDomain));
        if (pFirst == NULL) return (usDomLen);
        if (0 == _fmemcmp (pFirst, szObject, usObjLen)) return (pFirst - szDomain);
        pFirst++;
    }
    return (usDomLen);
}
long    _fstrtol (LPSTR lpString, LPSTR FAR *ppStrEnd, int iRadix)
{
    static char szWrkBuf[CFGMAXSTR];
    static char *pcTmpPtr;
    long        lRetVal;

    /********************************************************************/
    /********************************************************************/
    _fstrncpy (szWrkBuf, lpString, CFGMAXSTR);
    lRetVal = strtol (szWrkBuf, &pcTmpPtr, iRadix);
    if (ppStrEnd) *ppStrEnd = pcTmpPtr;
    return (lRetVal);
}

