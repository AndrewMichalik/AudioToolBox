/************************************************************************/
/* Windows User Message Support: WinMsg.c               V2.00  09/20/93 */
/* Copyright (c) 1987-1992 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#include "winmsg.h"                     /* User message support         */

#include <string.h>                     /* String manipulation funcs    */
#include <stdlib.h>                     /* Misc string/math/error funcs */

#if !(defined (_WINDLL))
    #define  _WINDLL                    /* Force DLL def for stdarg.h   */  
#endif
#include <stdarg.h>                     /* ANSI Std var length args     */
#include <ctype.h>                      /* Char classification funcs    */

/************************************************************************/
/************************************************************************/
MSGDSPPRC   MsgDspPrc = MessageBox;     /* Re-assignable msg output     */

/************************************************************************/
/************************************************************************/
int xwvsprintf (LPSTR szMsgTxt, LPCSTR lpFmtReq, va_list vaArgMrk)
{
    char        szFmtBuf [WINMAXMSG];
    WORD        usArgLst [WINMAXMSG / 2];
    char FAR  * lpFmtBuf = szFmtBuf;
    WORD FAR  * lpArgLst = usArgLst;
    
    /********************************************************************/
    /********************************************************************/
    while (*lpFmtReq && lpFmtBuf < &szFmtBuf[WINMAXMSG-1]) {
        if ('%' == (*lpFmtBuf++ = *lpFmtReq++)) {
            while (isdigit (*lpFmtReq) || ('.' == *lpFmtReq)) *lpFmtBuf++ = *lpFmtReq++;
            switch (*lpFmtReq) {
                case 'c':            
                case 'd':            
                case 'x':            
                case 'i': 
                    *lpFmtBuf++ = *lpFmtReq++;
                    *lpArgLst++ = va_arg (vaArgMrk, WORD);
                    break;
                case 'f':                       // Convert floats to hex value
                    lpFmtReq++;
                    *lpFmtBuf++ = 'l';
                    *lpFmtBuf++ = 'x';
                    *lpArgLst++ = va_arg (vaArgMrk, WORD);
                    *lpArgLst++ = va_arg (vaArgMrk, WORD);
                    break;
                case 'l': 
                    *lpFmtBuf++ = *lpFmtReq++;
                    *lpFmtBuf++ = *lpFmtReq++;
                    *lpArgLst++ = va_arg (vaArgMrk, WORD);
                    *lpArgLst++ = va_arg (vaArgMrk, WORD);
                    break;
                case 'F':                       // MSC DOS lib %Fs compatability
                    lpFmtReq++;
                case 's': 
                    *lpFmtBuf++ = *lpFmtReq++;
                    *lpArgLst++ = va_arg (vaArgMrk, WORD);
                    *lpArgLst++ = va_arg (vaArgMrk, WORD);
                    break;
                case '%':
                    *lpFmtBuf++ = *lpFmtReq++;
                    break;
                default:
                    *lpFmtBuf++ = '%';          // Error: show % as literal
            }
        }                
    }                
    *lpFmtBuf = '\0';
    
    /********************************************************************/
    /********************************************************************/
    return (wvsprintf (szMsgTxt, szFmtBuf, usArgLst));

}

int wvsscanf (LPSTR szRspTxt, LPCSTR lpFmtReq, va_list vaArgMrk)
{
    static char szWrkBuf[WINMAXMSG];
    short       sArgCnt = 0;
    
    /********************************************************************/
    /********************************************************************/
    while (*lpFmtReq) {
        if ('%' == *lpFmtReq++) {
            switch (*lpFmtReq) {
                case 'i':
                    if (*szRspTxt) {
                        _fstrncpy (szWrkBuf, szRspTxt, WINMAXMSG);
                        *((int FAR *) va_arg (vaArgMrk, LPVOID)) = atoi (szWrkBuf);
                        sArgCnt++;
                    }
                    else sArgCnt = -1;
                    break;
            }
        }
    }

    /********************************************************************/
    /********************************************************************/
    return (sArgCnt);

}

/************************************************************************/
/************************************************************************/
WORD CDECL  MsgAskUsr (const char far *pFmtReq, ...)
{
    /********************************************************************/
    /********************************************************************/
    return (0);                                             /* Abort?   */
}

WORD CDECL  MsgAskRes (HANDLE hInsHdl, LPCSTR lpDlgNam, DLGPROC fpMsgAskFmt, 
            WORD usMsgSID, LPCSTR lpFmtReq, ...)
{
    MSGASKBLK   mbMsgRsp;
    FARPROC     lpPrcAdr;
    short       sArgCnt = 0;

    /********************************************************************/
    /* Load user prompt text                                            */
    /********************************************************************/
    LoadString (hInsHdl, usMsgSID, mbMsgRsp.szMsgTxt, WINMAXMSG);

    /********************************************************************/
    /* Print default values using format string                         */
    /* Note: wsprintf has too few levels of indirection                 */
    /********************************************************************/
    {
    va_list vaArgMrk;
    va_start (vaArgMrk, lpFmtReq);
    wsprintf (mbMsgRsp.szRspTxt, lpFmtReq, *va_arg (vaArgMrk, WORD FAR *));
    va_end (vaArgMrk);                          /* Reset variable args  */
    }

    /********************************************************************/
    /********************************************************************/
    lpPrcAdr = MakeProcInstance ((FARPROC) fpMsgAskFmt, hInsHdl);
    while (!sArgCnt) {
        if (-1 == DialogBoxParam (hInsHdl, lpDlgNam, GetParWnd (NULL), 
          (DLGPROC) lpPrcAdr, (DWORD) (LPVOID) &mbMsgRsp)) {
            FreeProcInstance (lpPrcAdr);
            return (0);
        }
        sArgCnt = wvsscanf (mbMsgRsp.szRspTxt, lpFmtReq, (va_list) &lpFmtReq + _INTSIZEOF(lpFmtReq));
        if (-1 == sArgCnt) {
            MessageBeep (0);
            sArgCnt = 0;
        }
    }
    FreeProcInstance(lpPrcAdr);

    /********************************************************************/
    /********************************************************************/
    return (sArgCnt);

}

/************************************************************************/
/************************************************************************/
WORD CDECL  MsgDspUsr (LPCSTR lpFmtReq, ...) 
{
    char        szMsgTxt [WINMAXMSG];

    /********************************************************************/
    /********************************************************************/
    xwvsprintf (szMsgTxt, lpFmtReq, (va_list) &lpFmtReq + _INTSIZEOF(lpFmtReq));
	MsgDspPrc (GetParWnd (NULL), szMsgTxt, NULL, MB_OK | MB_ICONEXCLAMATION);

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);
}

WORD CDECL  MsgDspRes (HANDLE hResHdl, WORD usTypFlg, WORD usMsgSID, ...)
{
    char    szMsgTxt [WINMAXMSG];
    char    szDspTxt [WINMAXMSG];

    /********************************************************************/
    /********************************************************************/
    if (!usTypFlg) usTypFlg = MB_OK | MB_ICONEXCLAMATION;
    MsgLodStr (hResHdl, usMsgSID, szMsgTxt, WINMAXMSG);
    xwvsprintf (szDspTxt, szMsgTxt, (va_list) &usMsgSID + _INTSIZEOF(usMsgSID));
    MsgDspPrc (GetParWnd(NULL), szDspTxt, NULL, usTypFlg);

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);
}

/************************************************************************/
/************************************************************************/
WORD CDECL  MsgLodStr (HANDLE hResHdl, WORD usMsgSID, LPSTR szMsgTxt, WORD usMaxLen)
{
    static  char szErrTxt [WINMAXMSG];

    /********************************************************************/
    /********************************************************************/
    if (!usMaxLen) return (0);

    /********************************************************************/
    /********************************************************************/
    if (0 == usMsgSID) {
        szMsgTxt[0] = '\0';
        return (TRUE);
    }
    /********************************************************************/
    /********************************************************************/
    if (!LoadString (hResHdl, usMsgSID, szMsgTxt, usMaxLen)) {
        strncpy (szErrTxt, "Unknown message: ", WINMAXMSG);
        _itoa (usMsgSID, &szErrTxt[strlen (szErrTxt)], 10);
		_fstrncpy (szMsgTxt, szErrTxt, usMaxLen);
        return (FALSE);
    }
    /********************************************************************/
    /********************************************************************/
    return (TRUE);

}

/************************************************************************/
/************************************************************************/
WORD WINAPI MsgBegPol (LPVOID lpPolDat)
{
    return (TRUE);
}

WORD WINAPI MsgDspPol (LPVOID lpMsgTxt, unsigned long ulCurCnt, unsigned long ulTotCnt)
{
    MSG msg;

    /********************************************************************/
    /********************************************************************/
    LockData(0);
    while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage (&msg);
    }
    UnlockData(0);

    /********************************************************************/
    /* Return TRUE to continue processing                               */
    /********************************************************************/
    return (TRUE);
}

WORD WINAPI MsgEndPol (LPVOID lpPolDat)
{
    return (TRUE);
}

/************************************************************************/
/************************************************************************/
WORD WINAPI CtrDlgBox (HWND hActWnd, HWND hDBxWnd, HWND hDefWnd, DLGPOS fPosFlg)
{

    short   sScrWid, sScrHgt;
    short   sDBxWid, sDBxHgt;
    short   sActWid, sActHgt;
    RECT    rActRec;
    RECT    rDlgRec;
    RECT    rCtlRec;
    POINT   NewPos;

    /********************************************************************/
    /* If NULL active window, try to get parent                         */
    /********************************************************************/
    if (NULL == hActWnd) hActWnd = GetParWnd (hDBxWnd);

    /********************************************************************/
    /* Query width and depth of screen device                           */
    /********************************************************************/
    sScrWid = GetSystemMetrics (SM_CXSCREEN);
    sScrHgt = GetSystemMetrics (SM_CYSCREEN);

    /********************************************************************/
    /* Query width and depth of active window                           */
    /********************************************************************/
    if (NULL != hActWnd) {
        GetWindowRect (hActWnd, (LPRECT) &rActRec);
        sActWid = rActRec.right  - rActRec.left;
        sActHgt = rActRec.bottom - rActRec.top;
    } else {
        rActRec.right  = sActWid = sScrWid;
        rActRec.bottom = sActHgt = sScrHgt;
        rActRec.left   = 0;
        rActRec.top    = 0;
    }

    /********************************************************************/
    /* Query width and depth of dialog box                              */
    /********************************************************************/
    GetWindowRect (hDBxWnd, (LPRECT) &rDlgRec);
    sDBxWid = rDlgRec.right  - rDlgRec.left;
    sDBxHgt = rDlgRec.bottom - rDlgRec.top;
    
    /********************************************************************/
    /********************************************************************/
    NewPos.x = rActRec.left + (sActWid - sDBxWid)/2;
    NewPos.x = max (NewPos.x, 0);
    NewPos.x = min (NewPos.x, sScrWid - sDBxWid);

    if (fPosFlg == CTR_CTR) {
        NewPos.y = rActRec.top + (sActHgt - sDBxHgt)/2;
    }
    else if (fPosFlg == CTR_UPR) {
        NewPos.y = rActRec.bottom;
        GetClientRect (hActWnd, (LPRECT) &rActRec);
        NewPos.y -= rActRec.bottom;
    }            
    NewPos.y = max (NewPos.y, 0);
    NewPos.y = min (NewPos.y, sScrHgt - sDBxHgt);

    /********************************************************************/
    /* Center dialog box within the screen                              */
    /********************************************************************/
    SetWindowPos (hDBxWnd, HWND_TOPMOST, NewPos.x, NewPos.y, 0, 0, SWP_NOSIZE);

    /********************************************************************/
    /* Center Mouse on selected control                                 */
    /********************************************************************/
    if ((NULL != hDefWnd) && GetSystemMetrics (SM_MOUSEPRESENT)) {
        SetCapture (hDBxWnd);
        GetWindowRect (hDefWnd, (LPRECT) &rCtlRec);
        SetCursorPos (rCtlRec.left + (rCtlRec.right  - rCtlRec.left) / 4,
            rCtlRec.top + (rCtlRec.bottom - rCtlRec.top)  / 2);
        ReleaseCapture ();
    }

    return (0);
}

HWND WINAPI GetParWnd (HWND hActWnd)
{
    HWND    hParWnd;

    /********************************************************************/
    /********************************************************************/
    if (NULL == hActWnd) hActWnd = GetActiveWindow ();
    while (NULL != (hParWnd = GetParent (hActWnd))) hActWnd = hParWnd;

    /********************************************************************/
    /********************************************************************/
    return (hActWnd);
}
