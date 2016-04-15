REM
REM Source Files
REM
fd *.*                      /t12:00/d10-01-96
fd BINREP\*.*               /t12:00/d10-01-96
fd EFFDEV\*.*               /t12:00/d05-01-96
fd FIODEV\*.*               /t12:00/d10-01-96
fd IDXFIL\*.*               /t12:00/d10-01-96
fd IDXFIL\IDXLIB\*.*        /t12:00/d10-01-96
fd IDXFIL\MSCIDX\*.*        /t12:00/d10-01-96
fd LICDEV\*.*               /t12:00/d10-01-96
fd OS_DEV\*.*               /t12:00/d10-01-96
fd PCMCVT\CVTLIB\*.*        /t12:00/d10-01-96
fd PCMCVT\MSCCVT\*.*        /t12:00/d10-01-96
fd PCMCVT\MVBCVT\*.*        /t12:00/d10-01-96
fd PCMDEV\*.*               /t12:00/d05-01-96
fd REGDEV\*.*               /t12:00/d10-01-96
fd SNDCHP\*.*               /t12:00/d10-01-96
fd SNDCHP\CHPLIB\*.*        /t12:00/d10-01-96
fd SNDCHP\MSCCHP\*.*        /t12:00/d10-01-96
fd SPDADJ\*.*               /t12:00/d10-01-96
fd SU4CHP\*.*               /t12:00/d10-01-96
fd SU4PAK\*.*               /t12:00/d10-01-96
fd USRDEV\*.*               /t12:00/d10-01-96
REM
REM Release files
REM
fd RELDEV\*.*/s             /t12:00/d10-01-96
fd RELDEV\W31\???EFF.*      /t12:00/d05-01-96
fd RELDEV\W31\???PCM.*      /t12:00/d05-01-96
REM
REM Apprentice files
REM
REM fd APPDEV\*.*           /t12:00/d10-01-96
