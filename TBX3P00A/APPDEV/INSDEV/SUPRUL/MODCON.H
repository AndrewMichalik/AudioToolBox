
//-----------------------------------------------------------------------------
//                         InstallSHIELD Sample Files
// Copyright (c) 1990-1993, Stirling Technologies, Inc. All Rights Reserved
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      FILE:   MODCON.H
//
//      PURPOSE:
//         This header file defines the constants used in MODCON.RUL. It must
//         be included in the MODCON.RUL script file. This file must also be
//         available to the dialog box editor, when editing the related
//         dialog box resources.
//
//      RESOURCE: MODCON.DLG
//
//      RELATED FILES: MODCON.H  MODCON.RUL
//
//-----------------------------------------------------------------------------
declare

prototype       _CreateFileConList         (POINTER);
prototype       _ConfigUpdate              (POINTER, POINTER);
prototype       _ConfirmConfigFile         (POINTER, POINTER);
prototype       _CONFIG_OK                  (POINTER, POINTER, POINTER, STRING);
prototype       _CONFIGEX_DlgProc           (POINTER, POINTER, POINTER);
prototype       _CONFIG_DlgProc           (POINTER, POINTER, POINTER, POINTER, POINTER);
prototype       _CONFIG_Radio               (POINTER, POINTER, SHORT, SHORT, POINTER, STRING);
prototype       _LetInstallModifyConfig    (POINTER, POINTER);
prototype       _ModifyConfigFile          (POINTER, POINTER, POINTER, POINTER, POINTER);
prototype       _SaveConfigFile            (POINTER, STRING);
prototype       _WriteConfigFile           (POINTER);
//-----------------------------------------------------------------------------
// String constants used in DIALOGS\CONFIG\MODCON.RUL
//-----------------------------------------------------------------------------

#define STR_ERR_FILE             "Setup cannot open the CONFIG.SYS file."
#define STR_ERR_DLG              "Internal Error. Unable to show a dialog box."
#define STR_CON_UPDATE           "The updated version of your CONFIG.SYS file \n has been saved as "
#define STR_CON_DONE             "Setup has made the appropriate changes to your CONFIG.SYS file."
#define STR_CON_TIT             "Modify CONFIG.SYS file"
#define STR_CON_OP1             "&Let Setup modify the CONFIG.SYS file."
#define STR_CON_OP2             "&Save the required changes to new file: CONFIG.NEW"
#define STR_CON_OP3             "&Do not make any changes."
#define STR_CANNOT_CREATE       "Cannot create file: "

#define DLG_CONFIGEX                      13000
#define ID_DLG_CONFIGEX_LIST              13002
#define ID_DLG_CONFIGEX_CHANGE          13004
#define ID_DLG_CONFIGEX_SAVE              13005
#define ID_DLG_CONFIGEX_GETTEXT           13006
#define ID_DLG_CONFIGEX_NOCHANGE          13007
#define ID_DLG_CONFIGEX_MLE               13008
#define ID_DLG_CONFIGEX_EDIT              13011
#define ID_DLG_CONFIGEX_CANCEL_EDIT       13013

#define DLG_CONFIG                    6000
#define ID_DLG_CONFIG_LIST            6002
#define ID_DLG_CONFIG_CHANGE          6004
#define ID_DLG_CONFIG_SAVE            6005
#define ID_DLG_CONFIG_GETTEXT         6006
#define ID_DLG_CONFIG_NOCHANGE        6007
#define ID_DLG_CONFIG_EDIT            6011

