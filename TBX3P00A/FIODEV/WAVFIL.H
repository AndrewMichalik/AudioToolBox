/************************************************************************/
/* Wave File Definitions (DOS): WavFil.h                V2.00  10/15/96 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define WIOMAXNAM      256              /* Max length of file name      */
#define CHKNAMLEN       4L
#define CHKSIZLEN       4L

/************************************************************************/
/*              Declarations for Windows KERNEL Functions               */
/************************************************************************/
#if !(defined (WM_NULL))                                /* No Win       */ 
    typedef UINT        HTASK;
    typedef signed long LONG;
#elif !(defined (WF_ENHANCED))                          /* Win 2.x      */ 
#elif !(defined (WF_PAGING))                            /* Win 2 & 3.0  */ 
#elif (defined (WF_PAGING))                             /* Win 3.1 only */
#endif

/************************************************************************/
/************************************************************************/
typedef UINT            HMMIO;
typedef unsigned long   FOURCC;         /* a four character code        */
typedef char _huge    * HPSTR;          /* a huge version of LPSTR      */
typedef void far      * LPMMIOPROC;

/************************************************************************/
/************************************************************************/
typedef struct _MMIOINFO
{
        /* general fields */
        DWORD           dwFlags;        /* general status flags */
        FOURCC          fccIOProc;      /* pointer to I/O procedure */
        LPMMIOPROC      pIOProc;        /* pointer to I/O procedure */
        UINT            wErrorRet;      /* place for error to be returned */
        HTASK           htask;          /* alternate local task */

        /* fields maintained by MMIO functions during buffered I/O */
        LONG            cchBuffer;      /* size of I/O buffer (or 0L) */
        HPSTR           pchBuffer;      /* start of I/O buffer (or NULL) */
        HPSTR           pchNext;        /* pointer to next byte to read/write */
        HPSTR           pchEndRead;     /* pointer to last valid byte to read */
        HPSTR           pchEndWrite;    /* pointer to last byte to write */
        LONG            lBufOffset;     /* disk offset of start of buffer */

        /* fields maintained by I/O procedure */
        LONG            lDiskOffset;    /* disk offset of next read or write */
        DWORD           adwInfo[3];     /* data specific to type of MMIOPROC */

        /* other fields maintained by MMIO */
        DWORD           dwReserved1;    /* reserved for MMIO use */
        DWORD           dwReserved2;    /* reserved for MMIO use */
        HMMIO           hmmio;          /* handle to open file */
} MMIOINFO;

typedef struct _MMCKINFO
{
        FOURCC          ckid;           /* chunk ID */
        DWORD           cksize;         /* chunk size */
        FOURCC          fccType;        /* form type or list type */
        DWORD           dwDataOffset;   /* offset of data portion of chunk */
        DWORD           dwFlags;        /* flags used by MMIO functions */
} MMCKINFO;

#ifndef _INC_MMREG
#define _INC_MMREG      /* #defined if mmreg.h has been included */

/* general waveform format structure (information common to all formats) */
typedef struct pcmwaveformat_tag {
    WORD    wFormatTag;        /* format type                           */
    WORD    nChannels;         /* #channels (i.e. mono, stereo, etc.)   */
    DWORD   nSamplesPerSec;    /* sample rate                           */
    DWORD   nAvgBytesPerSec;   /* for buffer estimation                 */
    WORD    nBlockAlign;       /* block size of data                    */
    WORD    wBitsPerSample;    /* sample size                           */
} PCMWAVEFORMAT;
typedef struct waveformat_extended_tag {
    WORD    wFormatTag;        /* format type                           */
    WORD    nChannels;         /* #channels (i.e. mono, stereo, etc.)   */
    DWORD   nSamplesPerSec;    /* sample rate                           */
    DWORD   nAvgBytesPerSec;   /* for buffer estimation                 */
    WORD    nBlockAlign;       /* block size of data                    */
    WORD    wBitsPerSample;    /* sample size                           */
    WORD    cbSize;            /* #bytes of the size of extra info (after cbSize)   */
} WAVEFORMATEX;
typedef struct adpcmcoef_tag {
    short   iCoef1;
    short   iCoef2;
} ADPCMCOEFSET;
typedef struct adpcmwaveformat_tag {
    WAVEFORMATEX    wfx;
    WORD            wSamplesPerBlock;
    WORD            wNumCoef;
    ADPCMCOEFSET	aCoef[];
} ADPCMWAVEFORMAT;

typedef PCMWAVEFORMAT   FAR *LPPCMWAVEFORMAT;
typedef WAVEFORMATEX    FAR *LPWAVEFORMATEX;
typedef ADPCMWAVEFORMAT FAR *LPADPCMWAVEFORMAT;

#endif	/* _INC_MMREG */

/************************************************************************/
/************************************************************************/
/* MMIO macros */
#define mmioFOURCC( ch0, ch1, ch2, ch3 )                                \
                ( (DWORD)(BYTE)(ch0) | ( (DWORD)(BYTE)(ch1) << 8 ) |    \
                ( (DWORD)(BYTE)(ch2) << 16 ) | ( (DWORD)(BYTE)(ch3) << 24 ) )

/* standard four character codes */
#define FOURCC_RIFF     mmioFOURCC('R', 'I', 'F', 'F')
#define FOURCC_LIST     mmioFOURCC('L', 'I', 'S', 'T')

/************************************************************************/
/************************************************************************/
/* constants for dwFlags field of MMIOINFO */
#define MMIO_CREATE     0x00001000      /* create new file (or truncate file) */
#define MMIO_PARSE      0x00000100      /* parse new file returning path */
#define MMIO_DELETE     0x00000200      /* create new file (or truncate file) */
#define MMIO_EXIST      0x00004000      /* checks for existence of file */
#define MMIO_ALLOCBUF   0x00010000      /* mmioOpen() should allocate a buffer */
#define MMIO_GETTEMP    0x00020000      /* mmioOpen() should retrieve temp name */
#define MMIO_DIRTY      0x10000000      /* I/O buffer is dirty */


/* read/write mode numbers (bit field MMIO_RWMODE) */
#define MMIO_READ       0x00000000      /* open file for reading only */
#define MMIO_WRITE      0x00000001      /* open file for writing only */
#define MMIO_READWRITE  0x00000002      /* open file for reading and writing */

/* various MMIO flags */
#define MMIO_FINDCHUNK          0x0010  /* mmioDescend: find a chunk by ID */
#define MMIO_FINDRIFF           0x0020  /* mmioDescend: find a LIST chunk */
#define MMIO_CREATERIFF         0x0020  /* mmioCreateChunk: make a LIST chunk */

/* MMIO error return values */
#define MMIOERR_BASE            256
#define MMIOERR_FILENOTFOUND    (MMIOERR_BASE + 1)  /* file not found */
#define MMIOERR_OUTOFMEMORY     (MMIOERR_BASE + 2)  /* out of memory */
#define MMIOERR_CANNOTOPEN      (MMIOERR_BASE + 3)  /* cannot open */
#define MMIOERR_CANNOTCLOSE     (MMIOERR_BASE + 4)  /* cannot close */
#define MMIOERR_CANNOTREAD      (MMIOERR_BASE + 5)  /* cannot read */
#define MMIOERR_CANNOTWRITE     (MMIOERR_BASE + 6)  /* cannot write */
#define MMIOERR_CANNOTSEEK      (MMIOERR_BASE + 7)  /* cannot seek */
#define MMIOERR_CANNOTEXPAND    (MMIOERR_BASE + 8)  /* cannot expand file */
#define MMIOERR_CHUNKNOTFOUND   (MMIOERR_BASE + 9)  /* chunk not found */
#define MMIOERR_UNBUFFERED      (MMIOERR_BASE + 10) /* file is unbuffered */

#define WAVE_FORMAT_PCM                 0x0001
#define WAVE_FORMAT_ADPCM               0x0002
#define WAVE_FORMAT_IBM_CVSD            0x0005
#define WAVE_FORMAT_ALAW                0x0006
#define WAVE_FORMAT_MULAW               0x0007
#define WAVE_FORMAT_OKI_ADPCM           0x0010
#define WAVE_FORMAT_DIGISTD             0x0015
#define WAVE_FORMAT_DIGIFIX             0x0016
#define WAVE_FORMAT_DIALOGIC_OKI_ADPCM	0x0017  /* Dialogic Corporation */

/************************************************************************/
/************************************************************************/
WORD    PASCAL FAR WIOFndNxt (HMMIO, enum _PCMTYP FAR *, VISMEMHDL FAR *, MMCKINFO FAR *);
WORD    PASCAL FAR WIOOutIni (HMMIO, enum _PCMTYP, WORD, DWORD, VISMEMHDL FAR *, MMCKINFO FAR *);
DWORD   PASCAL FAR WIOFilCop (HMMIO hSrcHdl, HMMIO hDstHdl, DWORD ulReqCnt,
                   FIOPOLPRC fpLngPolPrc, DWORD ulPolDat);
WORD    PASCAL FAR WIOOutEnd (HMMIO, VISMEMHDL FAR *, MMCKINFO FAR *);

/************************************************************************/
/************************************************************************/
HMMIO PASCAL FAR mmioOpen (LPSTR szFileName, MMIOINFO FAR* lpmmioinfo,
                 DWORD dwOpenFlags);
UINT  PASCAL FAR mmioClose (HMMIO hmmio, UINT uFlags);
LONG  PASCAL FAR mmioSeek(HMMIO hmmio, LONG lOffset, int iOrigin);
LONG  PASCAL FAR mmioRead (HMMIO hmmio, HPSTR pch, LONG cch);
LONG  PASCAL FAR mmioWrite (HMMIO hmmio, const char _huge* pch, LONG cch);
UINT  PASCAL FAR mmioCreateChunk (HMMIO hmmio, MMCKINFO FAR* lpck, UINT uFlags);
UINT  PASCAL FAR mmioDescend (HMMIO hmmio, MMCKINFO FAR* lpck, 
                 const MMCKINFO FAR* lpckParent, UINT uFlags);
UINT  PASCAL FAR mmioAscend (HMMIO hmmio, MMCKINFO FAR* lpck, UINT uFlags);

