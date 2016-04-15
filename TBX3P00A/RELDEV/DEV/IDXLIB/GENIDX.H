/************************************************************************/
/* Generic Index File Utility Defs: GenIdx.h            V3.00  04/15/96 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define IDXVERNUM       0x0300          /* Idx support ver number       */

/************************************************************************/
/*      Define general purpose VISI data types for SDK users            */
/************************************************************************/
#if !(defined (FIOVERNUM))                              /* No FIO defs  */ 
    #define FIOMAXNAM       256         /* Max length of file name      */
#endif
#if !(defined (MEMVERNUM))                              /* No mem defs  */ 
    typedef unsigned long   VISMEMHDL;  /* VIS general purpose mem hdl  */
#endif

/************************************************************************/
/************************************************************************/
typedef struct _IDXBLK {                /* Global indexed ops structure */
    BYTE        ucRsv001[512];          /* Reserved                     */
} IDXBLK;

/************************************************************************/
/************************************************************************/
WORD    FAR PASCAL  IdxFilVer (void);
WORD    FAR PASCAL  IdxFilIni (WORD, DWORD, DWORD);
WORD    FAR PASCAL  IdxFilTrm ();

WORD    FAR PASCAL  IdxCfgIni (IDXBLK FAR *);
WORD    FAR PASCAL  IdxCfgLod (LPSTR, VISMEMHDL FAR *);
WORD    FAR PASCAL  IdxCfgRel (VISMEMHDL FAR *);
DWORD   FAR PASCAL  IdxCapQry (enum _PCMTYP, DWORD, DWORD);
                    
WORD    FAR PASCAL  IdxSetDeb (WORD);                   
LPVOID  FAR PASCAL  IdxSetDsp (LPVOID);                 
                    
/************************************************************************/
/************************************************************************/
typedef WORD  (FAR PASCAL *IDXPOLPRC) (LPVOID, DWORD, DWORD);
typedef WORD  (FAR cdecl  *IDXERRPRC) (LPCSTR, ...);

/************************************************************************/
/*                      Index Chop Definitions                          */
/************************************************************************/
typedef struct _CHIBLK {                /* Chop Index function block    */
    long        lNamOff;                /* Dest file name counter off   */
    DWORD       ulIdxPos;               /* Beginning index position     */
    DWORD       ulIdxCnt;               /* Index count                  */
    IDXPOLPRC   lpPolDsp;               /* Conversion poll display proc */
    DWORD       ulRsvPol;               /* Reserved                     */
    IDXERRPRC   lpMsgDsp;               /* Reserved                     */
    DWORD       ulRsvErr;               /* Reserved                     */
    BYTE        ucRsv001[32];           /* Reserved                     */
} CHIBLK;

typedef BOOL  (FAR PASCAL *CHICHKCBK) (LPSTR, DWORD);
typedef BOOL  (FAR PASCAL *CHIENDCBK) (DWORD, DWORD);

WORD    FAR PASCAL  ChpIdxIni (CHIBLK FAR *);
WORD    FAR PASCAL  ChpIdxFil (short, LPSTR, LPSTR, CHICHKCBK, LPVOID, CHIENDCBK, DWORD, CHIBLK FAR *);

/************************************************************************/
/*                      Index List Definitions                          */
/************************************************************************/
#define GENINHFLG   0x02                /* General info inhibit flag    */
#define VOXINHFLG   0x08                /* Vox     info inhibit flag    */
#define TXTINHFLG   0x20                /* Text    info inhibit flag    */

typedef struct _LSIBLK {                /* List Index function block    */
    WORD        usLstFlg;               /* List request flags           */
    IDXERRPRC   lpMsgDsp;               /* User message display proc    */
    DWORD       ulRsvErr;               /* Reserved                     */
    BYTE        ucRsv001[32];           /* Reserved                     */
} LSIBLK;

WORD    FAR PASCAL  LstIdxIni (LSIBLK FAR *);
DWORD   FAR PASCAL  LstIdxFil (short, LPCSTR, LPSTR, WORD, LSIBLK FAR *); 

/************************************************************************/
/*                      Index Pack Definitions                          */
/************************************************************************/
typedef struct _PKIBLK {                /* Pack Index function block    */
    short       sSrtTyp;                /* Sort order                   */ 
    DWORD       ulSmpFrq;               /* Header sample frequency      */
    IDXPOLPRC   lpPolDsp;               /* Conversion poll display proc */
    DWORD       ulRsvPol;               /* Reserved                     */
    IDXERRPRC   lpMsgDsp;               /* Reserved                     */
    DWORD       ulRsvErr;               /* Reserved                     */
    BYTE        ucRsv001[32];           /* Reserved                     */
} PKIBLK;

typedef BOOL  (FAR PASCAL *PKICHKCBK) (LPSTR, DWORD);
typedef BOOL  (FAR PASCAL *PKIENDCBK) (DWORD, DWORD);

WORD    FAR PASCAL  PakIdxIni (PKIBLK FAR *);
WORD    FAR PASCAL  PakIdxFil (LPSTR, LPSTR, LPSTR, PKICHKCBK, LPVOID, 
                    PKIENDCBK, DWORD, PKIBLK FAR *);

/************************************************************************/
/*                      Index Rebuild Definitions                       */
/************************************************************************/
#define MAXARRLEN     64                /* Replacement array length     */

typedef struct _RBIBLK {                /* Rebuild Index function block */
    short       sShfCnt;                /* Shift count                  */
    WORD        usAt_Pos;               /* Operation "at" position      */
    DWORD       ulSmpFrq;               /* Header sample frequency      */
    BYTE        ucRepVox[FIOMAXNAM];    /* Replacement vox file         */
    BYTE        ucRepTxt[FIOMAXNAM];    /* Replacement text file        */
    IDXPOLPRC   lpPolDsp;               /* Rebuild poll display proc    */
    DWORD       ulRsvPol;               /* Reserved                     */
    IDXERRPRC   lpMsgDsp;               /* Reserved                     */
    DWORD       ulRsvErr;               /* Reserved                     */
    BYTE        ucRsv001[32];           /* Reserved                     */
} RBIBLK;

WORD    FAR PASCAL  RebIdxIni (RBIBLK FAR *);
DWORD   FAR PASCAL  RebIdxFil (short, short, LPCSTR, LPCSTR, LPSTR, WORD, LPVOID); 

/************************************************************************/
/*                  Initialization Return Codes                         */
/************************************************************************/
#define SI_IDXNO_ERR    0           /* No error                         */
#define SI_IDXKEYERR    1           /* Key/License error, 8 segment lim */
#define SI_IDXVERERR    2           /* Module version mismatch          */


