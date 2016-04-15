/************************************************************************/
/* Index Chopping Utility: IdxChp.c                     V2.10  10/15/95 */
/* Copyright (c) 1987-1991 Andrew J. Michalik                           */
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
#include "..\fiodev\filutl.h"           /* File utilities definitions   */
#include "idxlib\genidx.h"              /* Indexed file defs            */
#include "idxsup.h"                     /* Indexed File supp            */

#include <stdlib.h>                     /* Misc string/math/error funcs */
#include <string.h>                     /* String manipulation funcs    */
#include <stdio.h>                      /* Standard I/O                 */

/************************************************************************/
/*                  System Support Module Initialization                */
/************************************************************************/
WORD    SysPrmIni (WORD usArgCnt, char **ppcArgArr, EXIMOD *pemExiMod)
{
    char    szKeyFil[FIOMAXNAM] = {'\0'};
    char    szKeyStr[FIOMAXNAM] = {'\0'};
    char    szCurDir[FIOMAXNAM] = {'\0'};
    char    szAppDir[FIOMAXNAM] = {'\0'};
    WORD    usRetCod = 0;
    WORD    usi;

    /********************************************************************/
    /* Get current working directory (with trailing slash).             */
    /* Get application executable directory; remove program file name.  */
    /********************************************************************/
    xgetdcwd (szCurDir, FIOMAXNAM - 1);
    usi = _fstrlen (_fstrncpy (szAppDir, *ppcArgArr, FIOMAXNAM));
    while (usi--) if ('\\' == szAppDir[usi]) {
        szAppDir[usi+1] = '\0';        
        break;
    }    

    /********************************************************************/
    /* Set working directory to one containing the application; this    */
    /* insures that DLL's are found (if not in path).                   */
    /********************************************************************/
    xsetdrive (szAppDir[0], NULL);
    xchdir (szAppDir);

    /********************************************************************/
    /* Check the version numbers of support modules (Win DLL's)         */
    /********************************************************************/
    if (IDXVERNUM != IdxFilVer()) {
        printf ("Main software module version mismatch, exiting...\n");
        usRetCod = (WORD) -1;
    }

    /********************************************************************/
    /* Check for existence of interim license file                      */
    /********************************************************************/
    #define TBXLICFIL   "tbxlic.txt"
    if (!FilGetSta (TBXLICFIL, NULL)) strncpy (szKeyFil, TBXLICFIL, FIOMAXNAM);

    /********************************************************************/
    /* Scan for debug and licensing params; initialize debug status.    */
    /********************************************************************/
    while (usArgCnt--) {
        if (!strncmp (*ppcArgArr, "-debug", 6)) IdxSetDeb ((WORD) strtol (&(*ppcArgArr)[6], NULL, 16));
        else if (!strncmp (*ppcArgArr, "-exit", 5)) *pemExiMod = (EXIMOD) strtol (&(*ppcArgArr)[5], NULL, 16);
        else if (!strncmp (*ppcArgArr, "-kf", 3)) strncpy (szKeyFil, &(*ppcArgArr)[3], FIOMAXNAM); 
        else if (!strncmp (*ppcArgArr, "-ks", 3)) strncpy (szKeyStr, &(*ppcArgArr)[3], FIOMAXNAM); 
        *ppcArgArr++;
    }

    /********************************************************************/
    /* Initialize and allocate Index File resources                     */
    /* Note: Support services must be initialized for query functions   */
    /* Note: IdxFilIni() returns no errors in this release              */
    /********************************************************************/
    if (!usRetCod) 
    switch (IdxFilIni (0, (DWORD)(LPSTR) szKeyFil, (DWORD)(LPSTR) szKeyStr)) {
        case SI_IDXNO_ERR: 
            break;
        default:
            printf ("Unknown error, exiting...\n");
            usRetCod = (WORD) -1;
            break;
    }

    /********************************************************************/
    /* Re-set working directory to original directory.                  */
    /********************************************************************/
    xsetdrive (szCurDir[0], NULL);
    xchdir (szCurDir);

    /********************************************************************/
    /* Return zero if processing should continue, non-zero to abort     */
    /********************************************************************/
    return (usRetCod);

}

WORD    SysPrmTrm ()
{
    /********************************************************************/
    /* Free Index File resources                                        */
    /********************************************************************/
    IdxFilTrm ();
    return (0);
}
