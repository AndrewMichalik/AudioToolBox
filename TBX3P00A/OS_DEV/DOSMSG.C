/************************************************************************/
/* Generic DOS User Message Support: DOSMsg.c           V2.00  08/15/93 */
/* Copyright (c) 1991-1992 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "dosmsg.h"                     /* User message support defs    */
#include "..\usrdev\usrstb.h"           /* User supplied I/O stubs      */

#include <string.h>                     /* String manipulation funcs    */
#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <stdarg.h>                     /* ANSI Std var length args     */

/************************************************************************/
/************************************************************************/
MSGDSPPRC   MsgDspPrc = VISPrtFmt;      /* Re-assignable msg output     */
#define     TRUE    1

/************************************************************************/
/************************************************************************/
char *  fdtorna (double, short, char *);

/************************************************************************/
/************************************************************************/
unsigned short cdecl MsgDspUsr (const char *pFmtLst, ...)
{

    /********************************************************************/
    /********************************************************************/
    va_list vaArgMrk;
    va_start (vaArgMrk, pFmtLst);
  
    /********************************************************************/
    /********************************************************************/
    MsgDspPrc (pFmtLst, vaArgMrk);

    /********************************************************************/
    /********************************************************************/
    va_end (vaArgMrk);                  /* Reset variable arguments     */

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);

}

unsigned short cdecl MsgAskUsr (const char *pFmtLst, ...)
{
    /********************************************************************/
    /********************************************************************/
    va_list vaArgMrk;
    va_start (vaArgMrk, pFmtLst);
  
    /********************************************************************/
    /********************************************************************/
    va_end (vaArgMrk);                  /* Reset variable arguments     */
    return (VISScnFmt (pFmtLst, vaArgMrk));

}

/************************************************************************/
/************************************************************************/
unsigned short far pascal   MsgBegPol (void far *lpPolDat)
{
    return (TRUE);
}
unsigned short far pascal   MsgDspPol (const char far *lpMsgTxt, unsigned long ulCurCnt, unsigned long ulTotCnt)
{
    #define         MAXSTRLEN   40
    #define         MAXPCTLEN    5
    char            szTxtBuf[MAXSTRLEN+1] = {0};    
    char            szDspBuf[MAXSTRLEN+1] = {0};    
    unsigned short  usDspLen;
    unsigned short  usCopLen;
    unsigned short  usi;

    /********************************************************************/
    /********************************************************************/
    if (ulTotCnt) {
        memset (szDspBuf, ' ', MAXSTRLEN);
        fdtorna (100. * ulCurCnt / ulTotCnt, 1, &szTxtBuf[strlen (szTxtBuf)]);
        usDspLen = MAXPCTLEN - min (MAXPCTLEN, strlen (szTxtBuf));
        memcpy (&szDspBuf[usDspLen], szTxtBuf, usCopLen = min (strlen (szTxtBuf), MAXSTRLEN - usDspLen));
        usDspLen += usCopLen;
        memcpy (&szDspBuf[usDspLen], "%%", usCopLen = min (2, MAXSTRLEN - usDspLen));
        usDspLen += usCopLen;
        if (lpMsgTxt) _fmemcpy (&szDspBuf[usDspLen], lpMsgTxt, usCopLen = min (_fstrlen (lpMsgTxt), MAXSTRLEN - usDspLen));
        szDspBuf[MAXSTRLEN] = '\0';
        usDspLen = MAXSTRLEN - 1L;              /* Compensate for %%    */
    }
    else {
        _ultoa (ulCurCnt, szDspBuf, 10);
        usDspLen = strlen (szDspBuf);
    }

    MsgDspUsr (szDspBuf);
    for (usi=0; usi < usDspLen; usi++) MsgDspUsr ("\b"); 

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);

}
unsigned short far pascal   MsgEndPol (void far *lpPolDat)
{
    return (TRUE);
}

/************************************************************************/
/*                  Double to rounded ASCII converter                   */
/************************************************************************/
char *  fdtorna (double dbSrcVal, short sDecCnt, char *szTxtBuf)
{
    char *  pcCvtTxt;
    char *  pcWrkBuf = szTxtBuf;
    int     iDecPos, iSgnFlg;

    /********************************************************************/
    /********************************************************************/
    if (0.0 == dbSrcVal) return (strcpy (szTxtBuf, "0"));

    /********************************************************************/
    /* Convert dbSrcVal to string, sDecCnt digits after decimal pt       */
    /********************************************************************/
    pcCvtTxt = _fcvt (dbSrcVal, sDecCnt, &iDecPos, &iSgnFlg);
    if (iSgnFlg) *pcWrkBuf++ = '-';

    /********************************************************************/
    /* Adjust iDecPos since fcvt returns < -sDecCnt for small values    */
    /********************************************************************/
    if (iDecPos < -sDecCnt) iDecPos = -sDecCnt;

    /********************************************************************/
    /********************************************************************/
    if (iDecPos <= 0) {
        *pcWrkBuf++ = '.';
        while (iDecPos++ < 0) *pcWrkBuf++ = '0';
    } else {
        while (iDecPos-- > 0) *pcWrkBuf++ = *pcCvtTxt++;
        *pcWrkBuf++ = '.';
    }
    strcpy (pcWrkBuf, pcCvtTxt);

    /********************************************************************/
    /********************************************************************/
    return (szTxtBuf);
  
}
