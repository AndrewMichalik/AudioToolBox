/************************************************************************/
/* Licensing Support Functions: LicSup.c                V2.00  04/15/96 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
WORD FAR PASCAL LicFilLod (LPSTR szCfgFil, VISMEMHDL FAR *pmhCfgMem)
{
    LPVOID       lpCfgBuf;
    struct _stat stFilSta;

    /********************************************************************/
    /********************************************************************/
    if (0 != FilGetSta (szCfgFil, &stFilSta)) return ((WORD) -1);
    if (NULL == (lpCfgBuf = GloAloLck (GMEM_MOVEABLE, pmhCfgMem, 
        min (stFilSta.st_size, CFGMAXBUF)))) return ((WORD) -1);
    
    /********************************************************************/
    /* Load szMapFil and compress all printables into LF term records   */
    /********************************************************************/
    if (CfgFilLod (szCfgFil, lpCfgBuf, CFGMAXBUF)) return ((WORD) -1);  

    /********************************************************************/
    /********************************************************************/
    GloMemUnL (*pmhCfgMem);
    return (0);

}

WORD FAR PASCAL LicFilRel (VISMEMHDL FAR *pmhCfgMem)
{
    return (((VISMEMHDL) NULL) != (*pmhCfgMem = GloAloRel (*pmhCfgMem)));
}

LPSTR FAR PASCAL LicStrXlt (LPSTR szDstStr, LPSTR szEncStr, WORD usMaxLen)
{
    WORD    usi = min (_fstrlen (szEncStr), usMaxLen);
    szDstStr[usi] = '\0';
    while (usi--) szDstStr[usi] = szEncStr[usi] ^ 0x07;
    return (szDstStr);
}

/////////////////////////////////////////////////////////////////////////////
// Audio ToolBox Installation DLL
// Copyright (c) 1987-1996 Andrew J. Michalik
/////////////////////////////////////////////////////////////////////////////
DWORD FAR PASCAL ChkRegSer (WORD usRsv001, LPCSTR szSerNum)
{
	/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	#define		DIGSUMCNT   10			// Number of digits to sum
	#define		DIGPOSMUL  100			// Digit position multiplier
	#define		SERMAXLEN   64			// Maximum serial number length
	#define		ASCNUMBAS 0x30			// ASCII numeric base value
	static char	szTmp[SERMAXLEN];		// Temporary work string
	char *		pTmp = szTmp;			// Temporary work string
	long		lSum = 0L;
	int			ii = 0;

	/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	if (!szSerNum) return (0L);

	/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	_fstrncpy (szTmp, szSerNum, SERMAXLEN - 1);
	szTmp[SERMAXLEN - 1] = '\0';

	/////////////////////////////////////////////////////////////////////////
	// Calculate the sum of the first DIGSUMCNT digits
	/////////////////////////////////////////////////////////////////////////
	while ((ii < DIGSUMCNT) && _fstrlen(pTmp)) {
		if (isdigit (*pTmp)) {
			lSum = lSum + (*pTmp - ASCNUMBAS);
			ii++;
		}
		pTmp++;
	}

	/////////////////////////////////////////////////////////////////////////
	// Return sum count if requested
	/////////////////////////////////////////////////////////////////////////
	if (usRsv001) return (3 * (lSum * lSum)); 

	/////////////////////////////////////////////////////////////////////////
	// Verify correct number of sum digits
	// Compare Mod of (3 * (sum * sum)) to last digits
	/////////////////////////////////////////////////////////////////////////
	if ((DIGSUMCNT == ii) && (((3 * (lSum * lSum)) % DIGPOSMUL) 
		== atol(pTmp))) return (TRUE);

	/////////////////////////////////////////////////////////////////////////
	return (FALSE);
}

DWORD FAR PASCAL ChkRegIns (WORD usRsv001, DWORD ulStrSum, LPCSTR szRegStr)
{
	WORD	usStrLen;
	DWORD	ulStrVal = 0L;
	#define	LICSTRLEN   80          	// Maximum string length

	/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	if (!szRegStr) return (0L);

	/////////////////////////////////////////////////////////////////////////
	// Sum non-whitespace characters preceding any ";" comment delimiter
	/////////////////////////////////////////////////////////////////////////
	usStrLen = min (_fstrlen (szRegStr), LICSTRLEN);
	while (usStrLen-- && (';' != szRegStr[usStrLen])) 
		if isgraph (szRegStr[usStrLen]) ulStrVal += szRegStr[usStrLen];	

	/////////////////////////////////////////////////////////////////////////
	// Generate checksum (3 * (sum * sum)) for input string
	/////////////////////////////////////////////////////////////////////////
	return (ulStrSum + (3 * (ulStrVal * ulStrVal)));

}

