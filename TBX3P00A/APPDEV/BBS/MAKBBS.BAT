fd    *.*/t12:00/d10-01-96
del   *.zip
pkzip app001.zip ..\insdev\dsk001\*.* -rp
fd    app001.zip/t12:00/d10-01-96
pkzip tba3p0.zip tbxapp.txt app001.zip 
fd    tba3p0.zip/t12:00/d10-01-96
del   app001.zip
