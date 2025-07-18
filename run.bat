@echo off
REM 如果提供了参数，则设定运行次数，否则默认为1
set count=1
if not "%1"=="" set count=%1

for /l %%i in (1,1,%count%) do (
    main.exe
)
