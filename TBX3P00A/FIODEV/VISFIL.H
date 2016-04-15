/************************************************************************/
/* VIS InterChange File Definitions (DOS): VISFil.h     V2.00  07/15/94 */
/* Copyright (c) 1987-1994 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define SI_INVFILFMT   1                /* Invalid File Format          */
#define SI_BADOVRWRT   2                /* Cannot Overwrite File        */

/************************************************************************/
/************************************************************************/
typedef struct _VIXHDR {                /* VISI Interchange chunk hdr   */
    DWORD   ulChk_ID;                   /* Chunk ID             (ASCII) */
    DWORD   ulFmtVer;                   /* File format version  (ASCII) */
    DWORD   ulFmtTyp;                   /* File format type     (ASCII) */
    DWORD   ulChkSiz;                   /* Chunk size      (binary/hex) */
} VIXHDR;

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL VIXFndFst (short, LPSTR, DWORD, DWORD FAR *, WORD);
WORD FAR PASCAL VIXFndNxt (short, DWORD, WORD);
WORD FAR PASCAL VIXDesCur (short, DWORD FAR *, WORD);
WORD FAR PASCAL VIXLodTyp (short, enum _PCMTYP FAR *, WORD FAR *, DWORD FAR *, WORD);
WORD FAR PASCAL VIXWrtFst (short, LPSTR, DWORD, DWORD FAR *, WORD);
WORD FAR PASCAL VIXWrtTyp (short, enum _PCMTYP, WORD, DWORD, WORD);
WORD FAR PASCAL VIXWrtDat (short, DWORD FAR *, WORD);
WORD FAR PASCAL VIXAscCur (short, DWORD FAR *, WORD);

