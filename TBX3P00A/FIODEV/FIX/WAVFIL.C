/************************************************************************/
/* Wave File Routines (DOS): WavFil.c                   V2.00  10/15/96 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
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
#include "fiosup.h"                     /* File I/O support definitions */
#include "filutl.h"                     /* File utilities definitions   */
#include "wavfil.h"                     /* Wave file (DOS) definitions  */
#include "..\pcmdev\genpcm.h"           /* PCM/APCM conv routine defs   */

#include <string.h>                     /* String manipulation funcs    */
#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <stdio.h>                      /* Standard I/O                 */
  
/************************************************************************/
/*                      Forward References                              */
/************************************************************************/
WORD    FndNxtChk (HMMIO, FOURCC, FOURCC, DWORD FAR *);
DWORD   WIOFilLen (HMMIO);

/************************************************************************/
/************************************************************************/
typedef struct _CHKBLK {
        FOURCC          ccChk_ID;       /* chunk ID                     */
        DWORD           ulChkSiz;       /* chunk size                   */
} CHKBLK;

/************************************************************************/
/************************************************************************/
WORD PASCAL FAR WIOFndNxt (HMMIO hFilHdl, enum _PCMTYP FAR *pusPCMTyp, 
                VISMEMHDL FAR *phwfChkFmt, MMCKINFO FAR *lpck)
{
    WORD        usRetCod;
    MMCKINFO    ciParChk;
    LPITCB      pibChkFmt;

    /********************************************************************/
    /********************************************************************/
    ciParChk.fccType = mmioFOURCC ('W', 'A', 'V', 'E');
    if (usRetCod = mmioDescend (hFilHdl, &ciParChk, (MMCKINFO FAR *) NULL, 
        MMIO_FINDRIFF)) return (usRetCod);

    /********************************************************************/
    /********************************************************************/
    lpck->ckid = mmioFOURCC ('f', 'm', 't', ' ');
    if (usRetCod = mmioDescend (hFilHdl, lpck, &ciParChk, MMIO_FINDCHUNK))
        return (usRetCod);

    /********************************************************************/
    /********************************************************************/
    if (NULL == (pibChkFmt = GloAloLck (GMEM_MOVEABLE, phwfChkFmt, 
        sizeof (*pibChkFmt)))) return (MMIOERR_CANNOTREAD);

    if (sizeof (PCMWAVEFORMAT) != mmioRead (hFilHdl, 
        (LPSTR) &pibChkFmt->mwMSW.afWEX.wfx, sizeof (PCMWAVEFORMAT)))
        return (MMIOERR_CANNOTREAD);

    if (WAVE_FORMAT_PCM != pibChkFmt->mwMSW.afWEX.wfx.wFormatTag) {
        if (sizeof (pibChkFmt->mwMSW.afWEX.wfx.cbSize) != mmioRead (hFilHdl, 
          (LPSTR) &pibChkFmt->mwMSW.afWEX.wfx.cbSize,
          sizeof (pibChkFmt->mwMSW.afWEX.wfx.cbSize))) {
            *phwfChkFmt = GloUnLRel (*phwfChkFmt);
            return (MMIOERR_CANNOTREAD);
        }
        if (pibChkFmt->mwMSW.afWEX.wfx.cbSize != (BYTE) mmioRead (hFilHdl, 
          (LPSTR) &pibChkFmt->mwMSW.afWEX.wSamplesPerBlock,
          pibChkFmt->mwMSW.afWEX.wfx.cbSize)) {
            *phwfChkFmt = GloUnLRel (*phwfChkFmt);
            return (MMIOERR_CANNOTREAD);
        }
    }

    if (usRetCod = mmioAscend (hFilHdl, lpck, 0)) return (usRetCod);

    /********************************************************************/
    /********************************************************************/
    switch (pibChkFmt->mwMSW.afWEX.wfx.wFormatTag) {
        case WAVE_FORMAT_PCM:
            switch (pibChkFmt->mwMSW.afWEX.wfx.wBitsPerSample) { 
                case  8: *pusPCMTyp = MPCPCM008; break;
                case 16: *pusPCMTyp = MPCPCM016; break;
                default: *pusPCMTyp = UNKPCM000; break;
            }
            break;
        case WAVE_FORMAT_MULAW:     
            *pusPCMTyp = G11PCMF08; 
            break;
        case WAVE_FORMAT_DIALOGIC_OKI_ADPCM:
            *pusPCMTyp = DLGPCM004; 
            break; 
        case WAVE_FORMAT_ADPCM:    
            *pusPCMTyp = MSAPCM004; 
            break; 
        case WAVE_FORMAT_ALAW:
            *pusPCMTyp = G11PCMI08; 
            break; 
        case WAVE_FORMAT_IBM_CVSD: 
        case WAVE_FORMAT_DIGISTD:  
        case WAVE_FORMAT_DIGIFIX:  
        default:
            *pusPCMTyp = UNKPCM000; 
            break;
    }

    /********************************************************************/
    /********************************************************************/
    lpck->ckid = mmioFOURCC ('d', 'a', 't', 'a');
    if (usRetCod = mmioDescend (hFilHdl, lpck, &ciParChk, MMIO_FINDCHUNK))
        return (usRetCod);

    /********************************************************************/
    /********************************************************************/
    return (0);

}

WORD PASCAL FAR WIOOutIni (HMMIO hFilHdl, enum _PCMTYP usPCMTyp, WORD usChnCnt,
                DWORD ulSmpFrq, VISMEMHDL FAR *phwfChkFmt, MMCKINFO FAR *lpck)
{

    WORD        usRetCod;
    LPITCB      pibChkFmt;

    /********************************************************************/
    /********************************************************************/
    if (NULL == (pibChkFmt = GloAloLck (GMEM_MOVEABLE, phwfChkFmt, 
        sizeof (*pibChkFmt)))) return (MMIOERR_CANNOTWRITE);

    /********************************************************************/
    /********************************************************************/
    lpck->fccType = mmioFOURCC ('W', 'A', 'V', 'E');
    mmioCreateChunk (hFilHdl, lpck, MMIO_CREATERIFF);

    /********************************************************************/
    /********************************************************************/
    switch (usPCMTyp) {
        case DLGPCM004:
            pibChkFmt->mwMSW.afWEX.wfx.wFormatTag = WAVE_FORMAT_DIALOGIC_OKI_ADPCM;
            pibChkFmt->mwMSW.afWEX.wfx.wBitsPerSample = 4;
            pibChkFmt->mwMSW.afWEX.wfx.nBlockAlign = 1;
            pibChkFmt->mwMSW.afWEX.wfx.cbSize = 0;
            lpck->cksize = sizeof (WAVEFORMATEX);
            break;
        case DLGPCM008: 
        case G11PCMF08: 
            pibChkFmt->mwMSW.afWEX.wfx.wFormatTag = WAVE_FORMAT_MULAW;
            pibChkFmt->mwMSW.afWEX.wfx.wBitsPerSample = 8;
            pibChkFmt->mwMSW.afWEX.wfx.nBlockAlign = 1;
            pibChkFmt->mwMSW.afWEX.wfx.cbSize = 0;
            lpck->cksize = sizeof (WAVEFORMATEX);
            break;
        case G11PCMI08: 
            pibChkFmt->mwMSW.afWEX.wfx.wFormatTag = WAVE_FORMAT_ALAW;
            pibChkFmt->mwMSW.afWEX.wfx.wBitsPerSample = 8;
            pibChkFmt->mwMSW.afWEX.wfx.nBlockAlign = 1;
            pibChkFmt->mwMSW.afWEX.wfx.cbSize = 0;
            lpck->cksize = sizeof (WAVEFORMATEX);
            break;
        case MPCPCM008:
            pibChkFmt->mwMSW.afWEX.wfx.wFormatTag = WAVE_FORMAT_PCM;
            pibChkFmt->mwMSW.afWEX.wfx.wBitsPerSample = 8;
            pibChkFmt->mwMSW.afWEX.wfx.nBlockAlign = 1;
            lpck->cksize = sizeof (PCMWAVEFORMAT);
            break;
        case MPCPCM016:
            pibChkFmt->mwMSW.afWEX.wfx.wFormatTag = WAVE_FORMAT_PCM;
            pibChkFmt->mwMSW.afWEX.wfx.wBitsPerSample = 16;
            pibChkFmt->mwMSW.afWEX.wfx.nBlockAlign = 2;
            lpck->cksize = sizeof (PCMWAVEFORMAT);
            break;
        default:
            return (MMIOERR_CANNOTWRITE);
    }
    pibChkFmt->mwMSW.afWEX.wfx.nChannels = usChnCnt;
    pibChkFmt->mwMSW.afWEX.wfx.nSamplesPerSec = ulSmpFrq;
    pibChkFmt->mwMSW.afWEX.wfx.nAvgBytesPerSec 
        = pibChkFmt->mwMSW.afWEX.wfx.nSamplesPerSec * pibChkFmt->mwMSW.afWEX.wfx.wBitsPerSample / 8L;

    /********************************************************************/
    /********************************************************************/
    lpck->ckid = mmioFOURCC ('f', 'm', 't', ' ');
    mmioCreateChunk (hFilHdl, lpck, 0);

    /********************************************************************/
    /********************************************************************/
    if (lpck->cksize != (DWORD) mmioWrite (hFilHdl, 
        (LPSTR) &pibChkFmt->mwMSW.afWEX.wfx, lpck->cksize))
        return (MMIOERR_CANNOTWRITE);
    if (usRetCod = mmioAscend (hFilHdl, lpck, 0)) return (usRetCod);

    /********************************************************************/
    /********************************************************************/
    lpck->ckid = mmioFOURCC ('d', 'a', 't', 'a');
    lpck->cksize = 0;
    mmioCreateChunk (hFilHdl, lpck, 0);

    /********************************************************************/
    /********************************************************************/
    return (0);

}

DWORD FAR PASCAL WIOFilCop (HMMIO hSrcHdl, HMMIO hDstHdl, DWORD ulReqCnt,
                FIOPOLPRC fpLngPolPrc, DWORD ulPolDat)
{
    VISMEMHDL   hgGloMem;
    LPSTR       lpWrkBuf;
    DWORD       ulBufSiz;
    DWORD       ulTmpCnt = 0;
    DWORD       ulXfrCnt = 0L;

    /********************************************************************/
    /* If no bytes requested or available, return OK                    */
    /* Future: check current file position for available bytes          */
    /********************************************************************/
    ulReqCnt = min (ulReqCnt, WIOFilLen (hSrcHdl));
    if (0L == ulReqCnt) return (ulReqCnt);

    /********************************************************************/
    /* Allocate Global memory in multiples of 1024L and < 63Kbyte       */
    /********************************************************************/
    if ((HANDLE) NULL == (hgGloMem = GloAloBlk (GMEM_FIXED, FIOGLOBLK, FIOGLOMIN,
        FIOGLOMAX, &ulBufSiz))) return ((DWORD) -1L);
    if ((LPSTR) NULL == (lpWrkBuf = (LPSTR) GloMemLck (hgGloMem))) {
        GloAloRel (hgGloMem);
        return ((DWORD) -1L);
    }

    /********************************************************************/
    /* Show/Hide Progress indicator for time consuming operations       */
    /********************************************************************/
    if (fpLngPolPrc) fpLngPolPrc (FIOPOLBEG, ulReqCnt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    while (-1 != ulTmpCnt) {
        ulTmpCnt = mmioRead (hSrcHdl, lpWrkBuf, min (ulReqCnt, ulBufSiz));
        if ((-1L == ulTmpCnt) || (0L == ulTmpCnt)) break;
        ulTmpCnt = mmioWrite (hDstHdl, lpWrkBuf, ulTmpCnt);
        if ((-1L == ulTmpCnt) || (0L == ulTmpCnt)) break;
        ulReqCnt -= ulTmpCnt;
        ulXfrCnt += ulTmpCnt;
        if (fpLngPolPrc && !fpLngPolPrc (FIOPOLCNT,  ulXfrCnt, ulPolDat)) {
            ulTmpCnt = (DWORD) -1L;
            break;
        }
    }
    if (fpLngPolPrc) fpLngPolPrc (FIOPOLEND,  ulXfrCnt, ulPolDat);

    /********************************************************************/
    /********************************************************************/
    GloUnLRel (hgGloMem);

    /********************************************************************/
    /********************************************************************/
    return (ulXfrCnt);
}

WORD PASCAL FAR WIOOutEnd (HMMIO hFilHdl, VISMEMHDL FAR *phwfChkFmt, MMCKINFO FAR *lpck)
{
    WORD        usRetCod;
    DWORD       ulRifLen;

    /********************************************************************/
    /* Get current file position; subtract out header for chunk length. */
    /********************************************************************/
    ulRifLen = ((long) mmioSeek (hFilHdl, 0L, SEEK_CUR)) - (CHKNAMLEN + CHKSIZLEN);
    if (usRetCod = mmioAscend (hFilHdl, lpck, 0)) return (usRetCod);

#if (defined (DOS)) /****************************************************/
// Apparently, mmio routines position the file pointer to the last byte
// written, thereby giving the actual location for the "current position".
// When using the DOS file I/O routine, a write leaves the file pointer
// at one past the last byte written; therefore, subtract one for 
// the empty byte position.
ulRifLen -= 1L;
#endif /*****************************************************************/

    /********************************************************************/
    /********************************************************************/
    if (-1L == mmioSeek (hFilHdl, CHKNAMLEN, SEEK_SET)) return (MMIOERR_CANNOTSEEK);
    mmioWrite (hFilHdl, (LPSTR) &ulRifLen, CHKSIZLEN);

    /********************************************************************/
    /********************************************************************/
    *phwfChkFmt = GloUnLRel (*phwfChkFmt);

    /********************************************************************/
    /********************************************************************/
    return (0);

}

WORD    FndNxtChk (HMMIO hmmio, FOURCC ccChk_ID, FOURCC ccFrm_ID, 
        DWORD FAR *pulChkSiz)
{
    CHKBLK      cbChkBlk;

    /********************************************************************/
    /********************************************************************/
    while (sizeof (cbChkBlk) == mmioRead (hmmio, (LPSTR) &cbChkBlk,
      sizeof (cbChkBlk))) {
        if (!_fmemcmp (&cbChkBlk.ccChk_ID, &ccChk_ID, CHKNAMLEN)) {
            *pulChkSiz = cbChkBlk.ulChkSiz;
            if (!ccFrm_ID) return (0);
            /************************************************************/
            /************************************************************/
            if (CHKNAMLEN != mmioRead (hmmio, (LPSTR) &cbChkBlk.ccChk_ID,
                CHKNAMLEN)) return (MMIOERR_CANNOTREAD);
            if (!_fmemcmp (&cbChkBlk.ccChk_ID, &ccFrm_ID, CHKNAMLEN)) return (0);      
            mmioSeek (hmmio, -CHKNAMLEN, SEEK_CUR);
        }
        if (-1L == mmioSeek (hmmio, cbChkBlk.ulChkSiz, SEEK_CUR)) return (MMIOERR_CANNOTSEEK);
    }        
    return (MMIOERR_CHUNKNOTFOUND);

}

DWORD   WIOFilLen (HMMIO hFilHdl)
{
	DWORD   ulFilPos;
	DWORD   ulFilLen;
    /********************************************************************/
    if (-1L == (ulFilPos = mmioSeek (hFilHdl, 0L, SEEK_CUR))) return (0L);
    if (-1L == (ulFilLen = mmioSeek (hFilHdl, 0L, SEEK_END))) return (0L);
    if (-1L == mmioSeek (hFilHdl, ulFilPos, SEEK_SET)) return (0L);
    return (ulFilLen);
}

/************************************************************************/
/*                  DOS Low level support routines                      */
/************************************************************************/
#if (defined (DOS)) /****************************************************/

HMMIO PASCAL FAR mmioOpen (LPSTR szFilNam, MMIOINFO FAR* lpmmioinfo,
                 DWORD dwOpenFlags)
{
    static  char    szLocNam[WIOMAXNAM];

    /********************************************************************/
    /********************************************************************/
    _fstrncpy (szLocNam, szFilNam, WIOMAXNAM);
    return (FilHdlOpn (szLocNam, (int) dwOpenFlags));

}
LONG  PASCAL FAR mmioSeek (HMMIO hmmio, LONG lOffset, int iOrigin)
{
    return (FilSetPos (hmmio, lOffset, iOrigin));
}
UINT  PASCAL FAR mmioClose (HMMIO hmmio, UINT uFlags)
{
    return (FilHdlCls (hmmio));
}
LONG  PASCAL FAR mmioRead (HMMIO hmmio, HPSTR pch, LONG cch)
{
    unsigned    uiErrCod;
    return (Rd_FilFwd (hmmio, (LPSTR) pch, (WORD) cch, FIOENCNON, &uiErrCod));
}
LONG  PASCAL FAR mmioWrite (HMMIO hmmio, const char _huge* pch, LONG cch)
{
    unsigned    uiErrCod;
    return (Wr_FilFwd (hmmio, (LPSTR) pch, (WORD) cch, FIOENCNON, &uiErrCod));

}
UINT PASCAL FAR mmioCreateChunk (HMMIO hmmio, MMCKINFO FAR* lpck, UINT uFlags)
{
    unsigned    uiErrCod;

    /********************************************************************/
    /********************************************************************/
    if (MMIO_CREATERIFF == uFlags) {
        lpck->ckid = FOURCC_RIFF;
        lpck->cksize = CHKNAMLEN;
        Wr_FilFwd (hmmio, (LPSTR) &lpck->ckid, CHKNAMLEN, FIOENCNON, &uiErrCod);
        Wr_FilFwd (hmmio, (LPSTR) &lpck->cksize, CHKSIZLEN, FIOENCNON, &uiErrCod);
        Wr_FilFwd (hmmio, (LPSTR) &lpck->fccType, CHKNAMLEN, FIOENCNON, &uiErrCod);
        lpck->cksize = 0;
        lpck->dwDataOffset = FilGetPos (hmmio);
        lpck->dwFlags = MMIO_DIRTY;
    }
    else {
        Wr_FilFwd (hmmio, (LPSTR) &lpck->ckid, CHKNAMLEN, FIOENCNON, &uiErrCod);
        Wr_FilFwd (hmmio, (LPSTR) &lpck->cksize, CHKSIZLEN, FIOENCNON, &uiErrCod);
        lpck->dwDataOffset = FilGetPos (hmmio);
        lpck->dwFlags = MMIO_DIRTY;
    }

    /********************************************************************/
    /********************************************************************/
    return (0);

}
UINT PASCAL FAR mmioDescend (HMMIO hmmio, MMCKINFO FAR* lpck, 
                const MMCKINFO FAR* lpckParent, UINT uFlags)
{

    /********************************************************************/
    /********************************************************************/
    if (MMIO_FINDRIFF == uFlags) {
        lpck->ckid = FOURCC_RIFF;
        if (FndNxtChk (hmmio, lpck->ckid, lpck->fccType, &lpck->cksize)) 
            return (MMIOERR_CHUNKNOTFOUND);
    }        
    else {
        if (FndNxtChk (hmmio, lpck->ckid, 0L, &lpck->cksize)) 
            return (MMIOERR_CHUNKNOTFOUND);
    }        

    /********************************************************************/
    /********************************************************************/
    lpck->dwDataOffset = FilGetPos (hmmio);
    lpck->dwFlags = 0L;
    return (0);

}
UINT PASCAL FAR mmioAscend (HMMIO hmmio, MMCKINFO FAR* lpck, UINT uFlags)
{

    unsigned    uiErrCod;
    DWORD       ulCurPos;
    BYTE        ucPadByt = 0;

    /********************************************************************/
    /********************************************************************/
    if (lpck->dwFlags & MMIO_DIRTY) {
        if ((ulCurPos = FilGetPos (hmmio)) % 2) {
            Wr_FilFwd (hmmio, (LPSTR) &ucPadByt, 1, FIOENCNON, &uiErrCod);
            ulCurPos++;
        }
        if (lpck->cksize != (ulCurPos - lpck->dwDataOffset)) {
            lpck->cksize = ulCurPos - lpck->dwDataOffset;
            if (-1L == mmioSeek (hmmio, lpck->dwDataOffset - CHKSIZLEN, SEEK_SET)) 
                return (MMIOERR_CANNOTSEEK);
            Wr_FilFwd (hmmio, (LPSTR) &lpck->cksize, CHKSIZLEN, FIOENCNON, &uiErrCod);
        }
    }

    /********************************************************************/
    /********************************************************************/
    if (-1L == mmioSeek (hmmio, lpck->dwDataOffset + lpck->cksize, SEEK_SET)) 
        return (MMIOERR_CANNOTSEEK);

    /********************************************************************/
    /********************************************************************/
    return (0);
}

#endif /*****************************************************************/


