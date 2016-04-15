/************************************************************************/
/* Windows DLL Support Functions: DLLSup.c              V2.00  05/20/93 */
/* Copyright (c) 1987-1993 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#include "winmem.h"                     /* Generic memory supp defs     */
#include "dllsup.h"                     /* Windows DLL support defs     */

#include <string.h>                     /* String manipulation funcs    */
#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <dos.h>                        /* Low level dos calls          */

/************************************************************************/
/************************************************************************/
#define DLLMAXSTR      40               /* Gen purpose string length    */

/************************************************************************/
/************************************************************************/
char FAR *  _fitoa (int iValue, char FAR * lpString, int iRadix)
{
    static char szWrkBuf[DLLMAXSTR];

    /********************************************************************/
    /********************************************************************/
    _itoa (iValue, szWrkBuf, iRadix);
    _fstrncpy (lpString, szWrkBuf, min (DLLMAXSTR, strlen (szWrkBuf) + 1));

    /********************************************************************/
    /********************************************************************/
    return (lpString);
}
char FAR *  _fultoa  (unsigned long ulValue, char FAR * lpString, int iRadix)
{
    static char szWrkBuf[DLLMAXSTR];

    /********************************************************************/
    /********************************************************************/
    _ultoa (ulValue, szWrkBuf, iRadix);
    _fstrncpy (lpString, szWrkBuf, min (DLLMAXSTR, strlen (szWrkBuf) + 1));

    /********************************************************************/
    /********************************************************************/
    return (lpString);
}
char FAR *  _ffcvt (double dbValue, int iCount, int FAR * lpiDecimal, 
            int FAR * lpiSign)
{
    static int iDecimal;
    static int iSign;
    static char szWrkBuf[DLLMAXSTR];

    /********************************************************************/
    /********************************************************************/
    _fstrcpy (szWrkBuf, _fcvt (dbValue, iCount, &iDecimal, &iSign));
    *lpiDecimal = iDecimal;
    *lpiSign = iSign;

    /********************************************************************/
    /********************************************************************/
    return ((char far *) szWrkBuf);
}
int _fatoi  (char FAR * lpString)
{
    static char szWrkBuf[DLLMAXSTR];

    /********************************************************************/
    /********************************************************************/
    _fstrncpy (szWrkBuf, lpString, min (DLLMAXSTR, _fstrlen (lpString) + 1));

    /********************************************************************/
    /********************************************************************/
    return (atoi (szWrkBuf));
}
float _fatof (char FAR * lpString)
{
    static char szWrkBuf[DLLMAXSTR];

    /********************************************************************/
    /********************************************************************/
    _fstrncpy (szWrkBuf, lpString, min (DLLMAXSTR, _fstrlen (lpString) + 1));

    /********************************************************************/
    /********************************************************************/
    return ((float) atof (szWrkBuf));
}
long _fatol  (char FAR * lpString)
{
    static char szWrkBuf[DLLMAXSTR];

    /********************************************************************/
    /********************************************************************/
    _fstrncpy (szWrkBuf, lpString, min (DLLMAXSTR, _fstrlen (lpString) + 1));

    /********************************************************************/
    /********************************************************************/
    return (atol (szWrkBuf));
}

