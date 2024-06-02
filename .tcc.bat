@echo off
if EXIST "c:\Program Files\tcc" (
    %comspec% /k ".tccVar.bat"
) else if EXIST "C:\Program Files (x86)\tcc" (
    %comspec% /k ".tccVar32.bat"
)
