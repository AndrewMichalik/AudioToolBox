touch /s /h /q /c /m /a /d 06-05-2005 /t 06:00 Application\*.*
touch /s /h /q /c /m /a /d 06-05-2005 /t 06:00 Setup\*.*
touch /s /h /q /c /m /a /d 06-05-2005 /t 06:00 Source\*.*
CD Application
..\touch /s    /q /c /m /a /d 11-17-2003 /t 21:20 Redemption.dll
..\touch /s    /q /c /m /a /d 11-17-2003 /t 21:20 Interop.Redemption.dll
..\touch /s    /q /c /m /a /d 09-30-2002 /t 04:10 Office.dll
..\touch /s    /q  c /m /a /d 09-30-2002 /t 04:10 Interop.Office.dll
..\touch /s    /q /c /m /a /d 03-02-1999 /t 11:53 msoutl9.olb
..\touch /s    /q /c /m /a /d 03-02-1999 /t 11:53 Outlook.dll
..\touch /s    /q /c /m /a /d 03-02-1999 /t 11:53 Interop.Outlook.dll
..\touch /s    /q /c /m /a /d 03-19-2003 /t 01:53 Microsoft.mshtml.dll
cd ..
if "%1" == "" Pause
