
//=======================================================================
//
//           Resource DLL for Example Installation
//
//        Copyright (c) 1993, Stirling Technologies, Inc.
//                      All rights reserved.
//=======================================================================

//=======================================================================
//      File:   RESRES.C
//
//      This file will be used to create resource-only DLL
//=======================================================================

#define NOCOMM
#include <windows.h>

        int a;

int FAR PASCAL LibMain ( HANDLE hInstance, WORD wDataSeg, WORD wHeapSize,
                         LPSTR  lpszCmdLine )
{
        if ( wHeapSize > 0 )
                UnlockData (0);

        a = 1;

        return 1;
}

/*------------------------------------------------------------------------*/
/*                                                                        */
/* Class :      NULL                                                      */
/* Name  :      WEP                                                       */
/*                                                                        */
/* Descrip      This is a standard windows exit procedure.                */
/*                                                                        */
/* Misc:                                                                  */
/*                                                                        */
/*------------------------------------------------------------------------*/
int FAR PASCAL WEP (bSystemExit)
       int  bSystemExit;
       {

          return 1;

       }

