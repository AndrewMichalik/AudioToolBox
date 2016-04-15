ECHO OFF
del err.txt
REM  ********************************************
REM  ** Go to each directory and perform build **
REM  ********************************************
REM  ********************************************
REM  ** Binary File Replacement Utility        **
REM  ********************************************
call mak001 BinRep ALL         
REM  ********************************************
REM  ** Speed Adjust                           **
REM  ********************************************
call mak001 SpdAdj ALL         
REM  ********************************************
REM  ** Indexed File DOS & Win Libraries       **
REM  ********************************************
del  IdxFil\err.txt
del  IdxFil\IdxLib\*.lib   
call mak001 IdxFil\IdxLib all     
cd ..
call mak002 IdxFil\IdxLib all mwe    
REM  
REM  ********************************************
REM  ** Indexed File SDK "C" samples           **
REM  ********************************************
REM  Stay in subdirectory...
cd   mscidx
del  *.exe
call makeme
xcopy *.*           ..\..\reldev\dev\mscidx/s
cd   ..\..
REM  ********************************************
REM  ** Indexed FIle .EXE's                    **
REM  ********************************************
call mak001 IdxFil all         
call mak002 IdxFil all mwe        
REM
REM  ********************************************
REM  ** Conversion DOS & Win Libraries         **
REM  ********************************************
del  PCMCvt\err.txt
del  PCMCvt\CvtLib\*.lib   
call mak001 PCMCvt\CvtLib dlg     
cd ..
call mak002 PCMCvt\CvtLib dlg mwe    
cd ..
call mak001 PCMCvt\CvtLib har     
cd ..
call mak002 PCMCvt\CvtLib har mwe    
cd ..
call mak001 PCMCvt\CvtLib g21     
cd ..
call mak002 PCMCvt\CvtLib g21 mwe    
cd ..
call mak001 PCMCvt\CvtLib nwv     
cd ..
call mak002 PCMCvt\CvtLib nwv mwe    
cd ..
call mak001 PCMCvt\CvtLib ptc     
cd ..
call mak002 PCMCvt\CvtLib ptc mwe    
cd ..
call mak001 PCMCvt\CvtLib rtx     
cd ..
call mak002 PCMCvt\CvtLib rtx mwe    
del  CvtLib\GenCvt.Lib
del  CvtLib\GenCvt.DLL
REM  
REM  ********************************************
REM  ** Conversion SDK "C" samples             **
REM  ********************************************
REM  Stay in subdirectory...
cd   msccvt
del  *.exe
call makeme
xcopy *.*           ..\..\reldev\dev\msccvt/s
cd   ..
REM  ********************************************
REM  ** Conversion SDK "VB" samples            **
REM  ********************************************
cd   mvbcvt
copy *.*            ..\..\reldev\dev\mvbcvt
cd   ..\..
REM  ********************************************
REM  ** Update Conversion .EXE's               **
REM  ********************************************
call mak001 PCMCvt dlg     
call mak002 PCMCvt dlg mwe    
call mak001 PCMCvt g21     
call mak002 PCMCvt g21 mwe    
call mak001 PCMCvt har     
call mak002 PCMCvt har mwe    
call mak001 PCMCvt nwv     
call mak002 PCMCvt nwv mwe    
call mak001 PCMCvt ptc     
call mak002 PCMCvt ptc mwe     
call mak001 PCMCvt rtx     
call mak002 PCMCvt rtx mwe     
REM
REM  ********************************************
REM  ** Sound Chop DOS & Win Libraries         **
REM  ********************************************
del  SndChp\err.txt
del  SndChp\ChpLib\*.lib   
call mak001 SndChp\ChpLib dlg     
cd ..
call mak002 SndChp\ChpLib dlg mwe    
cd ..
call mak001 SndChp\ChpLib har     
cd ..
call mak002 SndChp\ChpLib har mwe    
cd ..
call mak001 SndChp\ChpLib g21     
cd ..
call mak002 SndChp\ChpLib g21 mwe    
cd ..
call mak001 SndChp\ChpLib nwv     
cd ..
call mak002 SndChp\ChpLib nwv mwe    
cd ..
call mak001 SndChp\ChpLib ptc     
cd ..
call mak002 SndChp\ChpLib ptc mwe    
cd ..
call mak001 SndChp\ChpLib rtx     
cd ..
call mak002 SndChp\ChpLib rtx mwe    
del  ChpLib\GenChp.Lib
del  ChpLib\GenChp.DLL
REM  
REM  ********************************************
REM  ** Sound Chop SDK "C" samples             **
REM  ********************************************
REM  Stay in subdirectory...
cd   mscchp
del  *.exe
call makeme
xcopy *.*           ..\..\reldev\dev\mscchp/s
cd   ..\..
REM  ********************************************
REM  ** Sound Chop .EXE's                      **
REM  ********************************************
call mak001 SndChp dlg     
call mak002 SndChp dlg mwe    
call mak001 SndChp g21     
call mak002 SndChp g21 mwe    
call mak001 SndChp har     
call mak002 SndChp har mwe    
call mak001 SndChp nwv     
call mak002 SndChp nwv mwe    
call mak001 SndChp ptc     
call mak002 SndChp ptc mwe     
call mak001 SndChp rtx     
call mak002 SndChp rtx mwe     
REM
REM  ********************************************
REM  ** Summa Four .EXE's                      **
REM  ********************************************
call mak001 Su4Chp ALL         
call mak001 Su4Pak ALL         
ECHO ON
