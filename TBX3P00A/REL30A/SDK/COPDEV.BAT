copy ..\..\reldev\w31\*.* src001/v
REM  Copy PCM All EXEs and DLLs
copy ..\..\reldev\dev\sdkrel.txt src001/v
copy ..\..\reldev\dev\readme.txt src001/v
REM  Copy Sound Chop specific SDK files
copy ..\..\reldev\dev\chplib\*.*        src002\ChpSDK/v
copy ..\..\reldev\dev\mscchp\*.*        src002\ChpSDK/v
copy ..\..\reldev\dev\mscchp\object\*.* src002\ChpSDK/v
REM  Copy PCM Convert specific SDK files
copy ..\..\reldev\dev\cvtlib\*.*        src002\CvtSDK/v
copy ..\..\reldev\dev\msccvt\*.*        src002\CvtSDK/v
copy ..\..\reldev\dev\msccvt\object\*.* src002\CvtSDK/v
copy ..\..\reldev\dev\mvbcvt\*.*        src002\CvtSDK/v
REM  Copy Index File specific SDK files
copy ..\..\reldev\dev\idxlib\*.*        src002\IdxSDK/v
copy ..\..\reldev\dev\mscidx\*.*        src002\IdxSDK/v
copy ..\..\reldev\dev\mscidx\object\*.* src002\IdxSDK/v
REM  Delete un-released files
del  src001\har???.*
del  src001\rtx???.*
del  src001\???spd.*
del  src002\ChpSDK\har???.*
del  src002\ChpSDK\rtx???.*
del  src002\CvtSDK\har???.*
del  src002\CvtSDK\rtx???.*
