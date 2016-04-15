//-----------------------------------------------------------------------------
//                         InstallSHIELD Sample Files
// Copyright (c) 1990-1993, Stirling Technologies, Inc. All Rights Reserved
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      FILE:   WINSUB.H
//
//      PURPOSE:
//         This header file defines the constants, global variables, and
//         the prototype of the functions in the releated files.
//
//      RELATED FILES: WINSUB.RUL
//
//-----------------------------------------------------------------------------

declare

        //
        // Function definitions
        //


        // Visible entries

        prototype _WinSubGetModuleHandle ( STRING );

        prototype _WinSubShowCursor      ( INT );
        prototype _WinSubShowWindow      ( INT, INT );
        prototype _WinSubFocusControl    ( INT, INT );
        prototype _WinSubFocusWindow     ( INT );
        prototype _WinSubEnableControl   ( INT, INT, INT );
        prototype _WinSubGetChildWindow  ( INT, INT );
        prototype _WinSubEnableWindow    ( INT, INT );
        prototype _WinSubSetWindowTitle  ( INT, STRING );
        prototype _WinSubCenterWindow    ( INT );
        prototype _WinSubIsWindow        ( INT );
        prototype _WinSubGetWindowRect   ( INT, BYREF INT, BYREF INT,
                                               BYREF INT, BYREF INT );
        prototype _WinSubGetClientRect   ( INT, BYREF INT, BYREF INT,
                                               BYREF INT, BYREF INT );
        prototype _WinSubSetWindowPos    ( INT, INT, INT, INT, INT, INT );
        prototype _WinSubMoveWindow      ( INT, INT, INT, INT, INT, INT );
        prototype _WinSubPostMessage     ( INT, SHORT, SHORT, LONG );
        prototype _WinSubScreenSaver     ( SHORT );
        prototype _WinSubCheckScreenSaver (  );


        // Referenced DLL functions
        prototype INT KERNEL.GetModuleHandle ( STRING );

        prototype USER.SetWindowText     ( INT, POINTER );
        prototype BOOL USER.IsWindow          ( INT );
        prototype USER.GetWindowRect     ( INT, POINTER );
        prototype USER.GetClientRect     ( INT, POINTER );
        prototype USER.SetWindowPos      ( INT, INT, INT, INT, INT, INT, INT );
        prototype USER.MoveWindow        ( INT, INT, INT, INT, INT, INT );
        prototype BOOL USER.EnableWindow      ( INT, SHORT );
        prototype USER.GetDlgItem        ( INT, INT );
        prototype USER.PostMessage       ( INT, SHORT, SHORT, LONG );
        prototype USER.SetFocus          ( INT );
        prototype USER.ShowCursor        ( SHORT );
        prototype USER.ShowWindow        ( INT, SHORT );
        prototype USER.SystemParametersInfo( INT, INT, POINTER , INT );


        // Provate entries
        prototype _WinSubSetWindowText ( INT, STRING );
        prototype _WinSubErrDLL        ( STRING );
        prototype _WinSubErrDLLFunc    ( STRING );



        //
        // Constants definition
        //

        // Constants retrieved from windows.h
#ifndef SWP_NOSIZE
        #define SWP_NOSIZE              0x0001
        #define SWP_NOMOVE              0x0002
        #define SWP_NOZORDER            0x0004
        #define SWP_NOREDRAW            0x0008
        #define SWP_NOACTIVATE          0x0010
        #define SWP_FRAMECHANGED        0x0020
        #define SWP_SHOWWINDOW          0x0040
        #define SWP_HIDEWINDOW          0x0080
        #define SWP_NOCOPYBITS          0x0100
        #define SWP_NOOWNERZORDER       0x0200
        #define SWP_DRAWFRAME           SWP_FRAMECHANGED
        #define SWP_NOREPOSITION        SWP_NOOWNERZORDER
        #define SWP_NOSENDCHANGING      0x0400
        #define SWP_DEFERERASE          0x2000
#endif

#ifndef  SPI_GETBEEP

        #define SPI_GETBEEP                 1
        #define SPI_SETBEEP                 2
        #define SPI_GETMOUSE                3
        #define SPI_SETMOUSE                4
        #define SPI_GETBORDER               5
        #define SPI_SETBORDER               6
        #define SPI_GETKEYBOARDSPEED        10
        #define SPI_SETKEYBOARDSPEED        11
        #define SPI_LANGDRIVER              12
        #define SPI_ICONHORIZONTALSPACING   13
        #define SPI_GETSCREENSAVETIMEOUT    14
        #define SPI_SETSCREENSAVETIMEOUT    15
        #define SPI_GETSCREENSAVEACTIVE     16
        #define SPI_SETSCREENSAVEACTIVE     17
        #define SPI_GETGRIDGRANULARITY      18
        #define SPI_SETGRIDGRANULARITY      19
        #define SPI_SETDESKWALLPAPER        20
        #define SPI_SETDESKPATTERN          21
        #define SPI_GETKEYBOARDDELAY        22
        #define SPI_SETKEYBOARDDELAY        23
        #define SPI_ICONVERTICALSPACING     24
        #define SPI_GETICONTITLEWRAP        25
        #define SPI_SETICONTITLEWRAP        26
        #define SPI_GETMENUDROPALIGNMENT    27
        #define SPI_SETMENUDROPALIGNMENT    28
        #define SPI_SETDOUBLECLKWIDTH       29
        #define SPI_SETDOUBLECLKHEIGHT      30
        #define SPI_GETICONTITLELOGFONT     31
        #define SPI_SETDOUBLECLICKTIME      32
        #define SPI_SETMOUSEBUTTONSWAP      33
        #define SPI_SETICONTITLELOGFONT     34
        #define SPI_GETFASTTASKSWITCH       35
        #define SPI_SETFASTTASKSWITCH       36
#endif

#ifndef SPIF_UPDATEINIFILE

/* SystemParametersInfo flags */
        #define SPIF_UPDATEINIFILE          0x0001
        #define SPIF_SENDWININICHANGE       0x0002

#endif

        // Private constants
        #define MAX_WINSUB_TMPSTRING_LENGTH     80
        #define MAX_WINSUB_MSGSTRING_LENGTH     255


        //
        // Struct definition
        //

        typedef WINRECTSTRUCT
        begin
                INT     origX;
                INT     origY;
                INT     relX;
                INT     relY;
        end;


