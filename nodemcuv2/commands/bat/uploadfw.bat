@echo off
CALL commands\\bat\\board.bat

if "%1"=="" (
	pio run -e %BOARDTYPE% -t upload
) else (
	pio run -e %BOARDTYPE% -t upload --upload-port %1
)