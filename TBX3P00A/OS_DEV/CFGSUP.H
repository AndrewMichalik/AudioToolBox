/************************************************************************/
/* Configuration Param Config Support Defs: CfgSup.h    V2.00  01/10/93 */
/* Copyright (c) 1987-1993 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define CFGMAXSTR    128                /* Gen purpose string length    */
#define CFGMAXBUF  16384                /* Configuration buffer size    */

#define LF_CHR      0x0a                /* Line feed cfg file marker    */
#define CR_CHR      0x0d                /* Carriage return marker       */
#define EQ_CHR      0x3d                /* Equals sign marker           */
#define SB_CHR      0x5b                /* Section begin marker         */
#define SE_CHR      0x5d                /* Section end marker           */

/************************************************************************/
/************************************************************************/
typedef struct _CFGMAP {
    WORD    usNamLen;                   /* Configuration nam length     */
    LPSTR   szNamLst;                   /* Configuration name list      */
    WORD    usCfgVal[];                 /* Configuration value list     */
} CFGMAP;

/************************************************************************/
/************************************************************************/
WORD    FAR PASCAL  CfgPrmMap (CFGMAP FAR *, LPSTR, WORD FAR *);
WORD    FAR PASCAL  CfgFilLod (LPSTR, LPSTR, WORD);
long    FAR PASCAL  CfgLngGet (VISMEMHDL, LPSTR, LPSTR, long);
float   FAR PASCAL  CfgFltGet (VISMEMHDL, LPSTR, LPSTR, float);
LPSTR   FAR PASCAL  CfgStrGet (VISMEMHDL, LPSTR, LPSTR, LPSTR, LPSTR, WORD);

