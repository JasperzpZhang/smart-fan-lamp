@echo off
:START 

.\bin\make_image.py

set TOOL=stm32f407vg.jflash
set FILE=.\bin\app.bin
start ./JFlashTool/JFlashARM.exe -openprj./JFlashTool/%TOOL% -open%FILE%,0x8000000 -auto -startapp -exit

exit