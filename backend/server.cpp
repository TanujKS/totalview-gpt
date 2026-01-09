// server.cpp
// Main server implementation

#include "server.h"
#include "logger.h"
#include "config.h"
#include "http_server.h"
#include "handlers.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#include <arpa/inet.h>
#define closesocket close
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
typedef int SOCKET;
#endif

// Main server implementation (exception-free)
// Returns 0 on success, non-zero on error
int server_main() {
  // Load configuration
  Config config;
  if (!load_config("config.json", config)) {
    return 1;
  }
  
  return server_main(config);
}

// Start server with configuration
int server_main(const Config& config) {
  std::string openai_api_key = config.openai_api_key;
  int port = config.port;

  // Initialize Winsock
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    log(0, "Failed to initialize Winsock");
    return 1;
  }

  // Create socket
  SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (listen_socket == INVALID_SOCKET) {
    DWORD error = WSAGetLastError();
    log(0, "Failed to create socket (error: " + std::to_string(error) + ")");
    WSACleanup();
    return 1;
  }

  // Set socket options for address reuse
  int opt = 1;
  if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
    DWORD error = WSAGetLastError();
    log(0, "Failed to set socket option SO_REUSEADDR (error: " + std::to_string(error) + ")");
    closesocket(listen_socket);
    WSACleanup();
    return 1;
  }

  // Bind socket
  sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  if (bind(listen_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
    DWORD error = WSAGetLastError();
    std::string error_msg = "Failed to bind socket to port " + std::to_string(port) + " (error: " + std::to_string(error) + ")";
    
    // Provide helpful error messages for common errors
    if (error == WSAEADDRINUSE) {
      error_msg += " - Port " + std::to_string(port) + " is already in use. Please close any other applications using this port or choose a different port in config.json.";
    } else if (error == WSAEACCES) {
      error_msg += " - Access denied. This may be due to:";
      error_msg += "\n  1. Another process is using port " + std::to_string(port);
      error_msg += "\n  2. Windows Firewall or antivirus blocking the port";
      error_msg += "\n  3. Port is in a reserved range";
      error_msg += "\n  Try using a different port (e.g., 3000, 8888) by adding \"port\": 3000 to config.json";
    } else if (error == WSAEINVAL) {
      error_msg += " - Invalid address or socket already bound.";
    }
    
    log(0, error_msg);
    closesocket(listen_socket);
    WSACleanup();
    return 1;
  }

  // Listen
  if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
    DWORD error = WSAGetLastError();
    log(0, "Failed to listen on socket (error: " + std::to_string(error) + ")");
    closesocket(listen_socket);
    WSACleanup();
    return 1;
  }

  log(1, "Server starting on http://0.0.0.0:" + std::to_string(port));

  // Accept connections loop
  while (true) {
    sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    SOCKET client_socket = accept(listen_socket, (sockaddr*)&client_addr, &client_addr_len);
    
    if (client_socket == INVALID_SOCKET) {
      log(0, "Failed to accept connection");
      continue;
    }

    // Handle request in current thread (simple blocking server)
    HttpRequest req;
    if (parse_http_request(client_socket, req)) {
      HttpResponse res;
      
      // Route requests
      if (req.method == "OPTIONS") {
        // CORS preflight
        handle_options_endpoint(req, res);
      } else if (req.method == "GET" && req.path == "/health") {
        // Health check
        handle_health_endpoint(req, res);
      } else if (req.method == "POST" && req.path == "/chat") {
        // Chat endpoint
        handle_chat_endpoint(req, res, openai_api_key);
      } else {
        // 404 Not Found
        handle_not_found(req, res);
      }
      
      send_http_response(client_socket, res);
    }
    
    closesocket(client_socket);
  }

  // Cleanup (should never reach here, but included for completeness)
  closesocket(listen_socket);
  WSACleanup();
  return 0;
}

