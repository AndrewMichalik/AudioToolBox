del requires.tmp
for %%f in (*.*) do c:\dtools\requires %%f >>requires.tmp
sort <requires.tmp >requires.txt
del requires.tmp
