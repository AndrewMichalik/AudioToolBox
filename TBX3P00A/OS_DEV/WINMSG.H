/************************************************************************/
/* Windows User Message Support Defs: WinMsg.c          V2.00  09/20/93 */
/* Copyright (c) 1987-1993 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define     WINMAXMSG   256             /* Maximum user message length  */

/************************************************************************/
/*                  Dialog Box Positioning Definitions                  */
/************************************************************************/
typedef enum {
    CTR_CTR,                            /* center (x), Center (y)       */
    CTR_UPR                             /* center (x), Upper  (y)       */
} DLGPOS;
typedef int (WINAPI *MSGDSPPRC) (HWND, LPCSTR, LPCSTR, UINT);
extern  MSGDSPPRC   MsgDspPrc;

typedef struct _MSGASKBLK {             /* MsgAskRes data block         */
    char    szMsgTxt [WINMAXMSG];       /* Prompt message text          */
    char    szRspTxt [WINMAXMSG];       /* User response text           */
} MSGASKBLK;

/************************************************************************/
/************************************************************************/
WORD CDECL  MsgDspUsr (LPCSTR, ...);
WORD CDECL  MsgDspRes (HANDLE, WORD, WORD, ...);
WORD CDECL  MsgLodStr (HANDLE, WORD, LPSTR, WORD);
WORD CDECL  MsgAskUsr (LPCSTR, ...);
WORD CDECL  MsgAskRes (HANDLE, LPCSTR, DLGPROC, WORD, LPCSTR, ...);

WORD WINAPI MsgBegPol (LPVOID);
WORD WINAPI MsgDspPol (LPVOID, unsigned long, unsigned long);
WORD WINAPI MsgEndPol (LPVOID);
WORD WINAPI CtrDlgBox (HWND, HWND, HWND, DLGPOS);
HWND WINAPI GetParWnd (HWND);
