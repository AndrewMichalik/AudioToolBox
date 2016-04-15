/************************************************************************/
/* Generic Indexed File Utility Supp Defs: IdxSup.h     V2.00  08/15/95 */
/* Copyright (c) 1987-1996 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#define USEERREND          1            /* User termination exit code   */
#define FORALLVER       TRUE            /* Function avail on all ver    */
#define IDXMAXSTR         40            /* Gen purpose string length    */

/************************************************************************/
/************************************************************************/
typedef enum _EXIMOD {
    EXIALLEXI =   0,                    /* always exit                  */
    EXIERRPAU =   1,                    /* succ & warn exit, err pause  */
    EXIWRNPAU =   2,                    /* succ exit, err & warn pause  */
    EXIALLPAU =   3,                    /* always pause                 */
} EXIMOD;

/************************************************************************/
/************************************************************************/
WORD    SysPrmIni (WORD, char **, EXIMOD *);
WORD    SysPrmTrm ();
