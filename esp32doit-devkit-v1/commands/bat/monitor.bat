@echo off

if "%1"=="" (
	pio device monitor
) else (
	pio device monitor --port %1
)