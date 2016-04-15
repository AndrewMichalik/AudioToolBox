//-----------------------------------------------------------------------------
//                         InstallSHIELD Sample Files
// Copyright (c) 1990-1993, Stirling Technologies, Inc. All Rights Reserved
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      FILE:   MISC.H
//
//      PURPOSE:
//         This header file defines the constants used in MISC.RUL. It must
//         be included in the MISC.RUL script file. This file must also be
//         available to the dialog box editor, when editing the related
//         dialog box resources.
//
//      RESOURCE: MISC.DLG
//
//      RELATED FILES: MISC.H  MISC.RUL
//
//-----------------------------------------------------------------------------

declare

prototype        _CreateDLG                      ( STRING, INT, LONG );
prototype        _GiveOptionsWhenDoneInstalling  ( );
prototype        _TerminateInstallation          ( );
prototype        _Error                          ( STRING );
prototype        _CreateNewStringList            ( POINTER, POINTER, POINTER );
prototype        _DrawBitMap                     ( SHORT, STRING );
prototype        _WelcomeEx                      ( );
prototype        _PrepareForExit                 ( );
prototype        _PreviousVersionFound           ( );
prototype        _InsufficientDiskSpace          ( );
prototype        _BuildDrivesList                ( POINTER, NUMBER);
prototype        _ErrorMessageBox                ( STRING,  SHORT);
prototype        _DisplayPleaseWaitDialog        ( SHORT, STRING );



#define STR_ERR_DEFINEDLG   "Unable to create dialog box.\n\n Try running SETUP again."
#define STR_ERR_STRING      "Setup could not be completed due to system errors.\n\n"
#define STR_WELCOME_TIT     "Welcome"

#define DLG_EXIT                             14000

#define DLG_START                            2000

#define DLG_DONE                             1000
#define ID_DLG_DONE_RUN                      1001
#define ID_DLG_DONE_EXIT                     1002
#define ID_DLG_DONE_HELP                     1005


#define DLG_VERSION                          1085
#define ID_DLG_VERSION_CHANGE_DIR            1086


#define DLG_DISKSPACE                        1088
#define ID_DLG_DISKSPACE_MAINMENU            1089

#define DLG_PLWAIT                        1090

#define IDOK               1
#define IDCANCEL           2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7
#define IDHELP              998

