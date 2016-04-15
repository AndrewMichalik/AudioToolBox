REM 
REM Set the application level version numbers
REM 
bfr 3.00a.0 3.00a.1 BinRep\*.*          -b    
bfr 3.00a.0 3.00a.1 FrqEff\*.*          -b     
bfr 3.00a.0 3.00a.1 IdxFil\*.*          -b     
bfr 3.00a.0 3.00a.1 IdxFil\MSCIdx\*.*   -b     
bfr 3.00a.0 3.00a.1 PCMCvt\*.*          -b    
bfr 3.00a.0 3.00a.1 PCMCvt\MSCCvt\*.*   -b
bfr 3.00a.0 3.00a.1 SndChp\*.*          -b 
bfr 3.00a.0 3.00a.1 SndChp\MSCChp\*.*   -b 
bfr 3.00a.0 3.00a.1 Su4Chp\*.*          -b 
bfr 3.00a.0 3.00a.1 Su4Pak\*.*          -b 
bfr 3.00a.0 3.00a.1 AppDev\*.*          -b 

bfr "3,00,0,00" "3,00,0,01" IdxFil\IdxLib\*.rc      -b
bfr "3,00,0,00" "3,00,0,01" PCMCvt\CvtLib\*.rc      -b
bfr "3,00,0,00" "3,00,0,01" SndChp\ChpLib\*.rc      -b

bfr "3.00.000"  "3.00.001"  IdxFil\IdxLib\*.rc      -b
bfr "3.00.000"  "3.00.001"  PCMCvt\CvtLib\*.rc      -b
bfr "3.00.000"  "3.00.001"  SndChp\ChpLib\*.rc      -b

bfr "2.10"      "3.00"      IdxFil\IdxLib\???ini.c  -b
bfr "2.10"      "3.00"      PCMCvt\CvtLib\???ini.c  -b
bfr "2.10"      "3.00"      SndChp\ChpLib\???ini.c  -b

bfr "0x0210"    "0x0300"    IdxFil\IdxLib\gen*.h    -b     
bfr "0x0210"    "0x0300"    PCMCvt\CvtLib\gen*.h    -b 
bfr " &H210"    " &H300"    PCMCvt\MVBCvt\gen*.txt  -b 
bfr "0x0210"    "0x0300"    SndChp\ChpLib\gen*.h    -b 

REM 
REM Set the DLL level version numbers
REM 

bfr "0x0210"    "0x0300"    IdxFil\IdxLib\gen*.h    -b     
bfr "0x0210"    "0x0300"    PCMCvt\CvtLib\gen*.h    -b     
bfr "0x0210"    "0x0300"    SndChp\ChpLib\gen*.h    -b     
