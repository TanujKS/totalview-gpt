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

The backend currently relies on three external libraries:

### 1. libcurl (`libcurl`)
**Purpose**: HTTP/HTTPS client for communicating with OpenAI API

**Usage**:
- Makes HTTPS POST requests to `https://api.openai.com/v1/chat/completions`
- Handles SSL/TLS encryption
- Manages HTTP headers and request/response data

**Key Functions Used**:
- `curl_global_init()` - Initialize curl library
- `curl_easy_init()` - Create curl handle
- `curl_easy_setopt()` - Configure request options
- `curl_easy_perform()` - Execute HTTP request
- `curl_easy_getinfo()` - Get response status code
- `curl_easy_cleanup()` - Cleanup resources

**Why it's needed**: Secure HTTPS communication with proper SSL certificate validation and HTTP protocol handling.

### 2. httplib (`httplib.h`)
**Purpose**: Simple HTTP server for handling incoming requests

**Usage**:
- Creates HTTP server listening on port 8080
- Handles POST requests to `/chat` endpoint
- Manages CORS headers
- Processes request/response bodies

**Key Features Used**:
- `httplib::Server` - HTTP server instance
- `svr.Post()` - Register POST route handler
- `svr.Get()` - Register GET route handler (health check)
- `svr.Options()` - Register OPTIONS handler (CORS preflight)
- `httplib::Request` - Request object with body, headers
- `httplib::Response` - Response object for setting status, content, headers

**Why it's needed**: Provides a simple, header-only HTTP server without requiring additional runtime dependencies.

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

## Building Without External Libraries

If you need to build this without external dependencies in the future, here's what would need to be replaced:

### Option 1: Replace libcurl

**Alternatives**:
1. **Windows**: Use WinHTTP API (`winhttp.h`)
   ```cpp
   #include <winhttp.h>
   // Use WinHttpOpen, WinHttpConnect, WinHttpOpenRequest, etc.
   ```

2. **Linux**: Use native socket APIs
   - Create socket with `socket()`
   - Connect with `connect()`
   - Send/Receive with `send()/recv()`
   - Implement SSL/TLS with OpenSSL or similar

3. **Cross-platform**: Implement minimal HTTP client
   - Use platform-specific socket APIs
   - Implement basic HTTP/1.1 protocol
   - Add SSL/TLS support (OpenSSL, mbedTLS, or similar)

**Complexity**: High - requires implementing HTTP protocol, SSL/TLS, and certificate validation.

### Option 2: Replace httplib

**Alternatives**:
1. **Windows**: Use `httpapi.h` (HTTP Server API)
   ```cpp
   #include <http.h>
   // Use HttpCreateServerSession, HttpCreateRequestQueue, etc.
   ```

2. **Linux**: Use native socket APIs
   - Create server socket with `socket()` and `bind()`
   - Listen with `listen()` and `accept()`
   - Implement HTTP/1.1 protocol parsing
   - Handle request routing manually

3. **Cross-platform**: Implement minimal HTTP server
   - Use platform-specific socket APIs
   - Parse HTTP request headers and body
   - Generate HTTP response with proper headers
   - Handle routing and CORS

**Complexity**: Medium - requires HTTP protocol implementation, but simpler than HTTP client.

### Option 3: Replace nlohmann/json

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

### Linux Build

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install libcurl4-openssl-dev

# Build
chmod +x build.sh
./build.sh

# Run
./llm_poc
```

### Windows 32-bit Build

```cmd
REM Install MinGW-w64 with libcurl support
REM Or use MSVC with curl library

REM Build
build_windows.bat

REM Run
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
  "openai_api_key": "sk-your-api-key-here"
}
```

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

