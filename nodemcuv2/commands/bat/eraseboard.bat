@echo off
CALL commands\\bat\\board.bat

if "%1"=="" (
	pio run -e %BOARDTYPE% -t erase
) else (
	pio run -e %BOARDTYPE% -t erase --upload-port %1
)