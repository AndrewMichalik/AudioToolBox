cd %1
del object\*.obj
nmake %2= %3= >>..\err.txt
cd ..

