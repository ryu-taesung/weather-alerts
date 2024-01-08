@echo off
FOR /F "tokens=*" %%i IN ('git describe --always --tags') DO SET GIT_VERSION=%%i
echo %GIT_VERSION%
