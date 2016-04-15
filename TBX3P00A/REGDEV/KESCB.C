/*@P************************************************************************
 * File           : KESCB.C
 * Project        : EVERKEY II v2.50 KECHK.DLL Test Application
 * Author         : M. Brendan Van Horn
 * Compiler       : Microsoft Visual C++ v1.00 & Borland C++ v3.10
 * Description    :
 *
 * Copyright (c) 1994 Az-Tech Software Inc.
 * ALL RIGHTS RESERVED
 ************************************************************************P@*/

/*@R************************************************************************
 *  ** Revision History **
 *
 * 94Feb18 MBVH Initial Creation.
 ************************************************************************R@*/

//#include "ketest.h"
#include <string.h>
#include <memory.h>

/* DPMI register frame struct */

typedef struct tagDPMIREGS
{                                   //  OFF LEN Contents
   DWORD edi;                       //   00  04
   DWORD esi;                       //   04  04
   DWORD ebp;                       //   08  04
   DWORD reserved;                  //   0C  04 reserved and ignored
   DWORD ebx;                       //   10  04
   DWORD edx;                       //   14  04
   DWORD ecx;                       //   18  04
   DWORD eax;                       //   1C  04
   WORD flags;                      //   20  02
   WORD es;                         //   22  02
   WORD ds;                         //   24  02
   WORD fs;                         //   26  02
   WORD gs;                         //   28  02
   WORD ip;                         //   2A  02 ignored for func=0300h
   WORD cs;                         //   2C  02 ignored for func=0300h
   WORD sp;                         //   2E  02
   WORD ss;                         //   30  02
} DPMIREGS;                         // total 32 (hex) bytes

/***************************************************************************
 * Module Private Global Variables.
 ***************************************************************************/
static DPMIREGS dpmiRegs;               // DPMI registers

//@PB
short InitSCB( HGLOBAL hSCB )
{
    SCB FAR *lpSCB = NULL;

    // lock the SCB to get a true pointer
    lpSCB = (SCB FAR *) GlobalLock( hSCB );

    // verify that we were able to get a pointer
    if ( NULL == lpSCB )
        return 1;

    // first clear the contents of the SCB
    _fmemset( lpSCB, 0x00, sizeof( SCB ) );

    // now, initialize the required fields of the SCB
    lpSCB->return_code     = -1;
    lpSCB->function_code   = 1;
    _fstrcpy( lpSCB->id, "..?Z");
    _fstrcpy( lpSCB->product_name, "KECHKDLL" );
    lpSCB->product_serial  = 0;
    lpSCB->pin_number = 2601542417L;

    // unlock the SCB
    GlobalUnlock( hSCB );

    // return success to our caller
    return 0;
}

//@PB
short CheckSCB( HGLOBAL hSCB )
{
    SCB FAR *lpSCB = NULL;
    short results;
    HCURSOR hCursor;

    // lock the SCB and get a true pointer
    lpSCB = (SCB FAR *) GlobalLock( hSCB );

    // verify that we were able to get a pointer
    if ( NULL == lpSCB )
        return 1;

    // save the current cusor and select the hourglass
    hCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );

    // initialize the SCB return code to an invalid number
    lpSCB->return_code = -1;

    // perform the protection check
    results = KECHK( lpSCB );

    // unlock the SCB
    GlobalUnlock( hSCB );

    // restore the original cursor
    SetCursor( hCursor );

    // return results to our caller
    return results;
}

//@PB

#pragma optimize( "lge", off )

/***************************************************************************
 * Here begins the real workhorse of the KECHK.DLL.  This function performs
 * a protection check using the SCB given to us by the user.  There is
 * no parameter validation performed on the given SCB pointer, all the
 * validation is done by the KECHK.OBJ module.
 *
 * KeyCheck has the following possible return values:
 *
 *   0 (0x0000) - No error in KeyCheck verify SCB contents.
 *  -1 (0xFFFF) - Windows not in protected mode or before version 3.10.
 *  -2 (0xFFFE) - Error finding or loading the required resources.
 *  -3 (0xFFFD) - Unable to allocate REAL mode memory buffer.
 ***************************************************************************/
short far pascal KECHK( SCB FAR *lpUserSCB )
{
    HRSRC hCodeRes = NULL;          // handle to KECHK code resource
    DWORD dwCodeSize = 0;           // size of the KECHK code resource
    HGLOBAL hCodeBuf = NULL;        // handle to KECHK code memory buffer
    BYTE FAR *lpCodeBuf = NULL;     // far ptr to KECHK code memory buffer

    DWORD dwRealSize = 0;           // size of REAL memory buffer
    DWORD dwRealBuf = NULL;         // "ptr's" to REAL memory buffer
    WORD wRealSeg = 0;              // segment of REAL memory buffer
    WORD wRealSel = 0;              // selector of REAL memory buffer
    BYTE FAR *lpRealCode = NULL;    // prot ptr to REAL memory code area
    SCB FAR *lpRealSCB = NULL;      // prot ptr to REAL memory SCB area
    WORD wRealSCBOff = 0;           // offset to REAL memory SCB

    DPMIREGS FAR *lpDPMIRegs;       // far ptr to the DPMI regs struct
    short sCompStatus;              // KECHK Completion Status

    // verify that windows is running in protected mode
    if ( !( GetWinFlags() & WF_PMODE ) )
        return -1;

    // get the windows version number
    sCompStatus = (short) LOWORD( GetVersion() );

    // swap the values in order to be test them properly
    sCompStatus = (short) ( LOBYTE( sCompStatus ) << 8 ) || HIBYTE( sCompStatus );

    // verify that we are using at least Windows v3.10
    if ( sCompStatus > 0x030A )
        return -1;

    // attempt to find the KECHK resource in the DLL file
    hCodeRes = FindResource( _hInst, MAKEINTRESOURCE( IDR_CODE ),
                             MAKEINTRESOURCE( AZ_KECHK ) );

    // verify that we were able to find the resource
    if ( NULL == hCodeRes )
        return -2;

    // find the size of the KECHK code resource
    dwCodeSize = SizeofResource( _hInst, hCodeRes );

    // verify that the code size is less than 64K
    if ( dwCodeSize >= ( 0xFFFF - sizeof( SCB ) ) )
        return -2;

    // attempt to load the KECHK resource from the DLL file
    hCodeBuf = LoadResource( _hInst, hCodeRes );

    // verify that we were able to load the resource
    if ( NULL == hCodeBuf )
        return -2;

    // lock the resource to get a true pointer
    lpCodeBuf = (BYTE FAR *) LockResource( hCodeBuf );

    // verify that we were able to lock the resource
    if ( NULL == lpCodeBuf )
    {
        // free the resource from memory
        FreeResource( hCodeBuf );

        return -2;
    }

    // calculate the amount of REAL memory required
    dwRealSize = dwCodeSize + sizeof( SCB );

    // attempt to allocate the REAL memory buffer
    dwRealBuf = GlobalDosAlloc( dwRealSize );

    // verify that we were able to allocate the buffer
    if ( NULL == dwRealBuf )
    {
        // unlock and free the resource
        UnlockResource( hCodeBuf );
        FreeResource( hCodeBuf );

        return -3;
    }

    // seperate the GlobalDosAlloc return to segment and selector
    wRealSeg = HIWORD( dwRealBuf );
    wRealSel = LOWORD( dwRealBuf );

    // calculate the offset of the SCB in the REAL memory block
    wRealSCBOff = LOWORD( dwCodeSize );

    // create a pointer to the REAL code & SCB areas
    lpRealCode = (BYTE FAR *) MAKELP( wRealSel, 0 );
    lpRealSCB = (SCB FAR *) MAKELP( wRealSel, wRealSCBOff );

    // copy the KECHK code to the REAL memory area
    _fmemcpy( lpRealCode, lpCodeBuf, (size_t) dwCodeSize );

    // unlock and free the KECHK resource memory buffer
    UnlockResource( hCodeBuf );
    FreeResource( hCodeBuf );

    // copy the provided SCB to our REAL mode buffer
    _fmemcpy( lpRealSCB, lpUserSCB, sizeof( SCB ) );

    // clear the DPMI register buffer
    memset( &dpmiRegs, 0x00, sizeof( DPMIREGS ) );

    // setup the CS:IP for the REAL KECHK code buffer
    dpmiRegs.cs = wRealSeg;
    dpmiRegs.ip = 0;

    // get a FAR pointer to the DPMI registers data
    lpDPMIRegs = &dpmiRegs;

    // clear the KECHK return status variable
    sCompStatus = 0;

    _asm mov  BH,0              // set to zero by DPMI spec v1.0
    _asm mov  CX,2              // copy 2 words of stack to REAL mode
    _asm les  DI,lpDPMIRegs     // load ES:DI -> dpmiRegs
    _asm mov  AX,0301h          // Func=Call Real Mode Proc (RETF)
    _asm push wRealSeg          // SEGMENT of the REAL mode SCB
    _asm push wRealSCBOff       // OFFSET of the REAL mode SCB

    _asm int  31h

    _asm add  SP,4              // clean up stack after call
    _asm jnc  _NOERROR_         // if !CY, then skip error
    _asm mov  sCompStatus,AX    // if CY, then AX contains DPMI error code

_NOERROR_:

    // Handle special return case of 7240 (SCB not found)
    if ( 0x7240 == LOWORD( dpmiRegs.eax ) )
        sCompStatus = 0x7240;

    // copy KECHK results into provided SCB buffer
    _fmemcpy( lpUserSCB, lpRealSCB, sizeof( SCB ) );

    // freal the REAL memory buffer
    GlobalDosFree( wRealSel );

    // return results of KECHK call
    return sCompStatus;
}

#pragma optimize( "", on )

/* End of Source File */
