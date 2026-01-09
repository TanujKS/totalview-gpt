@echo off
REM Build script for 32-bit Windows application
REM Requires MinGW-w64 or MSVC compiler
REM Uses WinHTTP (Windows built-in API, no external dependencies)

echo Building 32-bit Windows application...

REM Delete existing executable if it exists
if exist llm_poc.exe del /f /q llm_poc.exe

REM Using MinGW-w64 (g++) for 32-bit Windows
REM Install MinGW-w64 with: pacman -S mingw-w64-i686-gcc

REM For 32-bit MinGW (i686):
REM WinHTTP and Winsock2 are linked via pragma comments in source files, but explicitly link for MinGW
REM Compile all source files and link together
g++ -std=c++17 -m32 -o llm_poc.exe main.cpp logger.cpp utils.cpp config.cpp http_server.cpp openai_client.cpp handlers.cpp server.cpp -lwinhttp -lws2_32 -static-libgcc -static-libstdc++

REM Alternative: Using MSVC (cl.exe)
REM cl /EHsc /std:c++17 /MD /arch:IA32 main.cpp /link winhttp.lib ws2_32.lib

if %ERRORLEVEL% EQU 0 (
    echo Build complete! Run with: llm_poc.exe
) else (
    echo Build failed!
    exit /b 1
)

