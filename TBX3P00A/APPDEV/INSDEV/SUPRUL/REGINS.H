
//-----------------------------------------------------------------------------
//                         InstallSHIELD Sample Files
// Copyright (c) 1990-1993, Stirling Technologies, Inc. All Rights Reserved
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      FILE:   REGINS.H
//
//      PURPOSE:
//         This header file defines the constants used in REGINS.RUL. It must
//         be included in the REGINS.RUL script file. This file must also be
//         available to the dialog box editor, when editing the related
//         dialog box resources.
//
//      RESOURCE: REGINS.DLG
//
//      RELATED FILES: REGINS.H  REGINS.RUL
//
//-----------------------------------------------------------------------------

declare


prototype      _ConfirmRegistrationEx    ( BYREF STRING, BYREF STRING, BYREF STRING ) ;
prototype      _RegistrationUpdateEx     ( BYREF STRING, BYREF STRING, BYREF STRING ) ;
prototype      _GETREGEX_DlgProc         ( BYREF STRING, BYREF STRING, BYREF STRING ) ;


//-----------------------------------------------------------------------------
// Ids for registration update dialog box
//-----------------------------------------------------------------------------

#define DLG_GETREGEX                  29000
#define ID_DLG_GETREGEX_NAME          29001
#define ID_DLG_GETREGEX_CO            29002
#define ID_DLG_GETREGEX_SERNUM        29009

#define DLG_CONFIRMEX                 4000
#define ID_DLG_CONFIRMEX_NAME         4005
#define ID_DLG_CONFIRMEX_CO           4006
#define ID_DLG_CONFIRMEX_SERNUM       4007

