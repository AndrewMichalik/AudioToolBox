cd %1
del object\*.obj
nmake %2= >>..\err.txt
cd ..
