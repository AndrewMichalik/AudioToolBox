/************************************************************************/
/* License Dialog: LicDlg.c                             V2.00  04/15/96 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define ADVPOLPER   3000                    /* Advertise poll per (ms)  */
#define TI_ADVTMR      2                    /* Advertise timer number   */

/************************************************************************/
/*                      Forward References                              */
/************************************************************************/
LONG FAR PASCAL LicDlgPrc (HWND, UINT, UINT, LONG);

/************************************************************************/
/************************************************************************/
#pragma pack(1)
struct DialogBoxHeader {
    DWORD   lStyle;
    BYTE    bNumberOfItems;
    WORD    x;
    WORD    y;
    WORD    cx;
    WORD    cy;
    char    szVarDat[];
};
#define PDLGTEMPLATE struct DialogBoxHeader NEAR *
struct ControlData {
    WORD    x;
    WORD    y;
    WORD    cx;
    WORD    cy;
    WORD    wID;
    DWORD   lStyle;
    union
    {
        BYTE    class;      /* if (class & 0x80)    */
        char    szClass;    /* otherwise            */
    } ClassID;
    char    szText[];
};
#define PCONTROLDATA struct ControlData NEAR *
#define CONTROLDATAPAD  1
#pragma pack()

/************************************************************************/
/************************************************************************/
int LicDlgDsp (LPSTR szRegNam, LPSTR szRegCmp, LPSTR szRegSer, LICDLGBLK FAR *lpDlgBlk)
{
    PDLGTEMPLATE    pDlgTmp;
    PCONTROLDATA    pCtlDat;
    VISMEMHDL       hLocMem;
    #define         LOCMEMSIZ   1024  

    /********************************************************************/
    /* Allocate some local memory.                                      */
    /********************************************************************/
    pDlgTmp = (PDLGTEMPLATE) LocAloLck (&hLocMem, LOCMEMSIZ);
    memset (pDlgTmp, 0, LOCMEMSIZ);

    /********************************************************************/
    /* Fill the DLGTEMPLATE information structure.                      */
    /********************************************************************/
    pDlgTmp->lStyle = DS_MODALFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU;
    pDlgTmp->bNumberOfItems = 4;
    pDlgTmp->x  = 0; 
    pDlgTmp->y  = 0; 
    pDlgTmp->cx = 188;
    pDlgTmp->cy =  96;
    pDlgTmp->szVarDat[0] = 0;
    pDlgTmp->szVarDat[1] = 0;
    _fstrcpy (&(pDlgTmp->szVarDat[2]), lpDlgBlk->szTtlTxt);
    /* add in the wPointSize and szFontName if the DS_SETFONT bit on    */
    pCtlDat = (PCONTROLDATA) (((PSTR) &(pDlgTmp->szVarDat[2])) 
            + (_fstrlen (&(pDlgTmp->szVarDat[2])) + 1));

    /********************************************************************/
    /* Future 32 bit: make sure the first item starts on DWORD boundary */
    /*                p = lpwAlign (p);                                 */
    /********************************************************************/
    /********************************************************************/
    /* This copy is licensed to:                                        */
    /********************************************************************/
    pCtlDat->x      =   1; 
    pCtlDat->y      =   4; 
    pCtlDat->cx     = 186; 
    pCtlDat->cy     =  14; 
    pCtlDat->wID    =   0;
    pCtlDat->lStyle = SS_CENTER | WS_VISIBLE | WS_CHILD;
    pCtlDat->ClassID.class  = 0x82;
    _fstrcpy (pCtlDat->szText, lpDlgBlk->szLicTxt);
    pCtlDat = (PCONTROLDATA) (((PSTR) (pCtlDat->szText)) 
            + (_fstrlen (pCtlDat->szText) + 1) + CONTROLDATAPAD);

    /********************************************************************/
    /* Registered user and serial number static control                 */
    /********************************************************************/
    pCtlDat->x      =   1; 
    pCtlDat->y      =  18; 
    pCtlDat->cx     = 186; 
    pCtlDat->cy     =  34; 
    pCtlDat->wID    =   0;
    pCtlDat->lStyle = SS_CENTER | WS_VISIBLE | WS_CHILD;
    pCtlDat->ClassID.class  = 0x82;
    _fstrcpy (pCtlDat->szText, szRegNam);
    _fstrcat (pCtlDat->szText, "\n");
    _fstrcat (pCtlDat->szText, szRegCmp);
    _fstrcat (pCtlDat->szText, "\n");
    _fstrcat (pCtlDat->szText, szRegSer);
    pCtlDat = (PCONTROLDATA) (((PSTR) (pCtlDat->szText)) 
            + (_fstrlen (pCtlDat->szText) + 1) + CONTROLDATAPAD);

    /********************************************************************/
    /* "OK" Button                                                      */
    /********************************************************************/
    pCtlDat->x      =  74; 
    pCtlDat->y      =  52; 
    pCtlDat->cx     =  40; 
    pCtlDat->cy     =  14; 
    pCtlDat->wID    = IDOK;
    pCtlDat->lStyle = BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD;
    pCtlDat->ClassID.class  = 0x80;
    _fstrcpy (pCtlDat->szText, "OK");
    pCtlDat = (PCONTROLDATA) (((PSTR) (pCtlDat->szText)) 
            + (_fstrlen (pCtlDat->szText) + 1) + CONTROLDATAPAD);

    /********************************************************************/
    /* Copyright & contact info static control                          */
    /********************************************************************/
    pCtlDat->x      =   1; 
    pCtlDat->y      =  68; 
    pCtlDat->cx     = 186; 
    pCtlDat->cy     =  26; 
    pCtlDat->wID    =   0;
    pCtlDat->lStyle = SS_CENTER | WS_VISIBLE | WS_CHILD;
    pCtlDat->ClassID.class  = 0x82;
    _fstrcpy (pCtlDat->szText, lpDlgBlk->szPrdNam);
    _fstrcat (pCtlDat->szText, "\n");
    _fstrcat (pCtlDat->szText, lpDlgBlk->szCopRgt);
    _fstrcat (pCtlDat->szText, "\n");
    _fstrcat (pCtlDat->szText, lpDlgBlk->szConAdr);
    pCtlDat = (PCONTROLDATA) (((PSTR) (pCtlDat->szText)) 
            + (_fstrlen (pCtlDat->szText) + 1) + CONTROLDATAPAD);

    /********************************************************************/
    /********************************************************************/
    CreateDialogIndirect (TBxGlo.hLibIns, (LPVOID) pDlgTmp, GetActiveWindow(), (DLGPROC) LicDlgPrc);
    hLocMem = LocUnLRel (hLocMem);

    /********************************************************************/
    /********************************************************************/
    return 0;

}

/************************************************************************/
/************************************************************************/
LONG FAR PASCAL LicDlgPrc(HWND hWnd, UINT iMessage, UINT wParam, LONG lParam)
{
//  Use of static causes GPF when exiting from QuickWin version of EXE
//  static HBRUSH   hDBxBsh;                        /* Background brush */

    static WORD usTmrAdv = NULL;
    DWORD       ulDBxClr = RGB (192, 192, 192);     /* Background color */
    HBRUSH      hDBxBsh;                            /* Background brush */
    LOGBRUSH    lbBsh;

    /********************************************************************/
    /********************************************************************/
    switch (iMessage) {
    case WM_INITDIALOG:
      /******************************************************************/
      /******************************************************************/
      CtrDlgBox (NULL, hWnd, NULL, CTR_CTR);
      usTmrAdv = SetTimer (hWnd, TI_ADVTMR, ADVPOLPER, NULL);
      /******************************************************************/
      /* Set focus on OK button, ask Windows not to override            */
      /******************************************************************/
      SetFocus (GetDlgItem (hWnd, IDOK));
      return FALSE;

    case WM_CTLCOLOR:
      /******************************************************************/
      /* Note: When created using DialogIndirect, the palette is not    */
      /* set to any known default. Therefore, use a stock object and    */
      /* query it for the correct background colors.                    */
      /******************************************************************/
      if ((CTLCOLOR_DLG == HIWORD (lParam)) 
        || (CTLCOLOR_STATIC == HIWORD (lParam))) { 
          if (!(hDBxBsh = GetStockObject (LTGRAY_BRUSH))) return (FALSE);
          if (GetObject (hDBxBsh, sizeof (lbBsh), (LPSTR) &lbBsh)) 
            ulDBxClr = lbBsh.lbColor;
          SetBkColor ((HDC) wParam, ulDBxClr);
          return ((LONG) hDBxBsh);
      }
      if ((CTLCOLOR_MSGBOX == HIWORD (lParam))) { 
          return ((LONG) GetStockObject (LTGRAY_BRUSH));
      }
      return ((LONG) NULL);

    case WM_TIMER:
    case WM_COMMAND:
      PostMessage (hWnd, WM_CLOSE, 0, 0L);
      break;

    case WM_CLOSE:
      DestroyWindow (hWnd);
      break;

    case WM_DESTROY:
      if (usTmrAdv) usTmrAdv = KillTimer (hWnd, TI_ADVTMR);
      break;
       
    default:
      /******************************************************************/
      /******************************************************************/
      return FALSE;

    }
    return TRUE;

}

