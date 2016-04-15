/************************************************************************/
/* MPC Conversion Support: MPCSup.c                     V2.00  07/15/95 */
/* Copyright (c) 1987-1995 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#include "genpcm.h"                     /* PCM/APCM conversion defs     */
#include "pcmsup.h"                     /* PCM/APCM xlate lib defs      */
#include "visdef.h"                     /* Standard type definitions    */

#include <string.h>                     /* String manipulation funcs    */

/************************************************************************/
/************************************************************************/
#define MPCMAX008       0x7f
#define MPCMIN008       0xff80
#define CVTP08TOG(P08,x)                                                    \
    _asm {                                                                  \
    _asm        test    P08, 0x80           /* Positive PCM (bit set)?  */  \
    _asm        jz      NegP08##x           /* No, negative             */  \
    _asm        and     P08, 0x7f           /* Toggle bit to zero       */  \
    _asm        jmp     pgDone##x                                           \
    }                                                                       \
    NegP08##x:                                                              \
    _asm {                                                                  \
    _asm        or      P08, 0xff80         /* Sign extend to 16 bit    */  \
    }                                                                       \
    pgDone##x:                                                                  

/************************************************************************/
/************************************************************************/
LPITCB FAR PASCAL MPCL16toIni (LPITCB lpITC)
{
    _fmemset (lpITC, 0, sizeof (*lpITC));
    return (lpITC);                     /* Return Init/Term/Cont block  */
}

WORD FAR PASCAL MPCL16toG16 (_segment _sBufSeg, BYTBASPTR pcSrcBuf,
                WORD FAR *lpusBytCnt, GENBASPTR psDstBuf, 
                WORD usRsv001, LPITCB lpITC)
{ 
    if (psDstBuf != (GENBASPTR) pcSrcBuf) 
        _fmemmove (_sBufSeg:>psDstBuf, _sBufSeg:>pcSrcBuf, *lpusBytCnt);
    /********************************************************************/
    return (*lpusBytCnt / 2);               /* Sample output count      */                            
}    

WORD FAR PASCAL MPCG16toL16 (_segment _sBufSeg, GENBASPTR psSrcBuf,
                WORD FAR *lpusSmpCnt, BYTBASPTR pcDstBuf, 
                WORD usRsv001, LPITCB lpITC)
{ 
    if (pcDstBuf != (BYTBASPTR) psSrcBuf) 
        _fmemmove (_sBufSeg:>pcDstBuf, _sBufSeg:>psSrcBuf, *lpusSmpCnt * sizeof (GENSMP));
    /********************************************************************/
    return (*lpusSmpCnt * 2);               /* Byte output count        */
}    

WORD FAR PASCAL MPCL16toM32 (_segment _sBufSeg, BYTBASPTR pcSrcBuf,
                WORD FAR *lpusRsv001, GENBASPTR psDstBuf, 
                WORD usOscCnt, DWORD ulSetSiz, LPITCB lpITC)
{
    WORD    usSetSiz;                   /* 64K limited set size         */
    WORD    usOscOrg = usOscCnt;        /* Original oscillation count   */
    TCMBLK  tbTCM = lpITC->tbTCM;       /* Init table coded I/F block   */

    /********************************************************************/
    /* Test for null oscillation count, set size range                  */
    /********************************************************************/
    if (0 == usOscCnt) return (0);
    usSetSiz = (WORD) min (ulSetSiz, 0xffffL);

    /********************************************************************/
    /* Since there is no guard calculation, set initial samp to current */
    /********************************************************************/
    tbTCM.gsCurWav = *(short FAR *)(_sBufSeg:>pcSrcBuf);

_asm {
            CPUSHA                      // Push c language regs

            //***********************************************************
            // Initialize registers, conversion parameters
            //***********************************************************
fmstrt:     mov     ax, _sBufSeg
            mov     ds, ax              // DS = Src/Dst buf segment
            mov     es, ax              // ES = DS
            cld                         // Clear direction flag (+)

            //***********************************************************
            // Initialize working registers
            //***********************************************************
            mov     ax, tbTCM.gsCurWav  // BX = Starting waveform value
            xor     ax, 0x80            // Convert BX to 8 bit unsigned
            mov     bx, ax              // Set init min PCM 
            mov     dx, ax              // Set init max PCM 
            mov     si, pcSrcBuf        // SI = &Source buffer
            mov     di, psDstBuf        // DI = &Destination buffer
  
            //***********************************************************
            // Test for null sample count (must be 2 or greater)
            //***********************************************************
chkset:     mov     cx, usSetSiz        // CX = # samples/ oscillation
            jz      pmdone              // Yes, exit

            //***********************************************************
            // Compute waveform value for 1st byte, test for min/max
            //***********************************************************
nxtsmp:     lodsw                       // AX = 16 bit sample
            cmp     ax, bx              // AX < current minimum?
            jg      tsthi1              // No, continue (signed)
            mov     bx, ax              // Yes, set new minimum
tsthi1:     cmp     ax, dx              // AL > current maximum?
            jl      decset              // No, continue (signed)
            mov     dx, ax              // Yes, set new maximum
decset:     loop    nxtsmp              // Continue with sample set?

            //***********************************************************
            // Clean up and end of sample set 
            //***********************************************************
endset:     xchg    ax, bx              // AX = Minimum PCM
                                        // BX = Current PCM 
            stosw                       // Set sample set minimum

            mov     ax, dx              // AX = Maximum PCM
            stosw                       // Set sample set maximum

            dec     usOscCnt            // Done with all oscillations?
            jz      setret              // Yes, set return values
            mov     dx, bx              // Set init max = min = current
            mov     cx, usSetSiz        // CX = # samples/ oscillation
            jmp     nxtsmp              // Continue with next byte
  
            //***********************************************************
            // End of oscillation sequence - set return values
            //***********************************************************
setret:     mov     tbTCM.gsCurWav, bx  // BX = Ending waveform value
  
            //***********************************************************
            //***********************************************************
pmdone:     CPOPA                       // Pop c language regs
  
}

    lpITC->tbTCM = tbTCM;               /* Reset ADPCM interface block  */
    return (usOscOrg);                  /* Oscillation output count     */
  
}

WORD FAR PASCAL MPCSiltoL16 (BYTE FAR *lpcDstBuf, WORD usBufSiz)
{
    _fmemset (lpcDstBuf, 0x00, usBufSiz);
    return (usBufSiz);
}

/************************************************************************/
/************************************************************************/
LPITCB FAR PASCAL MPCP08toIni (LPITCB lpITC)
{
    _fmemset (lpITC, 0, sizeof (*lpITC));
    return (lpITC);                     /* Return Init/Term/Cont block  */
}

WORD FAR PASCAL MPCP08toG16 (_segment _sBufSeg, BYTBASPTR pcSrcBuf,
                WORD FAR *lpusBytCnt, GENBASPTR psDstBuf, 
                WORD usRsv001, LPITCB lpITC)
{ 
    WORD        usi;
    short       gsCurWav;

    /********************************************************************/
    /* Check limits; convert to signed                                  */
    /********************************************************************/
    for (usi=0; usi<*lpusBytCnt; usi++) {
        gsCurWav = ((short) (char) (*_sBufSeg:>pcSrcBuf++ ^ (BYTE) 0x80));
        *_sBufSeg:>psDstBuf++ = gsCurWav; 
    }
    lpITC->tbTCM.gsCurWav = gsCurWav;

    /********************************************************************/
    /********************************************************************/
    return (*lpusBytCnt / 1);               /* Sample output count      */                            
}    

WORD FAR PASCAL MPCG16toP08 (_segment _sBufSeg, GENBASPTR psSrcBuf,
                WORD FAR *lpusSmpCnt, BYTBASPTR pcDstBuf, 
                WORD usRsv001, LPITCB lpITC)
{ 
    WORD        usi;
    short       gsCurWav;

    /********************************************************************/
    /* Check limits; convert to unsigned                                */
    /********************************************************************/
    for (usi=0; usi<*lpusSmpCnt; usi++) {
        gsCurWav = *_sBufSeg:>psSrcBuf++;
        if (gsCurWav > (short) MPCMAX008) gsCurWav = MPCMAX008; 
        else if (gsCurWav < (short) MPCMIN008) gsCurWav = MPCMIN008; 
        *_sBufSeg:>pcDstBuf++ = (BYTE) gsCurWav ^ (BYTE) 0x80;
    }
    lpITC->tbTCM.gsCurWav = gsCurWav;

    /********************************************************************/
    /********************************************************************/
    return (*lpusSmpCnt * 1);               /* Byte output count        */
}    

WORD FAR PASCAL MPCP08toM32 (_segment _sBufSeg, BYTBASPTR pcSrcBuf,
                WORD FAR *lpusRsv001, GENBASPTR psDstBuf, 
                WORD usOscCnt, DWORD ulSetSiz, LPITCB lpITC)
{
    WORD    usSetWrd;                   /* # smp words/osc (words/osc)  */
    WORD    usSetSiz;                   /* 64K limited set size         */
    WORD    usOscOrg = usOscCnt;        /* Original oscillation count   */
    TCMBLK  tbTCM = lpITC->tbTCM;       /* Init table coded I/F block   */

    /********************************************************************/
    /* Test for null oscillation count, set size range                  */
    /********************************************************************/
    if (0 == usOscCnt) return (0);
    usSetSiz = (WORD) min (ulSetSiz, 0xffffL); 

    /********************************************************************/
    /* Since there is no guard calculation, set initial samp to current */
    /********************************************************************/
    tbTCM.gsCurWav = (short) (((BYTE) *(_sBufSeg:>pcSrcBuf)) ^ (BYTE) 0x80);

_asm {
            CPUSHA                      // Push c language regs

            //***********************************************************
            // Initialize registers, conversion parameters
            //***********************************************************
fmstrt:     mov     ax, _sBufSeg
            mov     ds, ax              // DS = Src/Dst buf segment
            mov     es, ax              // ES = DS
            cld                         // Clear direction flag (+)

            //***********************************************************
            // Initialize working registers
            //***********************************************************
            mov     ax, tbTCM.gsCurWav  // AX = Starting waveform value
            xor     al, 0x80            // Convert AL to 8 bit unsigned
            mov     dl, al              // Set init min PCM 
            mov     dh, dl              // Set init max PCM 
            mov     si, pcSrcBuf        // SI = &Source buffer
            mov     di, psDstBuf        // DI = &Destination buffer
  
            //***********************************************************
            // Test for null sample count (must be 2 or greater)
            //***********************************************************
chkset:     mov     cx, usSetSiz        // CX = # samples/ oscillation
            shr     cx, 1               // CX = # smp pairs/osc (bytes/osc)
            mov     usSetWrd, cx        // usSetWrd = # words/osc
            cmp     usSetWrd, 0         // Null word request?
            jz      pmdone              // Yes, exit

            //***********************************************************
            // Compute waveform value for 1st byte, test for min/max
            //***********************************************************
fstbyt:     lodsw                       // AL = 1st PCM byte, AH = 2nd
            cmp     al, dl              // AL < current minimum?
            ja      tsthi1              // No, continue (unsigned)
            mov     dl, al              // Yes, set new minimum
tsthi1:     cmp     al, dh              // AL > current maximum?
            jb      secbyt              // No, continue (unsigned)
            mov     dh, al              // Yes, set new maximum

            //***********************************************************
            // Compute waveform value for 2nd byte, test for min/max
            //***********************************************************
            mov     al, ah              // AL = 2nd PCM byte
secbyt:     cmp     al, dl              // AL < current minimum?
            ja      tsthi2              // No, continue (unsigned)
            mov     dl, al              // Yes, set new minimum
tsthi2:     cmp     al, dh              // AL > current maximum?
            jb      decset              // No, continue (unsigned)
            mov     dh, al              // Yes, set new maximum

decset:     loop    fstbyt              // Continue with sample set?

            //***********************************************************
            // Check for last (odd) sample of at end of word sample set 
            //***********************************************************
lstbyt:     test    usSetSiz, 01h       // No, straggler in set?
            jz      endset              // No, finish set
            lodsb                       // AL = last PCM byte
            cmp     al, dl              // AL < current minimum?
            ja      tsthil              // No, continue (unsigned)
            mov     dl, al              // Yes, set new minimum
tsthil:     cmp     al, dh              // AL > current maximum?
            jb      endset              // No, finish set (unsigned)
            mov     dh, al              // Yes, set new maximum

            //***********************************************************
            // Clean up and end of sample set 
            //***********************************************************
endset:     xchg    al, dl              // AL = Minimum PCM
                                        // DL = Current PCM (ordered)
            CVTP08TOG(ax,1)             // AX = 16 bit 2's comp
            stosw                       // Set sample set minimum

            mov     al, dh              // AL = Maximum PCM
            CVTP08TOG(ax,2)             // AX = 16 bit 2's comp
            stosw                       // Set sample set maximum

            dec     usOscCnt            // Done with all oscillations?
            jz      setret              // Yes, set return values
            mov     dh, dl              // Set init max = min = current
            mov     cx, usSetWrd        // CX = # samples/ oscillation
            jmp     fstbyt              // Continue with next byte
  
            //***********************************************************
            // End of oscillation sequence - set return values
            //***********************************************************
setret:     mov     al, dl              // AL = Current PCM
            CVTP08TOG(ax,3)             // AX = 16 bit 2's comp
            mov     tbTCM.gsCurWav, ax  // AX = Ending waveform value
  
            //***********************************************************
            //***********************************************************
pmdone:     CPOPA                       // Pop c language regs
  
}

    lpITC->tbTCM = tbTCM;               /* Reset ADPCM interface block  */
    return (usOscOrg);                  /* Oscillation output count     */
  
}

WORD FAR PASCAL MPCSiltoP08 (BYTE FAR *lpcDstBuf, WORD usBufSiz)
{
    _fmemset (lpcDstBuf, 0x80, usBufSiz);
    return (usBufSiz);
}

/************************************************************************/
/************************************************************************/
WORD FAR PASCAL FLTF32toG16 (_segment _sBufSeg, BYTBASPTR pcSrcBuf,
                WORD FAR *lpusBytCnt, GENBASPTR psDstBuf, 
                WORD usRsv001, LPITCB lpITC)
{ 
// Note: PCMPtoG16 Assumes that PCM space requirements are > original
//    float   flSrcLoc;    
//    float   flDstMin = -0x7fff;
//    float   flDstMax = +0x7fff;
//    WORD    usi;
//
//    /********************************************************************/
//    /* Convert float to short; store short in destination buff          */
//    /********************************************************************/
//    for (usi=0; usi<usSmpCnt; usi++) {
//        flSrcLoc = *((float _based (_sBufSeg) *) pcSrcBuf++);        
//        if (flSrcLoc < flDstMin) *_sBufSeg:>psDstBuf++ = (short) flDstMin;
//          else if (flSrcLoc > flDstMax) *_sBufSeg:>psDstBuf++ = (short) flDstMax;
//            else *_sBufSeg:>psDstBuf++ = (short) flSrcLoc;
//    }
    return (*lpusBytCnt / 4);               /* Sample output count      */                            
}
WORD FAR PASCAL FLTG16toF32 (_segment _sBufSeg, GENBASPTR psSrcBuf,
                WORD FAR *lpusSmpCnt, BYTBASPTR pcDstBuf, 
                WORD usRsv001, LPITCB lpITC)
{ 
// Note: PCMG16toP Assumes that PCM space requirements are <= original. 
//    WORD        usi;
//    float FAR * lpTmpPtr = (LPVOID) _sBufSeg:>pcDstBuf;
//
//    /********************************************************************/
//    /* Convert short to float; store short in destination buff          */
//    /********************************************************************/
//    for (usi=0; usi<usSmpCnt; usi++) {
//        *lpTmpPtr++ = (float) *_sBufSeg:>psSrcBuf++;        
//    }
    return (*lpusSmpCnt * 4);               /* Byte output count        */
}


