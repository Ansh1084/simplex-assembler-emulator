@echo off
g++ asm.cpp -o asm.exe
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

for %%f in (successful_test\*.asm) do (
    echo Running on %%f
    asm.exe "%%f"
    echo --- Log for %%f ---
    type "%%~dpnxf.log"
    echo.
)
