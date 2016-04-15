/************************************************************************/
/* File I/O Low Level Support: FilLow.c                 V2.00  07/15/94 */
/* Copyright (c) 1987-1994 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "visdef.h"                     /* VIS Standard type defs       */
#include "filutl.h"                     /* File utilities definitions   */
#include "bitrev.tab"                   /* Bit reverse tables           */

#include <string.h>                     /* String manipulation funcs    */
#include <stdio.h>                      /* Standard I/O                 */
#include <io.h>                         /* Low level file I/O           */
#include <fcntl.h>                      /* Flags used in open/ sopen    */
#include <errno.h>                      /* errno value constants        */
#include <sys\stat.h>                   /* File status types            */

/************************************************************************/
/************************************************************************/
#if (defined (W31)) /****************************************************/
    int   FAR PASCAL _lclose (int);
    int   FAR PASCAL _lcreat (LPSTR, int);
    long  FAR PASCAL _llseek (int, long, int);
    int   FAR PASCAL _lopen  (LPSTR, int);
#endif /*****************************************************************/
#if (defined (DOS)) /****************************************************/
    #define _lclose         _close
    #define _llseek         _lseek
#endif /*****************************************************************/

/************************************************************************/
/*                  Bit/Nibble/Byte/Word Ordering Macros                */
/************************************************************************/
#define N__SWP(b)   ((BYTE)((((BYTE)((b)&0xff)) >> 4) | (((BYTE)((b)&0xff)) << 4)))
#define W__SWP(w)   ((((WORD)((w)&0xffff)) >> 8) | (((WORD)((w)&0xffff)) << 8))
#define DW_SWP(dw)  (((DWORD)W__SWP(((DWORD)(dw)) >> 16)) | (((DWORD)W__SWP((DWORD)(dw))) << 16))

/************************************************************************/
/*                      Forward References                              */
/************************************************************************/
unsigned int EncFilDat (DWORD, WORD, char far *, WORD);
unsigned int DecFilDat (DWORD, WORD, char far *, WORD);
unsigned int RndFilDat (unsigned int, WORD);

/************************************************************************/
/*                  Low-level Open/Close/Length routines                */
/************************************************************************/
short FAR PASCAL FilHdlOpn (const char far *lpFilNam, unsigned short usOpnFlg)
{
    static  char    szLocNam[FIOMAXNAM];

    /********************************************************************/
    /********************************************************************/
    _fstrncpy (szLocNam, lpFilNam, FIOMAXNAM);

#if (defined (W31)) /****************************************************/
    /********************************************************************/
    /* _lopen does not have O_BINARY option                             */
    /********************************************************************/
    usOpnFlg &=  ~O_BINARY;
    /********************************************************************/
    /* _lopen does not have O_CREAT & _O_TRUNC option                   */
    /********************************************************************/
    if ((O_TRUNC & usOpnFlg) || ((O_CREAT & usOpnFlg) && FilGetSta (szLocNam, NULL))) {
        return (FilHdlCre (szLocNam, usOpnFlg, S_IWRITE));
    }
    return (_lopen (szLocNam, usOpnFlg));
#endif /*****************************************************************/

#if (defined (DOS)) /****************************************************/
    return (_open (szLocNam, usOpnFlg));
#endif /*****************************************************************/

}

short FAR PASCAL FilHdlCre (const char far *lpFilNam, unsigned short usOpnFlg, unsigned short usModFlg)
{
    static  char    szLocNam[FIOMAXNAM];
    short           sFilHdl;

    /********************************************************************/
    /********************************************************************/
    _fstrncpy (szLocNam, lpFilNam, FIOMAXNAM);

#if (defined (W31)) /****************************************************/
    if (-1 == (sFilHdl = _lcreat (szLocNam, (S_IWRITE & usModFlg) ? 0 : 1)))
        return (-1);
    FilHdlCls (sFilHdl);
    return (FilHdlOpn (szLocNam, usOpnFlg & ~(O_TRUNC | O_CREAT)));
#endif /*****************************************************************/

#if (defined (DOS)) /****************************************************/
    return (_open (szLocNam, usOpnFlg | O_CREAT | O_TRUNC, usModFlg));
#endif /*****************************************************************/
}

short FAR PASCAL FilHdlCls (short sFilHdl)
{
    return (_lclose (sFilHdl));
}

long FAR PASCAL FilGetPos (short sFilHdl)
{
    return (_tell (sFilHdl));
}

long FAR PASCAL FilSetPos (short sFilHdl, long ulNewPos, unsigned short usPosFlg)
{
    return (_llseek (sFilHdl, ulNewPos, usPosFlg));
}

long FAR PASCAL FilGetLen (short sFilHdl)
{
    return (_filelength (sFilHdl));
}

short FAR PASCAL FilGetSta (const char far *lpFilNam, struct _stat far *lpFilSta)
{
    static  char        szLocNam[FIOMAXNAM];
    static struct _stat ssStaBlk;
    unsigned int        uiErrCod;

    /********************************************************************/
    /********************************************************************/
    _fstrncpy (szLocNam, lpFilNam, FIOMAXNAM);
    uiErrCod = _stat (szLocNam, &ssStaBlk);
    if (NULL != lpFilSta) *lpFilSta = ssStaBlk;

    /********************************************************************/
    /********************************************************************/
    return (uiErrCod);

}

//_stat() fails when in a Windows DLL (MS C7)
//short FAR PASCAL FilGetSta (const char far *lpFilNam, struct _stat far *lpFilSta)
//{
//    short   sFilHdl;
//
//    /********************************************************************/
//    /********************************************************************/
//    if (-1 == (sFilHdl = FilHdlOpn (lpFilNam, O_BINARY | O_RDONLY))) return (-1);
//    if (NULL != lpFilSta) lpFilSta->st_size = FilGetLen (sFilHdl);
//    FilHdlCls (sFilHdl);
//
//    /********************************************************************/
//    /********************************************************************/
//    return (0);
//}

/************************************************************************/
/*                      Low-level I/O routines                          */
/************************************************************************/
unsigned int Rd_FilRev (int iFilHdl, void far *lpFilBuf, unsigned int uiBytCnt, 
    unsigned int uiEncFmt, unsigned int far *lpErrCod)
{
    DWORD   ulCurPos = FilGetPos (iFilHdl);

    /********************************************************************/
    /* Read count bytes preceding (not including) current position      */
    /********************************************************************/
    uiBytCnt = ((DWORD) uiBytCnt <= ulCurPos) ? uiBytCnt : (unsigned int) ulCurPos;
    if (uiBytCnt == 0) return (0);

    /********************************************************************/
    /********************************************************************/
    if (-1L == FilSetPos (iFilHdl, ulCurPos - (DWORD) uiBytCnt, SEEK_SET)) 
        return ((unsigned int) -1);
    uiBytCnt = Rd_FilFwd (iFilHdl, lpFilBuf, uiBytCnt, uiEncFmt, lpErrCod);

    FilSetPos (iFilHdl, ulCurPos - (DWORD) uiBytCnt, SEEK_SET);
    return (uiBytCnt);
}

unsigned int Wr_FilRev (int iFilHdl, void far *lpFilBuf, unsigned int uiBytCnt, 
    unsigned int uiEncFmt, unsigned int far *lpErrCod)
{
    DWORD   ulCurPos = FilGetPos (iFilHdl);

    /********************************************************************/
    /* Write count bytes preceding (not including) current position     */
    /********************************************************************/
    uiBytCnt = ((DWORD) uiBytCnt <= ulCurPos) ? uiBytCnt : (unsigned int) ulCurPos;
    if (uiBytCnt == 0) return (0);

    /********************************************************************/
    /********************************************************************/
    if (-1L == FilSetPos (iFilHdl, ulCurPos - (DWORD) uiBytCnt, SEEK_SET)) 
        return ((unsigned int) -1);

    if (-1 != (uiBytCnt = Wr_FilFwd (iFilHdl, lpFilBuf, uiBytCnt, uiEncFmt, lpErrCod))) 
        FilSetPos (iFilHdl, ulCurPos - (DWORD) uiBytCnt, SEEK_SET);

    return (uiBytCnt);

}  

/************************************************************************/
/************************************************************************/
unsigned int Rd_FilFwd (int iFilHdl, void far *lpFilBuf, unsigned int uiBytCnt, 
    unsigned int uiEncFmt, unsigned int far *lpErrCod)
{

    int iRetCod;

    /********************************************************************/
    /* Round input bytes to bit/nibble/byte/word swap size              */
    /********************************************************************/
    if (uiEncFmt & FIOENCSWP) uiBytCnt = RndFilDat (uiBytCnt, uiEncFmt);

    /********************************************************************/
    /********************************************************************/
    _asm {

            //***********************************************************
            // Initialize registers
            //***********************************************************
            CPUSHA                      // Push c language regs
            mov     bx, iFilHdl
            lds     dx, lpFilBuf
            mov     cx, uiBytCnt

            //***********************************************************
            // Request read operation
            //***********************************************************
            mov     ah, 3fh             // 3f = read function
            int     21h
            jc      rcferr              // Error indicated by CF?
            mov     cx, 0               // Yes, set cx = everything OK
            jmp     rexnor              // Exit normal

            //***********************************************************
            // Translate error conditions to "C" errno.h values
            //***********************************************************
    rcferr: cmp     ax, 5               // Access denied?
            jne     rhnerr              // No
            mov     cx, EACCES          // Indicate access denied
            jmp     rexerr              // Exit with err set

    rhnerr: cmp     ax, 6               // Invalid file handle?
            jne     runerr              // No
            mov     cx, EBADF           // Indicate invalid handle
            jmp     rexerr              // Exit with err set

    runerr: mov     cx, FIO_E_UNK       // Unknown error

            //***********************************************************
            // Exit routine; set byte count & return code
            //***********************************************************
    rexerr: mov     ax, -1              // Indicate error "-1" bytes
    rexnor: mov     uiBytCnt, ax        // ax = bytes read
            mov     iRetCod, cx         // iRetCod = cx  
            CPOPA                       // Pop c language regs
    }

    /********************************************************************/
    /********************************************************************/
    if ((-1 != uiBytCnt) && ((FIOENCINP | FIOENCSWP) & uiEncFmt)) 
        DecFilDat (0L, uiEncFmt, lpFilBuf, uiBytCnt);

    /********************************************************************/
    /********************************************************************/
    *lpErrCod = iRetCod;

    return (uiBytCnt); 

}


/************************************************************************/
/************************************************************************/
unsigned int Wr_FilFwd (int iFilHdl, void far *lpFilBuf, unsigned int uiBytCnt, 
    unsigned int uiEncFmt, unsigned int far *lpErrCod)
{

    int iRetCod;

    /********************************************************************/
    /* Allow size change only when buffer == NULL                       */
    /********************************************************************/
    if (0 == uiBytCnt) {
        if (NULL != lpFilBuf) return (0);
        lpFilBuf = (char far *) &iRetCod;
    }

    /********************************************************************/
    /* Round output bytes to bit/nibble/byte/word swap size             */
    /********************************************************************/
    if (uiEncFmt & FIOENCSWP) uiBytCnt = RndFilDat (uiBytCnt, uiEncFmt);

    /********************************************************************/
    /********************************************************************/
    if ((FIOENCOUT | FIOENCSWP) & uiEncFmt) 
        EncFilDat (0L, uiEncFmt, lpFilBuf, uiBytCnt);

    /********************************************************************/
    /********************************************************************/
    _asm {

            //***********************************************************
            // Initialize registers
            //***********************************************************
            CPUSHA                      // Push c language regs
            mov     bx, iFilHdl
            lds     dx, lpFilBuf
            mov     cx, uiBytCnt

            //***********************************************************
            // Request write operation
            //***********************************************************
            mov     ah, 40h             // 40 = write function
            int     21h
            jc      wcferr              // Error indicated by CF?
            cmp     cx, ax              // Correct uiBytCnt written?
            jne     wsperr              // No, truncated write            
            mov     cx, 0               // Yes, set cx = everything OK
            jmp     wexnor              // Exit normal

            //***********************************************************
            // Translate error conditions to "C" errno.h values
            //***********************************************************
    wcferr: cmp     ax, 5               // Access denied?
            jne     whnerr              // No
            mov     cx, EACCES          // Indicate access denied
            jmp     wexerr              // Exit with err set

    whnerr: cmp     ax, 6               // Invalid file handle?
            jne     wunerr              // No
            mov     cx, EBADF           // Indicate invalid handle
            jmp     wexerr              // Exit with err set

    wunerr: mov     cx, FIO_E_UNK       // Unknown error
            jmp     wexerr              // Exit with err set

    wsperr: mov     cx, ENOSPC          // Insufficient space

            //***********************************************************
            // Exit routine; set byte count & return code
            //***********************************************************
    wexerr: mov     ax, -1              // Indicate error "-1" bytes
    wexnor: mov     uiBytCnt, ax        // ax = bytes written
            mov     iRetCod, cx         // iRetCod = cx  
            CPOPA                       // Pop c language regs
    }

    /********************************************************************/
    /********************************************************************/
    *lpErrCod = iRetCod;

    return (uiBytCnt); 

}

unsigned int Wr_FilCom (int iFilHdl)
{

    int iRetCod;

    /********************************************************************/
    /********************************************************************/
    _asm {

            //***********************************************************
            // Initialize registers
            //***********************************************************
            CPUSHA                      // Push c language regs
            mov     bx, iFilHdl

            //***********************************************************
            // Request commit buffers operation
            //***********************************************************
            mov     ah, 68h             // 68 = commit function
            int     21h
            jc      ccferr              // Error indicated by CF?
            mov     cx, 0               // Yes, set cx = everything OK
            jmp     cexnor              // Exit normal

            //***********************************************************
            // Translate error conditions to "C" errno.h values
            //***********************************************************
    ccferr: cmp     ax, 5               // Access denied?
            jne     chnerr              // No
            mov     cx, EACCES          // Indicate access denied
            jmp     cexerr              // Exit with err set

    chnerr: cmp     ax, 6               // Invalid file handle?
            jne     cunerr              // No
            mov     cx, EBADF           // Indicate invalid handle
            jmp     cexerr              // Exit with err set

    cunerr: mov     cx, FIO_E_UNK       // Unknown error
            jmp     cexerr              // Exit with err set

            //***********************************************************
            // Exit routine; set byte count & return code
            //***********************************************************
    cexerr: 
    cexnor: mov     iRetCod, cx         // iRetCod = cx  
            CPOPA                       // Pop c language regs
    }

    /********************************************************************/
    /********************************************************************/
    return (iRetCod); 

}

/************************************************************************/
/************************************************************************/
BYTE    ucKeyArr[FIOKEYMSK+1] = {0, 'A', '1', 'B', '2', 'C', '3', 'D'};
void    SwpFilDat (WORD, char far *, WORD);

unsigned int EncFilDat (DWORD ulFilPos, WORD usEncKey, char far *pcDstBuf, WORD usBufSiz)
{
    BYTE    ucEncByt;

    /********************************************************************/
    /********************************************************************/
    if (usEncKey & FIOENCKEY) {
        ucEncByt = ucKeyArr[usEncKey & FIOKEYMSK];
        while (usBufSiz--) *pcDstBuf++ ^= ucEncByt;
    }
    else if (usEncKey & FIOENCSWP) {
        if (usEncKey & FIOSWPBIT) SwpFilDat (FIOSWPBIT, pcDstBuf, usBufSiz);
        if (usEncKey & FIOSWPNIB) SwpFilDat (FIOSWPNIB, pcDstBuf, usBufSiz);
        if (usEncKey & FIOSWPBYT) SwpFilDat (FIOSWPBYT, pcDstBuf, usBufSiz);
        if (usEncKey & FIOSWPWRD) SwpFilDat (FIOSWPWRD, pcDstBuf, usBufSiz);
    }

    /********************************************************************/
    /********************************************************************/
    return (0);
}

unsigned int DecFilDat (DWORD ulFilPos, WORD usEncKey, char far *pcDstBuf, WORD usBufSiz)
{
    BYTE        ucEncByt;

    /********************************************************************/
    /********************************************************************/
    if (usEncKey & FIOENCKEY) {
        ucEncByt = ucKeyArr[usEncKey & FIOKEYMSK];
        while (usBufSiz--) *pcDstBuf++ ^= ucEncByt;
    }
    else if (usEncKey & FIOENCSWP) {
        if (usEncKey & FIOSWPBIT) SwpFilDat (FIOSWPBIT, pcDstBuf, usBufSiz);
        if (usEncKey & FIOSWPNIB) SwpFilDat (FIOSWPNIB, pcDstBuf, usBufSiz);
        if (usEncKey & FIOSWPBYT) SwpFilDat (FIOSWPBYT, pcDstBuf, usBufSiz);
        if (usEncKey & FIOSWPWRD) SwpFilDat (FIOSWPWRD, pcDstBuf, usBufSiz);
    }

    /********************************************************************/
    /********************************************************************/
    return (0);
}

unsigned int RndFilDat (unsigned int uiBytCnt, WORD usEncKey)
{
    switch (usEncKey & FIOSWPMSK) {
        case FIOSWPBIT:
            return (uiBytCnt);
        case FIOSWPNIB:
            return (uiBytCnt);
        case FIOSWPBYT:
            return ((uiBytCnt / sizeof (WORD)) * sizeof (WORD)); 
        case FIOSWPWRD:
            return ((uiBytCnt / sizeof (DWORD)) * sizeof (DWORD)); 
    }
    return (uiBytCnt);
}

void    SwpFilDat (WORD usEncKey, char far *pcDstBuf, WORD usBufSiz)
{
    /********************************************************************/
    /********************************************************************/
    switch (usEncKey & FIOSWPMSK) {
        case FIOSWPBIT:
            while (usBufSiz--) *pcDstBuf++ = ucBitRev[*pcDstBuf];
            break;
        case FIOSWPNIB:
            while (usBufSiz--) *pcDstBuf++ = N__SWP (*pcDstBuf);
            break;
        case FIOSWPBYT:
            usBufSiz /= sizeof (WORD);
            while (usBufSiz--) *((WORD far *) pcDstBuf)++ = W__SWP (*((WORD far *) pcDstBuf));
            break;
        case FIOSWPWRD:
            usBufSiz /= sizeof (DWORD);
            while (usBufSiz--) *((DWORD far *) pcDstBuf)++ = DW_SWP (*((DWORD far *) pcDstBuf));
            break;
    }
}
