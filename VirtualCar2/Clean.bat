@ECHO OFF

REM ****************************************
REM   Delete working folders
REM ****************************************

IF "%OS%" == "Windows_NT" GOTO WinNT
FOR %%i IN (Debug, Release) DO DELTREE %%i
GOTO CONT2
:WinNT
FOR %%i IN (Debug, Release) DO IF EXIST %%i RD %%i /S/Q
:CONT2

REM ****************************************
REM   Delete files
REM ****************************************

FOR %%i IN (OPT, PLG, APS, NCB) DO IF EXIST *.%%i DEL *.%%i


REM ****************************************
REM   Delete files under bin
REM ****************************************

FOR %%i IN (SUP,ILK) DO IF EXIST BIN\*.%%i DEL BIN\*.%%i
