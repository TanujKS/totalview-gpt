# TotalView GPT Backend

C++ HTTP server that proxies requests to OpenAI's ChatGPT API for network administration assistance.

## Overview

The backend is a lightweight, embeddable HTTP server built with ISO C++17. It handles network monitoring data, formats it into prompts for ChatGPT, and manages API communication with retry logic and rate limiting support.

## Key Features

- **Exception-free**: Main routine handles errors without throwing exceptions
- **Log callback system**: All logging done through callback function (no direct I/O)
- **Retry logic**: Automatic retry with exponential backoff for rate limiting (HTTP 429)
- **32-bit Windows compatible**: Builds for 32-bit Windows architectures
- **UTF-8 strings**: All string handling uses UTF-8 encoding
- **Embeddable**: Can be embedded in other applications via `main_with_logging()` function

## External Dependencies

The backend currently relies on one external library and Windows APIs:

### 1. WinHTTP (Windows HTTP Services API)
**Purpose**: HTTP/HTTPS client for communicating with OpenAI API (Windows built-in, no external dependencies)

**Usage**:
- Makes HTTPS POST requests to `https://api.openai.com/v1/chat/completions`
- Handles SSL/TLS encryption natively
- Manages HTTP headers and request/response data
- No installation required - part of Windows SDK

**Key Functions Used**:
- `WinHttpOpen()` - Initialize WinHTTP session
- `WinHttpConnect()` - Connect to host
- `WinHttpOpenRequest()` - Create HTTP request
- `WinHttpSetOption()` - Set timeout options
- `WinHttpAddRequestHeaders()` - Add HTTP headers
- `WinHttpSendRequest()` - Send HTTP request with body
- `WinHttpReceiveResponse()` - Receive HTTP response
- `WinHttpQueryHeaders()` - Get response status code
- `WinHttpReadData()` - Read response body
- `WinHttpCloseHandle()` - Cleanup resources

**Why it's used**: Native Windows API that provides secure HTTPS communication with proper SSL certificate validation and HTTP protocol handling, without requiring external dependencies like libcurl. Perfect for 32-bit Windows builds.

### 2. Winsock2 (Windows Sockets API)
**Purpose**: HTTP server implementation using native Windows socket APIs

**Usage**:
- Creates HTTP server listening on configurable port (default: 3000)
- Handles POST requests to `/chat` endpoint
- Handles GET requests to `/health` endpoint
- Handles OPTIONS requests for CORS preflight
- Manages CORS headers
- Processes HTTP request/response parsing

**Key Functions Used**:
- `WSAStartup()` - Initialize Winsock
- `socket()` - Create server socket
- `bind()` - Bind socket to port
- `listen()` - Listen for connections
- `accept()` - Accept client connections
- `recv()` - Receive HTTP request data
- `send()` - Send HTTP response data
- `closesocket()` - Close socket connections
- `WSACleanup()` - Cleanup Winsock

**Why it's used**: Native Windows API for socket operations. We migrated from httplib to Winsock2 for better 32-bit Windows compatibility and to eliminate external dependencies. The HTTP server is implemented using standard Windows socket APIs.

### 3. nlohmann/json (`json.hpp`)
**Purpose**: JSON parsing and generation

**Usage**:
- Parses configuration file (`config.json`)
- Parses incoming HTTP request bodies
- Parses OpenAI API responses
- Generates JSON responses and API request payloads

**Key Features Used**:
- `json::parse()` - Parse JSON string
- `json::value()` - Safe field access with defaults
- `json::dump()` - Convert JSON object to string
- `json::array()` - Create JSON array
- `json::object()` - Create JSON object

**Why it's needed**: JSON is the standard format for API communication and configuration files.

## Dependencies Summary

The backend is optimized for Windows builds with minimal dependencies:

### HTTP Client: WinHTTP (Already Implemented)

The code uses WinHTTP API which is built into Windows, so no external dependencies are needed for HTTP/HTTPS client functionality. This makes 32-bit Windows builds straightforward without requiring libcurl.

**Current Implementation**:
- Uses `winhttp.h` (Windows SDK)
- Native SSL/TLS support via WinHTTP
- Proper certificate validation
- No external library installation required

### HTTP Server: Winsock2 (Already Implemented)

The code uses Winsock2 API (also built into Windows) for the HTTP server implementation. We migrated from httplib to Winsock2 for better 32-bit Windows compatibility.

**Current Implementation**:
- Uses `winsock2.h` and `ws2tcpip.h` (Windows SDK)
- Custom HTTP request/response parsing
- CORS header support
- Route handling for `/chat`, `/health`, and OPTIONS endpoints
- No external library installation required


### Option 1: Replace nlohmann/json (if needed)

**Alternatives**:
1. **Manual parsing**: Write recursive descent parser
   - Parse JSON tokens (strings, numbers, booleans, null)
   - Build object/array structures
   - Handle escape sequences and Unicode

2. **Use standard library only**: Minimal JSON implementation
   - Use `std::map<std::string, std::variant<...>>` for objects
   - Use `std::vector` for arrays
   - Implement parsing with string manipulation

3. **State machine parser**: More efficient but complex
   - Use finite state machine for tokenization
   - Build parse tree

**Complexity**: Medium - JSON parsing is well-defined but requires careful implementation.

## Build Instructions

### Windows 32-bit Build

```cmd
REM No external dependencies needed - WinHTTP is part of Windows SDK
REM Only requires MinGW-w64 compiler
REM Tested with MINGW32 (MinGW-w64 32-bit) from MSYS2

REM Build
./build_windows.bat

REM Run from CMD
llm_poc.exe
```

### Embedded Usage

```cpp
#include "main.h"

void my_logger(int level, const std::string& msg) {
    // Your logging implementation
    if (level == 0) {
        log_error(msg);
    } else if (level == 1) {
        log_info(msg);
    } else {
        log_debug(msg);
    }
}

int main() {
    // Set log callback before starting server
    return main_with_logging(my_logger);
}
```

## Log Levels

- **Level 0**: Errors only (critical failures, API errors)
- **Level 1**: Communication steps (API calls, request processing)
- **Level 2**: Detailed information (request/response bodies, full debugging)

## Configuration

Create `config.json`:
```json
{
  "openai_api_key": "sk-your-api-key-here",
  "port": 3000
}
```

**Configuration Options**:
- `openai_api_key` (required): Your OpenAI API key
- `port` (optional, default: 3000): Port number for the HTTP server to listen on

## API Endpoints

- `POST /chat` - Send message to ChatGPT
- `GET /health` - Health check
- `OPTIONS /*` - CORS preflight

## Error Handling

All errors are handled without exceptions:
- Configuration errors: Returns empty string, logged at level 0
- JSON parse errors: Returns error response, logged at level 1
- API errors: Returns error response, logged at level 0
- Network errors: Retries with exponential backoff, logged at level 1

## Retry Logic

- **Rate limiting (HTTP 429)**: Retries up to 3 times with exponential backoff (2s, 4s, 8s)
- **Network errors**: Retries up to 3 times with linear delay (1s, 2s, 3s)
- **Other errors**: No retry, returns error immediately

