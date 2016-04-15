/************************************************************************/
/* Summa Four u-Law / A-Law Converter: Su4Cvt.c         V1.10  09/10/93 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "..\fiodev\filutl.h"           /* File utilities definitions   */
#include "su4hdr.h"                     /* Summa Four header support    */
#include "su4chp.h"                     /* Summa Four chop support      */
#include "g11xl1.tab"                   /* u-A, A-u conversion tables   */

#include <io.h>                         /* Low level file I/O           */

/************************************************************************/
/************************************************************************/
WORD    Su4uLwToA (BYTE FAR *, WORD);
WORD    Su4ALwTou (BYTE FAR *, WORD);

/************************************************************************/
/************************************************************************/
long    Su4CopCvt (short sSrcHdl, short sDstHdl, LPSTR lpWrkBuf, 
        WORD usBufSiz, WORD usSrcFmt, WORD usDstFmt, unsigned int far *lpErrCod)
{
    WORD        usTmpCnt = 0;
    DWORD       ulXfrCnt = 0L;
    DWORD       ulReqCnt = FilGetLen (sSrcHdl) - FilGetPos (sSrcHdl);

    /********************************************************************/
    /********************************************************************/
    if ((0L == ulReqCnt) || (-1L == ulReqCnt)) return (ulReqCnt);

    /********************************************************************/
    /********************************************************************/
    while (usTmpCnt != -1) {
        usTmpCnt = Rd_FilFwd (sSrcHdl, lpWrkBuf, usBufSiz, FIOENCNON, lpErrCod);
        if ((-1 == usTmpCnt) || (0 == usTmpCnt)) break;
        if (SU4PCMA08 == usDstFmt) Su4uLwToA (lpWrkBuf, usTmpCnt);
        if (SU4PCMA08 == usSrcFmt) Su4ALwTou (lpWrkBuf, usTmpCnt);
        usTmpCnt = Wr_FilFwd (sDstHdl, lpWrkBuf, usTmpCnt, FIOENCNON, lpErrCod);
        if ((-1 == usTmpCnt) || (0 == usTmpCnt)) break;
        ulXfrCnt += (DWORD) usTmpCnt;
    }

    /********************************************************************/
    /********************************************************************/
    if (-1 == usTmpCnt) return (-1L);
    return (ulXfrCnt);

}

/************************************************************************/
/* Summa Four unfolded u-Law to A-Law conversion                        */
/************************************************************************/
#define     MAGMSK  ((BYTE)0x7f)        /* Magnitude mask               */
#define     SGNMSK  ((BYTE)0x80)        /* Sign mask                    */
#define     INVMSK  ((BYTE)0x2a)        /* Inversion mask (no sign)     */

WORD    Su4uLwToA (BYTE FAR *lpWrkBuf, WORD usSmpCnt)
{
    BYTE    ucWrkByt;

    /********************************************************************/
    /* Note: uLaw NOT unfolded!                                         */
    /********************************************************************/
    while (usSmpCnt--) {
        ucWrkByt = *lpWrkBuf & MAGMSK;
        switch (ucWrkByt) {
            default:   ucWrkByt = u_to_A[ucWrkByt];
        }
        *lpWrkBuf++ = (ucWrkByt ^ INVMSK) | (*lpWrkBuf & SGNMSK);
    }

    /********************************************************************/
    /********************************************************************/
    return (0);

}

WORD    Su4ALwTou (BYTE FAR *lpWrkBuf, WORD usSmpCnt)
{
    BYTE    ucWrkByt;

    /********************************************************************/
    /* Note: uLaw NOT folded!                                           */
    /********************************************************************/
    while (usSmpCnt--) {
        ucWrkByt = (*lpWrkBuf & MAGMSK) ^ INVMSK;
        switch (ucWrkByt) {
            default:   ucWrkByt = A_to_u[ucWrkByt];
        }
        *lpWrkBuf++ = ucWrkByt | (*lpWrkBuf & SGNMSK);
    }

    /********************************************************************/
    /********************************************************************/
    return (0);

}

