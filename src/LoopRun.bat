@echo off
:loop
   WindowsWriteCrossPage.exe
   if %ERRORLEVEL% GEQ 3 EXIT /B 3
   goto :loop