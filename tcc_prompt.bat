@echo off
if EXIST "c:\Program Files\tcc" (
    %comspec% /k "C:\Program Files\tcc\bin\tcc_env.bat"
) else if EXIST "C:\Program Files (x86)\tcc" (
    %comspec% /k "C:\Program Files (x86)\tcc\bin\tcc_env.bat"
)
