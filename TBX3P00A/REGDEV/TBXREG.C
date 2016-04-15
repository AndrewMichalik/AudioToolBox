/************************************************************************/
/* ToolBox Demo Registration: TBxReg.c                  V2.00  01/10/93 */
/* Copyright (c) 1989-1993 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include <dos.h>                        /* DOS low-level routines       */

/************************************************************************/
/************************************************************************/
static  DWORD   _fstrtoul (LPSTR, LPSTR FAR *, int);
static  DWORD   _Day1970 ();

/************************************************************************/
/************************************************************************/
WORD    ChkRegKey (char far *pcOrgKeyArr, char far *pcSeqNumArr, WORD far *pusCmpCod)
{
    char    szWrkKey[REGKEYLEN+1];
    DWORD   ulKeyApp, ulKeyUsr, ulSeqNum, ulFulApp, ulCurDay;

    /********************************************************************/
    /********************************************************************/
    *pusCmpCod = 0;

    /********************************************************************/
    /* Keys stored in reversed strings to increase random appearance    */          
    /********************************************************************/
    _fstrncpy (szWrkKey, pcOrgKeyArr, REGKEYLEN);
    szWrkKey[REGKEYLEN] = '\0';
    _fstrrev (szWrkKey);

    ulKeyUsr = _fstrtoul (&szWrkKey[4], NULL, 36);      /* Convert tail */
    szWrkKey[4] = '\0';                                 /* Terminate    */
    ulKeyApp = _fstrtoul (&szWrkKey[0], NULL, 36);      /* Convert head */

    _fstrncpy (szWrkKey, pcSeqNumArr, SERSEGLEN);
    szWrkKey[SERSEGLEN] = '\0';
    ulSeqNum = _fstrtoul (&szWrkKey[0], NULL, 10);      /* Convert Seq# */

    ulCurDay = _Day1970 ();                             /* Since 1/1/70 */

    /********************************************************************/
    /* Check for "Demo Only" string                                     */
    /********************************************************************/
    if ((ulKeyApp == 0L) || (ulKeyUsr == 0L)) return (REGDEMKEY);

    /********************************************************************/
    /* Application Key is a prime number mult of ulFulApp (or master)   */
    /* Check App Key against multiple & check Usr Key against sequence# */
    /* Check App Key against master key multiplier                      */
    /********************************************************************/
    if ((0L == (ulKeyApp % atol (KEYEVLAPP))) && (ulKeyUsr > ulCurDay))
        return (REGEVLKEY);
    if ((0L == (ulKeyApp % atol (KEYSEQAPP))) && (ulKeyUsr == ulSeqNum))
        return (REGFULKEY);
    if ((0L == (ulKeyApp % atol (KEYMASAPP))) && (0L == (ulKeyApp % atol (KEYMASAPP))))
        return (REGMASKEY);

    /********************************************************************/
    /* Load ulFulApp divisor from hardware and check caller key strings */
    /********************************************************************/
    if (0 != (ulFulApp = GetEncKey (pcSeqNumArr, 0, pusCmpCod))) {
        if ((0L == (ulKeyApp % ulFulApp)) && (ulKeyUsr == ulSeqNum))
            return (REGFULKEY);
    }    

    /********************************************************************/
    /********************************************************************/
    return (REGDEMKEY);

}

/************************************************************************/
/************************************************************************/
WORD    GetEncKey (char far *pcSeqNumArr, WORD usKeyFld, WORD far *pusCmpCod)
{
    char    szWrkKey[SERSEGLEN+1];
    SCB     SCB;
    DWORD   ulSeqNum; 

    /********************************************************************/
    /********************************************************************/
    *pusCmpCod = 0;

    /********************************************************************/
    /********************************************************************/
    _fstrncpy (szWrkKey, pcSeqNumArr, SERSEGLEN);
    szWrkKey[SERSEGLEN] = '\0';
    ulSeqNum = _fstrtoul (&szWrkKey[0], NULL, 10);      /* Convert Seq# */

    /********************************************************************/
    /********************************************************************/
    _fstrcpy(SCB.id, "..?Z");           /* Signature Literal            */
    SCB.return_code = -1;               /* Init to invalid return code  */
    SCB.function_code = 1;              /* Code 0, 1, or 2              */
    _fstrcpy(SCB.product_name, "ToolBox");
    SCB.pin_number = 2046833937;        /*  Verify Personal ID Number   */

    /********************************************************************/
    /********************************************************************/
    SCB.product_serial = ulSeqNum;

    /********************************************************************/
    /********************************************************************/
    if (*pusCmpCod = KECHK (&SCB)) {
        if (TBxGlo.usDebFlg & KEY___DEB) 
            MsgDspUsr ("Accelerator I/O Error: %04x\n", *pusCmpCod);
        return (0);
    }
    if (SCB.return_code) {
        if (TBxGlo.usDebFlg & KEY___DEB) 
            MsgDspUsr ("Accelerator Access Error: %04x-%04x-%04x\n",
                SCB.return_code, SCB.return_status, SCB.return_function);
        *pusCmpCod = 0x8000;            // Force completion code non-zero
        return (0);
    }
    return (((WORD *) SCB.user_data)[usKeyFld]);

}

/************************************************************************/
/************************************************************************/
DWORD   _fstrtoul (LPSTR lpString, LPSTR FAR *ppStrEnd, int iRadix)
{
    static char szWrkBuf[TBXMAXSTR];
    static char *pcTmpPtr;
    DWORD       ulRetVal;

    /********************************************************************/
    /********************************************************************/
    _fstrncpy (szWrkBuf, lpString, TBXMAXSTR);
    ulRetVal = strtoul (szWrkBuf, &pcTmpPtr, iRadix);
    if (ppStrEnd) *ppStrEnd = pcTmpPtr;
    return (ulRetVal);
}

/************************************************************************/
/************************************************************************/
static char tbDayTab[2][13] = {
    { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};
DayOfYear (WORD usMon, WORD usDay, WORD usYear)
{
    WORD    usi;
    WORD    usLeap;
    /********************************************************************/
    /********************************************************************/
    usLeap = usYear%4 == 0 && usYear%100 != 0 || usYear%400 == 0;
    for (usi = 1; usi < usMon; usi++) usDay += tbDayTab[usLeap][usi];
    return (usDay);
}
DWORD   _Day1970 ()
{
    static  struct _dosdate_t ddate;
    _dos_getdate (&ddate);
    return (((ddate.year - 1970) * 365) + ((ddate.year - 1968) / 4)
        + (DayOfYear (ddate.month, ddate.day, ddate.year) - 1));
}
