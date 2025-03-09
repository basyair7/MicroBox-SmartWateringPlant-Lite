@echo off
CALL commands\\bat\\board.bat

if "%1"=="" (
	pio run -e %BOARDTYPE% -t uploadfs
) else (
	pio run -e %BOARDTYPE% -t uploadfs --upload-port %1
)