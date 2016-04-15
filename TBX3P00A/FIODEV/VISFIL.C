/************************************************************************/
/* VIS InterChange File Routines (DOS): VISFil.c        V2.00  07/15/94 */
/* Copyright (c) 1987-1994 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "..\os_dev\dosmem.h"           /* Generic memory support defs  */
#include "filutl.h"                     /* File utilities definitions   */
#include "visfil.h"                     /* VIS InterChange definitions  */
#include "wavfil.h"                     /* Wave file (DOS) definitions  */
#include "..\pcmdev\genpcm.h"           /* PCM/APCM conv routine defs   */

#include <stdio.h>                      /* Standard I/O                 */

/************************************************************************/
/************************************************************************/
typedef struct _SMPBLK {                /* Sample description block     */
    PCMTYP  usPCMTyp;
    WORD    usChnCnt; 
    DWORD   ulSmpFrq; 
} SMPBLK;

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL VIXFndFst (short sFilHdl, LPSTR szChkTyp, DWORD ulBakLnk, 
                DWORD FAR *pulNxtLnk, WORD usEncFmt)
{
    VIXHDR      vhVIXHdr;
    unsigned    uiErrCod;

    /********************************************************************/
    /* Read "VISI" header                                               */
    /********************************************************************/
    if (sizeof (VIXHDR) != Rd_FilFwd (sFilHdl, (LPSTR) &vhVIXHdr,
        sizeof (VIXHDR), usEncFmt, &uiErrCod)) return (SI_INVFILFMT);

    /********************************************************************/
    /* Read "WAVE" header                                               */
    /********************************************************************/
    if (sizeof (VIXHDR) != Rd_FilFwd (sFilHdl, (LPSTR) &vhVIXHdr,
        sizeof (VIXHDR), usEncFmt, &uiErrCod)) return (SI_INVFILFMT);
    if (vhVIXHdr.ulChk_ID != mmioFOURCC ('W', 'A', 'V', 'E')) return (SI_INVFILFMT);
    if (-1L == FilSetPos (sFilHdl, - (long) sizeof (VIXHDR), SEEK_CUR)) return (SI_INVFILFMT);

    /********************************************************************/
    /********************************************************************/
    if (pulNxtLnk) *pulNxtLnk = FilGetPos (sFilHdl);
    return (0);

}

WORD FAR PASCAL VIXFndNxt (short sFilHdl, DWORD ulMaxOff, WORD usEncFmt)
{
    VIXHDR      vhVIXHdr;
    unsigned    uiErrCod;

    /********************************************************************/
    /********************************************************************/
    if (sizeof (VIXHDR) != Rd_FilFwd (sFilHdl, (LPSTR) &vhVIXHdr,
        sizeof (VIXHDR), usEncFmt, &uiErrCod)) return (SI_INVFILFMT);

    /********************************************************************/
    /********************************************************************/
    if (-1L == FilSetPos (sFilHdl, vhVIXHdr.ulChkSiz, SEEK_CUR)) return (SI_INVFILFMT);

    /********************************************************************/
    /********************************************************************/
    return (0);

}

WORD FAR PASCAL VIXDesCur (short sFilHdl, DWORD FAR *pulMaxOff, WORD usEncFmt)
{
    VIXHDR      vhVIXHdr;
    unsigned    uiErrCod;

    /********************************************************************/
    /* Read current VIX header                                          */
    /********************************************************************/
    if (sizeof (VIXHDR) != Rd_FilFwd (sFilHdl, (LPSTR) &vhVIXHdr,
        sizeof (VIXHDR), usEncFmt, &uiErrCod)) return (SI_INVFILFMT);
    if (pulMaxOff) *pulMaxOff = FilGetPos (sFilHdl) + vhVIXHdr.ulChkSiz;

    /********************************************************************/
    /********************************************************************/
    return (0);

}

WORD PASCAL FAR VIXLodTyp (short sFilHdl, enum _PCMTYP FAR *pusPCMTyp, 
                WORD FAR *pusChnCnt, DWORD FAR *pulSmpFrq, WORD usEncFmt)
{
    VIXHDR      vhVIXHdr;
    SMPBLK      sbSmpBlk;
    unsigned    uiErrCod;
    DWORD       ulOrgOff = FilGetPos (sFilHdl);

    /********************************************************************/
    /* Read "TYPE" header & info                                        */
    /********************************************************************/
    if (sizeof (VIXHDR) != Rd_FilFwd (sFilHdl, (LPSTR) &vhVIXHdr,
        sizeof (VIXHDR), usEncFmt, &uiErrCod)) return (SI_INVFILFMT);
    if (sizeof (SMPBLK) != Rd_FilFwd (sFilHdl, (LPSTR) &sbSmpBlk,
        sizeof (SMPBLK), usEncFmt, &uiErrCod)) return (SI_INVFILFMT);

    /********************************************************************/
    /********************************************************************/
    *pusPCMTyp = sbSmpBlk.usPCMTyp;
    *pusChnCnt = sbSmpBlk.usChnCnt;
    *pulSmpFrq = sbSmpBlk.ulSmpFrq;

    /********************************************************************/
    /********************************************************************/
    if (-1L == FilSetPos (sFilHdl, ulOrgOff, SEEK_SET)) return (SI_INVFILFMT);
    return (0);

}

WORD FAR PASCAL VIXWrtFst (short sFilHdl, LPSTR szChkTyp, DWORD ulBakLnk, 
                DWORD FAR *pulNxtLnk, WORD usEncFmt)
{
    VIXHDR      vhVIXHdr;
    unsigned    uiErrCod;

    /********************************************************************/
    /********************************************************************/
    vhVIXHdr.ulChk_ID = mmioFOURCC ('V', 'I', 'S', 'I');
    vhVIXHdr.ulFmtVer = 0L;
    vhVIXHdr.ulFmtTyp = 0L;
    vhVIXHdr.ulChkSiz = 0L;
    if (-1L == FilSetPos (sFilHdl, ulBakLnk, SEEK_SET)) return (SI_BADOVRWRT);
    if (sizeof (VIXHDR) != (WORD) Wr_FilFwd (sFilHdl, (LPSTR) &vhVIXHdr,
        sizeof (VIXHDR), usEncFmt, &uiErrCod)) return (SI_BADOVRWRT);                          /* No Overwrite */

    /********************************************************************/
    /********************************************************************/
    if (pulNxtLnk) *pulNxtLnk = FilGetPos (sFilHdl);

    /********************************************************************/
    /********************************************************************/
    vhVIXHdr.ulChk_ID = mmioFOURCC ('W', 'A', 'V', 'E');
    vhVIXHdr.ulFmtVer = 0L;
    vhVIXHdr.ulFmtTyp = 0L;
    vhVIXHdr.ulChkSiz = 0L;
    if (sizeof (VIXHDR) != (WORD) Wr_FilFwd (sFilHdl, (LPSTR) &vhVIXHdr,
        sizeof (VIXHDR), usEncFmt, &uiErrCod)) return (SI_BADOVRWRT);                          /* No Overwrite */

    /********************************************************************/
    /********************************************************************/
    return (0);

}

WORD PASCAL FAR VIXWrtTyp (short sFilHdl, enum _PCMTYP usPCMTyp, 
                WORD usChnCnt, DWORD ulSmpFrq, WORD usEncFmt)
{
    VIXHDR      vhVIXHdr;
    SMPBLK      sbSmpBlk;
    unsigned    uiErrCod;

    /********************************************************************/
    /********************************************************************/
    vhVIXHdr.ulChk_ID = mmioFOURCC ('T', 'Y', 'P', 'E');
    vhVIXHdr.ulFmtVer = 0L;
    vhVIXHdr.ulFmtTyp = 0L;
    vhVIXHdr.ulChkSiz = sizeof (SMPBLK);
    if (sizeof (VIXHDR) != (WORD) Wr_FilFwd (sFilHdl, (LPSTR) &vhVIXHdr,
        sizeof (VIXHDR), usEncFmt, &uiErrCod)) return (SI_BADOVRWRT);                          /* No Overwrite */

    /********************************************************************/
    /********************************************************************/
    sbSmpBlk.usPCMTyp = usPCMTyp;
    sbSmpBlk.usChnCnt = usChnCnt;
    sbSmpBlk.ulSmpFrq = ulSmpFrq;
    if (sizeof (SMPBLK) != (WORD) Wr_FilFwd (sFilHdl, (LPSTR) &sbSmpBlk,
        sizeof (SMPBLK), usEncFmt, &uiErrCod)) return (SI_BADOVRWRT);                          /* No Overwrite */

    /********************************************************************/
    /********************************************************************/
    return (0);

}

WORD PASCAL FAR VIXWrtDat (short sFilHdl, DWORD FAR *pulMaxOff, WORD usEncFmt)
{
    VIXHDR      vhVIXHdr;
    unsigned    uiErrCod;

    /********************************************************************/
    /********************************************************************/
    vhVIXHdr.ulChk_ID = mmioFOURCC ('D', 'A', 'T', 'A');
    vhVIXHdr.ulFmtVer = 0L;
    vhVIXHdr.ulFmtTyp = 0L;
    vhVIXHdr.ulChkSiz = 0L;
    if (sizeof (VIXHDR) != (WORD) Wr_FilFwd (sFilHdl, (LPSTR) &vhVIXHdr,
        sizeof (VIXHDR), usEncFmt, &uiErrCod)) return (SI_BADOVRWRT);                          /* No Overwrite */

    /********************************************************************/
    /********************************************************************/
    return (0);

}

WORD FAR PASCAL VIXAscCur (short sFilHdl, DWORD FAR *pulMaxOff, WORD usEncFmt)
{
    VIXHDR      vhVIXHdr;
    unsigned    uiErrCod;

    /********************************************************************/
    /* Read current VIX header                                          */
    /********************************************************************/
    if (sizeof (VIXHDR) != Rd_FilFwd (sFilHdl, (LPSTR) &vhVIXHdr,
        sizeof (VIXHDR), usEncFmt, &uiErrCod)) return (SI_INVFILFMT);
    if (pulMaxOff) *pulMaxOff = FilGetPos (sFilHdl) + vhVIXHdr.ulChkSiz;

    /********************************************************************/
    /********************************************************************/
    return (0);

}


