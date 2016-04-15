//-----------------------------------------------------------------------------
//                         InstallSHIELD Sample Files
// Copyright (c) 1990-1993, Stirling Technologies, Inc. All Rights Reserved
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      FILE:   REG.H
//
//      PURPOSE:
//         This header file defines the constants, global variables, and
//         the prototype of the functions in the releated files.
//
//      RELATED FILES: REG.RUL
//
//-----------------------------------------------------------------------------


declare
        // Set-1, to handle username+company
        prototype _RegistrationGet ( STRING, STRING, STRING, BYREF STRING,
                                                    STRING, BYREF STRING );
        prototype _RegistrationPut ( STRING, STRING, STRING, STRING,
                                                    STRING, STRING );

        // Set-2, to handle username+company+S/N
        prototype _RegistrationGetEx ( STRING, STRING, STRING, BYREF STRING,
                                                      STRING, BYREF STRING,
                                                      STRING, BYREF STRING );
        prototype _RegistrationPutEx ( STRING, STRING, STRING, STRING, STRING,
                                                      STRING, STRING, STRING );

