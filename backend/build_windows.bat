@echo off
REM Build script for 32-bit Windows application
REM Requires MinGW-w64 or MSVC compiler

echo Building 32-bit Windows application...

REM Using MinGW-w64 (g++) for 32-bit Windows
REM Install MinGW-w64 with: pacman -S mingw-w64-i686-gcc mingw-w64-i686-curl

REM For 32-bit MinGW (i686):
g++ -std=c++17 -m32 -o llm_poc.exe main.cpp -lcurl -lpthread -static-libgcc -static-libstdc++

REM Alternative: Using MSVC (cl.exe)
REM cl /EHsc /std:c++17 /MD /arch:IA32 main.cpp /link libcurl.lib ws2_32.lib

if %ERRORLEVEL% EQU 0 (
    echo Build complete! Run with: llm_poc.exe
) else (
    echo Build failed!
    exit /b 1
)

