@echo off
REM Build script for 32-bit Windows application
REM Requires MinGW-w64 or MSVC compiler
REM Uses WinHTTP (Windows built-in API, no external dependencies)

echo Building 32-bit Windows application...

REM Using MinGW-w64 (g++) for 32-bit Windows
REM Install MinGW-w64 with: pacman -S mingw-w64-i686-gcc

REM For 32-bit MinGW (i686):
REM WinHTTP is linked via pragma comment in main.cpp, but explicitly link for MinGW
g++ -std=c++17 -m32 -o llm_poc.exe main.cpp -lwinhttp -lpthread -static-libgcc -static-libstdc++

REM Alternative: Using MSVC (cl.exe)
REM cl /EHsc /std:c++17 /MD /arch:IA32 main.cpp /link winhttp.lib ws2_32.lib

if %ERRORLEVEL% EQU 0 (
    echo Build complete! Run with: llm_poc.exe
) else (
    echo Build failed!
    exit /b 1
)

