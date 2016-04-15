echo OFF
echo 
echo **** Build TBXSDK.RED, CHPSDK.RED, CVTSDK.RED, IDXSDK.RED ****
echo 
pause
cd src001
..\..\r
copy *.red ..\dsk001/v
del  *.red
cd ..
cd src002\ChpSDK
echo **** CHPSDK ****
pause
..\..\..\r
copy *.red ..\..\dsk002/v
del  *.red
cd ..\..
cd src002\CvtSDK
echo **** CVTSDK ****
pause
..\..\..\r
copy *.red ..\..\dsk002/v
del  *.red
cd ..\..
cd src002\IdxSDK
echo **** IDXSDK ****
pause
..\..\..\r
copy *.red ..\..\dsk002/v
del  *.red
cd ..\..
echo ON
call setdat

