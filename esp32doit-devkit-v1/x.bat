@echo off

IF "%1" == "" (
    cls
    echo ShortCut Command PlatformIO
    echo running example : "%0 <command> <port>"
    echo %0 monitor or %0 monitor com0 
    echo.
    echo list commands :
    for %%f in (commands\bat\*.bat) do (
        ::set "filelist=!filelist! %%~nxf"
        echo - %%~nxf
    )

) ELSE (
    CALL commands\\bat\\%1 %2
)