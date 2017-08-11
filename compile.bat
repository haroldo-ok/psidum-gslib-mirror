@echo off
rem :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
rem ::             WLA DX compiling batch file v3              ::
rem :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
rem :: Do not edit anything unless you know what you're doing! ::
rem :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

set WLAPATH=%~dp0%wladx-v9.6\binaries\

rem Cleanup to avoid confusion
if exist object.o del object.o

rem Compile
"%WLAPATH%wla-z80.exe" -o main.asm object.o

rem Make linkfile
echo [objects]>linkfile
echo object.o>>linkfile

rem Link
"%WLAPATH%wlalink.exe" -drvs linkfile GSElibTest.sms

rem Fixup for eSMS
if exist GSElibTest.sms.sym del GSElibTest.sms.sym
ren GSElibTest.sym GSElibTest.sms.sym

rem Cleanup to avoid mess
if exist linkfile del linkfile
if exist object.o del object.o