REM 
REM  ***** Disk 1: Main program files *****
REM 
del                                                 Dsk001\WinXxx\*.exe
del                                                 Dsk001\WinXxx\*.hlp
copy INSTALL.INS                                    Dsk001/v
copy \Betdev\TBxDev\AppDev\AUDTBX.EXE               Dsk001\WinXxx/v
copy \Betdev\TBxDev\AppDev\HlpDev\AUDTBX.HLP        Dsk001\WinXxx/v

copy \Betdev\TBxDev\Appdev\InsDev\ResDev\TBxIns.DLL Dsk001\WinXxx/v
copy \Betdev\TBxDev\AppDev\InsDev\ResDev\TBxIns.DLL Dsk001\InsDir/v

REM 
REM  ***** Disk 1: These files rarely change *****
REM 
REM copy \Betdev\TBxDev\ExeDev\IDXCHP.PIF           Dsk001\WinXxx/v
REM copy \Betdev\TBxDev\ExeDev\IDXLST.PIF           Dsk001\WinXxx/v
REM copy \Betdev\TBxDev\ExeDev\IDXPAK.PIF           Dsk001\WinXxx/v
REM copy \Betdev\TBxDev\ExeDev\IDXREB.PIF           Dsk001\WinXxx/v
REM copy \Betdev\TBxDev\ExeDev\PCMCVT.PIF           Dsk001\WinXxx/v
REM copy \Betdev\TBxDev\ExeDev\SNDCHP.PIF           Dsk001\WinXxx/v

REM 
REM  ***** Disk 1: These files rarely change *****
REM  ***** Note: check to insure that original date is maintained
REM 
REM copy \Betdev\TBxDev\ExeDev\VBRUN300.DLL         Src001\MVB300/v
REM copy \Betdev\TBxDev\ExeDev\MHRUN400.DLL         Src001\MVB300/v  
REM copy \Betdev\TBxDev\ExeDev\MHSL200.DLL          Src001\MVB300/v  
REM copy \Betdev\TBxDev\ExeDev\CMDIALOG.VBX         Src001\MVB300/v
REM copy \Betdev\TBxDev\ExeDev\MHTAB.VBX            Src001\MVB300/v
REM copy \Betdev\TBxDev\ExeDev\THREED.VBX           Src001\MVB300/v
REM copy \Betdev\TBxDev\ExeDev\COMMDLG.DLL          Src001\MVB300/v
REM copy \Betdev\TBxDev\ExeDev\VER.DLL              Src001\MVB300/v

REM 
REM  ***** Disk 1: Build compression sets *****
REM 
Compress src001\mvb300\*.* dsk001\mvb300.z

:ErrEnd

