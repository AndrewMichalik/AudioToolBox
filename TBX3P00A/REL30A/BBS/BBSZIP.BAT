del *.zip
fd  *.*          /t12:00/d10-01-96
pkzip idxchp.zip -ex ..\license.txt idxchp.txt  ..\ret\src001\idxchp.* ..\ret\src001\idxtst.vap
pkzip idxlst.zip -ex ..\license.txt             ..\ret\src001\idxlst.* ..\ret\src001\idxtst.vap
pkzip idxpak.zip -ex ..\license.txt idxpak.txt  ..\ret\src001\idxpak.* ..\ret\src001\pak*.*
pkzip idxreb.zip -ex ..\license.txt             ..\ret\src001\idxreb.* ..\ret\src001\idxtst.vap
pkzip sndchp.zip -ex ..\license.txt             ..\ret\src001\sndchp.* ..\ret\src001\pcmcvt.wav
pkzip dlgcvt.zip -ex ..\license.txt pcmcvt.txt  ..\ret\src001\pcmcvt.* ..\ret\src001\dlgcvt.exe 
pkzip tbx001.zip -ex                            ..\ret\dsk001\*.*
fd    tbx00?.zip /t12:00/d10-01-96
pkzip tbxall.zip -ex tbxman.*       tbxall.txt  tbx001.zip
pkzip sdk001.zip -ex                            ..\sdk\dsk001\*.*
pkzip sdk002.zip -ex                            ..\sdk\dsk002\*.*
fd    sdk00?.zip /t12:00/d10-01-96
pkzip tbxsdk.zip -ex tbxsdk.txt                 sdk001.zip sdk002.zip 
pkzip app001.zip -ex                            ..\..\appdev\insdev\dsk001\*.* -rp
fd    app00?.zip /t12:00/d10-01-96
pkzip tbxapp.zip -ex tbxapp.txt                 app001.zip 
fd *.zip /t12:00/d10-01-96
