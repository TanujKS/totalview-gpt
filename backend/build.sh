#!/bin/bash
# Build script for the backend server

# Install libcurl development libraries if not already installed
sudo apt-get install libcurl4-openssl-dev  # Ubuntu/Debian
# sudo yum install libcurl-devel            # CentOS/RHEL

# Compile with libcurl
g++ -std=c++17 -o llm_poc main.cpp -lcurl -lpthread

echo "Build complete! Run with: ./llm_poc"

