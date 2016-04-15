/************************************************************************/
/* VBase Indexed File Format Defs: VBsFil.c             V2.00  10/15/95 */
/* Copyright (c) 1987-1994 Andrew J. Michalik                           */
/************************************************************************/
#define LPOFSTRUCT  short 

/************************************************************************/
/*              VBase File Header & Records Definitions                 */
/************************************************************************/
#define VBSMAXIDX   5000                /* Maximum allowed index count  */
#define VBSMAXFRQ 0xffffL               /* Maximum VBase sample freq    */
#define VBSDEFFRQ   6000L               /* Default VBase sample freq    */
#define VBSCNTDEF    100                /* Default VBase idx count      */
#define VBSTXTMAX  16384                /* Maximum text length          */

typedef struct _VBSHDR {                /* VBase record format          */
    DWORD   ulIdxTot;                   /* VBase File prompt count max  */
    DWORD   ulSmpFrq;                   /* Sample frequency             */
    DWORD   ulIdxUse;                   /* VBase File prompt count actv */
    DWORD   uldummya;                   /* Dummy                        */
    DWORD   ulBytUse;                   /* Bytes used (for Idx used)    */
    DWORD   uldummyb;                   /* Dummy                        */
} VBSHDR;

typedef struct _VBIREC {                /* VBase record format          */
    DWORD   ulVoxOff;                   /* Source File prompt offset    */
    DWORD   ulVoxLen;                   /* Source File prompt length    */
    DWORD   ulTxtOff;                   /* Snippet text offset          */
} VBIREC;

/************************************************************************/
/*              VBase File Manipulation Functions                       */
/************************************************************************/
WORD    FAR PASCAL  IniVBSHdr (VISMEMHDL FAR *, VISMEMHDL FAR *, WORD, DWORD);
FIORET  FAR PASCAL  LodVBSHdr (LPOFSTRUCT, VISMEMHDL FAR *, VISMEMHDL FAR *, WORD);
FIORET  FAR PASCAL  WrtVBSHdr (LPOFSTRUCT, struct _VBSHDR FAR *, 
                    struct _VBIREC FAR *, WORD);
WORD    FAR PASCAL  RelVBSHdr (VISMEMHDL, VISMEMHDL);

short               CmpVBSIdx (VBSHDR FAR *, VBIREC FAR *, DWORD, DWORD, short);
FIORET              ShfVBSFil (short, short, VBSHDR FAR *, VBIREC FAR *,
                    DWORD FAR *, DWORD FAR *, DWORD, FIOPOLPRC, DWORD);       
short               ShfIdxBlk (VBSHDR FAR *, VISMEMHDL FAR *, WORD, WORD, short);



