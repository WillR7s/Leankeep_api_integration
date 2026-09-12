@echo off
title MainFlow

echo.
echo ========================================
echo          INICIANDO O MAINFLOW
echo ========================================
echo.

if not exist "%~dp0MainFlow.exe" (
    echo ERRO: MainFlow.exe nao foi encontrado.
    pause
    exit /b 1
)

echo Iniciando o servidor...
start "" /D "%~dp0" "%~dp0MainFlow.exe"

echo Aguardando o servidor iniciar...
timeout /t 3 /nobreak >nul

echo Abrindo o MainFlow...
start "" "https://willr7s.github.io/Leankeep_api_integration/Layout/"

echo.
echo MainFlow iniciado!
echo Pode fechar esta janela.
echo.

timeout /t 3 /nobreak >nul
exit