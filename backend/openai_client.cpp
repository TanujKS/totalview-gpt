// openai_client.cpp
// OpenAI API client implementation

#include "openai_client.h"
#include "logger.h"
#include "utils.h"
#include "json.hpp"
#include <windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

using json = nlohmann::json;

// Function to call OpenAI API using WinHTTP with retry logic
std::pair<int, std::string> call_openai_api(const std::string& api_key, const std::string& request_body) {
  const int max_retries = 3;
  const int retry_delay_ms = 1000;
  int retry_count = 0;
  
  log(1, "Calling OpenAI API...");
  log(2, "Request body: " + request_body);

  while (retry_count <= max_retries) {
    HINTERNET hSession = nullptr;
    HINTERNET hConnect = nullptr;
    HINTERNET hRequest = nullptr;
    DWORD dwStatusCode = 0;
    DWORD dwStatusCodeSize = sizeof(dwStatusCode);
    std::string response_string;
    bool success = false;

    // Initialize WinHTTP session
    hSession = WinHttpOpen(L"TotalView-GPT/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
                           WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
      log(0, "Failed to initialize WinHTTP session");
      json error_json = {{"error", "Failed to initialize WinHTTP session"}};
      return std::make_pair(500, error_json.dump());
    }

    // Set timeout to 60 seconds (60000 milliseconds)
    DWORD timeout = 60000;
    WinHttpSetTimeouts(hSession, timeout, timeout, timeout, timeout);
    WinHttpSetOption(hSession, WINHTTP_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));

    // Connect to host
    std::wstring host_w = utf8_to_wstring("api.openai.com");
    hConnect = WinHttpConnect(hSession, host_w.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
      DWORD error = GetLastError();
      WinHttpCloseHandle(hSession);
      log(0, "Failed to connect to api.openai.com (error: " + std::to_string(error) + ")");
      
      // Retry on network errors
      if (retry_count < max_retries) {
        retry_count++;
        log(1, "Retrying request (attempt " + std::to_string(retry_count) + "/" + std::to_string(max_retries) + ")...");
        Sleep(retry_delay_ms * retry_count);
        continue;
      }
      
      json error_json = {{"error", "Failed to connect to api.openai.com"}};
      return std::make_pair(502, error_json.dump());
    }

    // Create request
    std::wstring path_w = utf8_to_wstring("/v1/chat/completions");
    hRequest = WinHttpOpenRequest(hConnect, L"POST", path_w.c_str(), NULL, 
                                   WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 
                                   WINHTTP_FLAG_SECURE);
    if (!hRequest) {
      WinHttpCloseHandle(hConnect);
      WinHttpCloseHandle(hSession);
      log(0, "Failed to create WinHTTP request");
      
      if (retry_count < max_retries) {
        retry_count++;
        log(1, "Retrying request (attempt " + std::to_string(retry_count) + "/" + std::to_string(max_retries) + ")...");
        Sleep(retry_delay_ms * retry_count);
        continue;
      }
      
      json error_json = {{"error", "Failed to create WinHTTP request"}};
      return std::make_pair(502, error_json.dump());
    }

    // Prepare headers - each header must end with \r\n
    std::string headers_str = "Content-Type: application/json\r\n";
    headers_str += "Authorization: Bearer " + api_key + "\r\n";
    std::wstring headers_w = utf8_to_wstring(headers_str);
    
    // Add headers
    if (!WinHttpAddRequestHeaders(hRequest, headers_w.c_str(), -1, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE)) {
      WinHttpCloseHandle(hRequest);
      WinHttpCloseHandle(hConnect);
      WinHttpCloseHandle(hSession);
      log(0, "Failed to add request headers");
      
      if (retry_count < max_retries) {
        retry_count++;
        log(1, "Retrying request (attempt " + std::to_string(retry_count) + "/" + std::to_string(max_retries) + ")...");
        Sleep(retry_delay_ms * retry_count);
        continue;
      }
      
      json error_json = {{"error", "Failed to add request headers"}};
      return std::make_pair(502, error_json.dump());
    }

    // Send request with body
    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, 
                            (LPVOID)request_body.c_str(), (DWORD)request_body.size(), 
                            (DWORD)request_body.size(), 0)) {
      DWORD error = GetLastError();
      WinHttpCloseHandle(hRequest);
      WinHttpCloseHandle(hConnect);
      WinHttpCloseHandle(hSession);
      log(0, "WinHttpSendRequest() failed (error: " + std::to_string(error) + ")");
      
      if (retry_count < max_retries) {
        retry_count++;
        log(1, "Retrying request (attempt " + std::to_string(retry_count) + "/" + std::to_string(max_retries) + ")...");
        Sleep(retry_delay_ms * retry_count);
        continue;
      }
      
      json error_json = {{"error", "WinHttpSendRequest() failed"}};
      return std::make_pair(502, error_json.dump());
    }

    // Receive response
    if (!WinHttpReceiveResponse(hRequest, NULL)) {
      DWORD error = GetLastError();
      WinHttpCloseHandle(hRequest);
      WinHttpCloseHandle(hConnect);
      WinHttpCloseHandle(hSession);
      log(0, "WinHttpReceiveResponse() failed (error: " + std::to_string(error) + ")");
      
      if (retry_count < max_retries) {
        retry_count++;
        log(1, "Retrying request (attempt " + std::to_string(retry_count) + "/" + std::to_string(max_retries) + ")...");
        Sleep(retry_delay_ms * retry_count);
        continue;
      }
      
      json error_json = {{"error", "WinHttpReceiveResponse() failed"}};
      return std::make_pair(502, error_json.dump());
    }

    // Get status code
    if (!WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, 
                             WINHTTP_HEADER_NAME_BY_INDEX, &dwStatusCode, &dwStatusCodeSize, 
                             WINHTTP_NO_HEADER_INDEX)) {
      WinHttpCloseHandle(hRequest);
      WinHttpCloseHandle(hConnect);
      WinHttpCloseHandle(hSession);
      log(0, "Failed to get response status code");
      
      if (retry_count < max_retries) {
        retry_count++;
        log(1, "Retrying request (attempt " + std::to_string(retry_count) + "/" + std::to_string(max_retries) + ")...");
        Sleep(retry_delay_ms * retry_count);
        continue;
      }
      
      json error_json = {{"error", "Failed to get response status code"}};
      return std::make_pair(502, error_json.dump());
    }

    // Read response body
    DWORD dwBytesAvailable = 0;
    DWORD dwBytesRead = 0;
    char buffer[8192];

    do {
      // Query available data
      if (!WinHttpQueryDataAvailable(hRequest, &dwBytesAvailable)) {
        break; // No more data or error
      }
      
      if (dwBytesAvailable == 0) {
        break; // No more data
      }
      
      // Read data (read available bytes or buffer size, whichever is smaller)
      DWORD bytes_to_read = (dwBytesAvailable < sizeof(buffer)) ? dwBytesAvailable : sizeof(buffer);
      if (!WinHttpReadData(hRequest, buffer, bytes_to_read, &dwBytesRead)) {
        break; // Error reading data
      }
      
      // Append read data to response string
      if (dwBytesRead > 0) {
        response_string.append(buffer, dwBytesRead);
      }
    } while (dwBytesAvailable > 0);

    // Cleanup
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    log(1, "OpenAI API response status: " + std::to_string(dwStatusCode));
    log(2, "Response body: " + response_string);

    // Handle rate limiting (HTTP 429) with exponential backoff
    if (dwStatusCode == 429) {
      if (retry_count < max_retries) {
        retry_count++;
        DWORD delay = retry_delay_ms * (1 << retry_count); // Exponential backoff: 2s, 4s, 8s
        log(1, "Rate limit encountered. Retrying after " + std::to_string(delay) + "ms (attempt " + std::to_string(retry_count) + "/" + std::to_string(max_retries) + ")...");
        Sleep(delay);
        continue;
      }
    }

    // Return immediately for non-429 responses
    return std::make_pair(static_cast<int>(dwStatusCode), response_string);
  }

  // Should not reach here, but handle it
  json error_json = {{"error", "Max retries exceeded"}};
  return std::make_pair(429, error_json.dump());
}

