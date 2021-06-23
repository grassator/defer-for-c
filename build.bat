@echo off

if not defined DevEnvDir (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
)

rmdir /S /Q build
mkdir build

copy demo.c build\

pushd build

set FLAGS=/Od /std:c11 /nologo /Zo /Zi /Wall /wd4996 /wd4710

cl %FLAGS% ..\demo.c
if %errorlevel% neq 0 (goto Fail)

:Success
popd
exit /b 0

:Fail
popd
exit /b 1