@echo off
:START 
set TOOL=stm32f107vc.jflash
set FILE=.\bin\sys.bin
start ./JFlashTool/JFlashARM.exe -openprj./JFlashTool/%TOOL% -open%FILE%,0x8000000 -auto -startapp -exit
pause
goto START 
