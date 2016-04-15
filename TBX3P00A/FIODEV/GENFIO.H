/************************************************************************/
/* Generic File I/O Definitions: GenFIO.h               V2.00  07/15/94 */
/* Copyright (c) 1987-1992 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define FIOVERNUM       0x0212          /* File I/O support ver number  */

#define FIOMAXPTH       260             /* Max length of full pathname */
#define FIOMAXDRV         3             /* Max length of drive         */
#define FIOMAXDIR       256             /* Max length of path          */
#define FIOMAXNAM       256             /* Max length of file name     */
#define FIOMAXEXT       256             /* Max length of extension     */
#define FIOMAXDES        40             /* Max description text length */

/************************************************************************/
/*              Declarations for Windows KERNEL Functions               */
/************************************************************************/
#if !(defined (WM_NULL))                                /* No Win       */ 
    typedef short       LPOFSTRUCT;
    #define OFSTRUCT(x) BYTE x [136]
#elif !(defined (WF_ENHANCED))                          /* Win 2.x      */ 
#elif !(defined (WF_PAGING))                            /* Win 2 & 3.0  */ 
#elif (defined (WF_PAGING))                             /* Win 3.1 only */
#endif

/************************************************************************/
/************************************************************************/
#define ENUPCM      enum _PCMTYP        /* Forward reference for PCMTYP */

typedef enum _FILFMT {
    UNKFILFMT =   0,                    /* File format = Unknown        */
    ALLHDRFMT =   4,                    /* File format = Headered       */
    ALLPURFMT =   8,                    /* File format = Pure           */
    AIFHDRFMT =  12,                    /* File format = AIF            */
    BKTHDRFMT =  16,                    /* File format = Brooktrout     */
    PL2HDRFMT =  20,                    /* File format = PowerLine      */
    SU4HDRFMT =  24,                    /* File format = Summa Four     */
    VBSIDXFMT =  28,                    /* File format = Indexed VBase  */
    VISHDRFMT =  32,                    /* File format = VIS Header     */
    WAVHDRFMT =  36,                    /* File format = MM Wave        */
    MAXFILFMT                           /* Maximum # entries            */
} FILFMT;                  

/************************************************************************/
/*                  File I/O Operation return codes                     */
/************************************************************************/
typedef enum _FIOQRY {
    UNKFIOQRY =   0,                    /* Unknown query                */
    FIODEFQRY =   4,                    /* File format default          */
    PCMLCKQRY =   8,                    /* File PCM locked/unlocked     */
    CHNLCKQRY =  12,                    /* File PCM locked/unlocked     */
    FRQLCKQRY =  16,                    /* Chan locked/unlocked         */
} FIOQRY;                  

typedef enum _MASQRY {
    UNKITCQRY =   0,                    /* Unknown query                */
    MASITCQRY =   4,                    /* Masre ITC Block query        */
} MASQRY;                  

typedef enum _SEGQRY {
    SEGNULQRY =   0,                    /* No operation                 */
    SEGSELQRY =   4,                    /* Segment select               */
    SEGCREQRY =   8,                    /* Segment create               */
    SEGDELQRY =  12,                    /* Segment delete               */
    SEGFSTQRY =  16,                    /* Segment first                */
    SEGLSTQRY =  20,                    /* Segment last                 */
    SEGPRVQRY =  24,                    /* Segment previous             */
    SEGNXTQRY =  28,                    /* Segment next                 */
    SEGOFFQRY =  32,                    /* Segment offset               */
    SEGLENQRY =  36,                    /* Segment length               */
    SEGNUMQRY =  40,                    /* Segment number               */
} SEGQRY;                  

/************************************************************************/
/*                      Master File Record Structures                   */
/************************************************************************/
typedef struct  _MASINF {               /* Shell for new/exist/work     */
    OFSTRUCT   (ofLodUpd);              /* Load & update source OFS     */
    FILFMT      ffFilFmt;               /* File format                  */
    ENUPCM      usPCMTyp;               /* PCM type                     */
    WORD        usEncMsk;               /* Encoding mask                */
    WORD        usChnCnt;               /* Channel count                */
    DWORD       ulSmpFrq;               /* Sample freq                  */
    VISMEMHDL   mhHdrMem;               /* Header data global mem       */
    VISMEMHDL   mhIdxMem;               /* Index data global mem        */
    BYTE        ucRsv001[32 - 20];      /* Reserved                     */
} MASINF;

/************************************************************************/
/*                  Segmented File Record Structures                    */
/************************************************************************/
typedef struct  _PURSEG {               /* Pure (Raw) segment format    */
    DWORD       ulAudOff;               /* Audio data offset (0)        */
    DWORD       ulAudLen;               /* Audio data length (file len) */
    BYTE        ucRsv001[32 - 8];       /* Reserved                     */
} PURSEG;

typedef struct  _IDXSEG {               /* Indexed segment format       */
    WORD        usIdxNum;               /* Work File index number       */
    DWORD       ulVoxOff;               /* Original WrkFil vox offset   */
    DWORD       ulVoxLen;               /* Original WrkFil vox length   */
    DWORD       ulTxtOff;               /* Original WrkFil txt offset   */
    DWORD       ulTxtSiz;               /* Orig text length (with term) */
    BOOL        bfMaiCmp;               /* Maintain as compressed flag  */
    BYTE        ucRsv001[32 - 20];      /* Reserved                     */
} IDXSEG;

typedef struct  _WAVSEG {               /* Wave segment format          */
    DWORD       ulAudOff;               /* Audio data offset (if valid) */
    DWORD       ulAudLen;               /* Audio data length (if valid) */
    BYTE        ucRsv001[32 - 8];       /* Reserved                     */
} WAVSEG;

typedef struct  _SEGINF {               /* Segmented work file info blk */
    union   {
        PURSEG  rsPurSeg;               /* Pure (Raw) segment block     */  
        IDXSEG  isIdxSeg;               /* Indexed segment block        */  
        WAVSEG  wsWavSeg;               /* Wave segment block           */  
    };
    BYTE        ucRsv001[32];           /* Reserved                     */
} SEGINF;

/************************************************************************/
/*                  Callback procedure definitions                      */
/************************************************************************/
typedef enum _FIOPOL {
    FIOPOLBEG,                              /* Begin poll operation     */
    FIOPOLCNT,                              /* Continuation poll op     */
    FIOPOLPOS,                              /* Position poll notify     */
    FIOPOLEMP,                              /* Empty poll operation     */
    FIOPOLEND                               /* End poll operation       */
} FIOPOL;
typedef WORD (FAR PASCAL *FIOPOLPRC)(FIOPOL,DWORD,DWORD);

/************************************************************************/
/*                  File Operation return codes                         */
/*  The file operation code will tell the calling procedure whether     */
/*  the request was sucessful. If unsuccessful, will also tell the      */
/*  caller whether Dest File was "damaged" or remains untouched.        */
/************************************************************************/
typedef enum _FIORET {
    FIORETSUC = 0,                      /* File op successful           */
    FIORETCAN,                          /* File op canceled - no change */
    FIORETERR,                          /* File op error    - no change */
    FIORETBAD,                          /* File op err - bad transfer   */
} FIORET;

/************************************************************************/
/*              File I/O Resource Support Routines                      */
/************************************************************************/
WORD    FAR PASCAL  FIODLLIni (WORD, DWORD, DWORD);
WORD    FAR PASCAL  FIOSupVer (void);

/************************************************************************/
/*                  Hardware Specific Query Functions                   */
/************************************************************************/
FILFMT  FAR PASCAL  FIOOpnEnu (FILFMT);
FILFMT  FAR PASCAL  FIOEnuIXp (FILFMT);
LPSTR   FAR PASCAL  FIODesQry (FILFMT, LPSTR, WORD);
LPSTR   FAR PASCAL  FIOExtQry (FILFMT, LPSTR, WORD);
DWORD   FAR PASCAL  FIOCapQry (FIOQRY, DWORD, DWORD);

/************************************************************************/
/*          Master / Work / Snippet / Text Support Procedures           */
/************************************************************************/
FIORET  FAR PASCAL  FIOMasLod (LPOFSTRUCT, MASINF FAR *, FIOPOLPRC, DWORD);
FIORET  FAR PASCAL  FIOMasUpd (LPOFSTRUCT, MASINF FAR *, FIOPOLPRC, DWORD);
FIORET  FAR PASCAL  FIOMasRel (MASINF FAR *);
DWORD   FAR PASCAL  FIOMasQry (MASINF FAR *, WORD, LPVOID, DWORD);

FIORET  FAR PASCAL  FIOWrkLod (LPOFSTRUCT, MASINF FAR *, FIOPOLPRC, DWORD);
FIORET  FAR PASCAL  FIOWrkUpd (LPOFSTRUCT, MASINF FAR *, FIOPOLPRC, DWORD);
FIORET  FAR PASCAL  FIOWrkRel (MASINF FAR *);

FIORET  FAR PASCAL  FIOSegSel (SEGQRY, MASINF FAR *, const SEGINF FAR *, 
                    SEGINF FAR *);
FIORET  FAR PASCAL  FIOSegDes (SEGQRY, MASINF FAR *, const SEGINF FAR *, 
                    LPVOID, DWORD);
FIORET  FAR PASCAL  FIOSegCre (MASINF FAR *, SEGINF FAR *, SEGINF FAR *);
FIORET  FAR PASCAL  FIOSegDel (MASINF FAR *, SEGINF FAR *, SEGINF FAR *);

FIORET  FAR PASCAL  FIOSnpLod (LPOFSTRUCT, MASINF FAR *, SEGINF FAR *, 
                    FIOPOLPRC, DWORD);
FIORET  FAR PASCAL  FIOSnpUpd (LPOFSTRUCT, MASINF FAR *, SEGINF FAR *, 
                    FIOPOLPRC, DWORD);
FIORET  FAR PASCAL  FIOSnpRel (SEGINF FAR *);

FIORET  FAR PASCAL  FIOTxtLod (VISMEMHDL FAR *, MASINF FAR *, SEGINF FAR *, 
                    FIOPOLPRC, DWORD);
FIORET  FAR PASCAL  FIOTxtUpd (VISMEMHDL, MASINF FAR *, SEGINF FAR *, 
                    FIOPOLPRC, DWORD);
FIORET  FAR PASCAL  FIOTxtRel (VISMEMHDL FAR *);

/************************************************************************/
/*                      OFS File Operations                             */
/************************************************************************/
FIORET  FAR PASCAL FIOOFSApp (LPOFSTRUCT, LPOFSTRUCT, DWORD, DWORD, FIOPOLPRC, DWORD);
FIORET  FAR PASCAL FIOOFSDup (LPOFSTRUCT, LPOFSTRUCT, FIOPOLPRC, DWORD);
FIORET  FAR PASCAL FIOOFSShf (LPOFSTRUCT, DWORD, long, FIOPOLPRC, DWORD);
FIORET  FAR PASCAL FIOOFSRep (LPOFSTRUCT, DWORD FAR *, DWORD FAR *, 
        LPOFSTRUCT, DWORD, DWORD, LPOFSTRUCT, FIOPOLPRC, DWORD);
DWORD   FAR PASCAL FIOOFSLen (LPOFSTRUCT);

/************************************************************************/
/*                      File Direct Operations                          */
/************************************************************************/
long    FAR PASCAL FIOFilCop (short, short, DWORD, short FAR *, FIOPOLPRC, DWORD);
long    FAR PASCAL FIOFilDup (short, short, short FAR *, FIOPOLPRC, DWORD);
long    FAR PASCAL FIOFilMov (char *, char *, BOOL, short FAR *, FIOPOLPRC, DWORD);
long    FAR PASCAL FIOFilShf (short, short, DWORD, long, short FAR *, FIOPOLPRC, DWORD);
                              

