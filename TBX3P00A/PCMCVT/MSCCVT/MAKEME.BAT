REM Make Dialogic Microsoft Large   model Emulator version
del object\*.obj
nmake dlg= mle=
REM Make Dialogic Microsoft Windows model Emulator version
del object\*.obj
nmake dlg= mwe=

REM Make CCITT G.721 Microsoft Large   model Emulator version
nmake g21= mle=
REM Make CCITT G.721 Microsoft Windows model Emulator version
nmake g21= mwe=

REM Make NewVoice Microsoft Large   model Emulator version
nmake nwv= mle=
REM Make NewVoice Microsoft Windows model Emulator version
nmake nwv= mwe=

REM Make Perception Technology Microsoft Large   model Emulator version
nmake ptc= mle=
REM Make Perception Technology Microsoft Windows model Emulator version
nmake ptc= mwe=
