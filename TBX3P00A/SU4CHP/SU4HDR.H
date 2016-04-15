/************************************************************************/
/* Summa Four Header Defs: Su4Hdr.c                     V1.00  09/10/92 */
/* Copyright (c) 1987-1993 Andrew J. Michalik                           */
/* Portions Copyright (c) 1993 Summa Four, Inc.  All rights reserved    */
/*                                                                      */
/************************************************************************/
#define SU4HDRFMT   0x53756d34L
#define SU4HDRVER   0x00000001L
#define SU4DEFFRQ   8000
#define SU4DESLEN   32

typedef struct _SU4HDR {
    DWORD   ulHdrFmt;
    DWORD   ulHdrSiz;
    DWORD   ulHdrVer;
    DWORD   ulFil_ID;
    DWORD   ulLibCod;
    DWORD   ulEncFmt;
    DWORD   ulSmpFrq;
    DWORD   ulRsv001;
    BYTE    ucFilDes[SU4DESLEN];
} SU4HDR;

typedef enum _SU4PCM {
    SU4PCMU08 = 1,
    SU4PCMA08 = 2,
} SU4PCM;


