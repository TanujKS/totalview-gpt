#!/bin/bash
# Build script for the backend server (Linux)
# For 32-bit Windows, use build_windows.bat

# Install libcurl development libraries if not already installed
# sudo apt-get install libcurl4-openssl-dev  # Ubuntu/Debian
# sudo yum install libcurl-devel            # CentOS/RHEL

# Compile with libcurl (ISO C++17, UTF-8)
g++ -std=c++17 -o llm_poc main.cpp -lcurl -lpthread

if [ $? -eq 0 ]; then
    echo "Build complete! Run with: ./llm_poc"
else
    echo "Build failed!"
    exit 1
fi

