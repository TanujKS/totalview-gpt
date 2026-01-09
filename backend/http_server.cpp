// http_server.cpp
// HTTP server implementation

#include "http_server.h"
#include "utils.h"
#include <vector>
#include <sstream>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

// Parse HTTP request from socket
bool parse_http_request(socket_t sock, HttpRequest& req) {
  char buffer[8192];
  std::string request_data;
  std::string all_data;
  int total_read = 0;
  size_t header_end_pos = std::string::npos;
  
  // Read request line and headers (up to double CRLF)
  while (total_read < sizeof(buffer) - 1) {
    int bytes_read = recv(sock, buffer + total_read, sizeof(buffer) - 1 - total_read, 0);
    if (bytes_read <= 0) {
      return false;
    }
    total_read += bytes_read;
    buffer[total_read] = '\0';
    
    // Check for end of headers (double CRLF)
    all_data = std::string(buffer, total_read);
    header_end_pos = all_data.find("\r\n\r\n");
    if (header_end_pos != std::string::npos) {
      request_data = all_data.substr(0, header_end_pos);
      break;
    }
  }
  
  if (request_data.empty() || header_end_pos == std::string::npos) {
    return false;
  }
  
  // Parse request line
  size_t first_line_end = request_data.find("\r\n");
  if (first_line_end == std::string::npos) {
    return false;
  }
  
  std::string request_line = request_data.substr(0, first_line_end);
  size_t method_end = request_line.find(' ');
  if (method_end == std::string::npos) {
    return false;
  }
  
  req.method = request_line.substr(0, method_end);
  size_t path_start = method_end + 1;
  size_t path_end = request_line.find(' ', path_start);
  if (path_end == std::string::npos) {
    return false;
  }
  req.path = request_line.substr(path_start, path_end - path_start);
  
  // Parse headers
  int content_length = -1;
  size_t header_start = first_line_end + 2;
  while (header_start < request_data.size()) {
    size_t header_line_end = request_data.find("\r\n", header_start);
    if (header_line_end == std::string::npos) {
      break;
    }
    std::string header_line = request_data.substr(header_start, header_line_end - header_start);
    size_t colon_pos = header_line.find(':');
    if (colon_pos != std::string::npos) {
      std::string key = header_line.substr(0, colon_pos);
      std::string value = header_line.substr(colon_pos + 1);
      // Trim whitespace from value
      size_t value_start = value.find_first_not_of(" \t");
      if (value_start != std::string::npos) {
        value = value.substr(value_start);
      }
      req.headers[key] = value;
      
      // Check for Content-Length (case-insensitive)
      if (to_lower(key) == "content-length") {
        try {
          content_length = std::stoi(value);
        } catch (...) {
          content_length = -1;
        }
      }
    }
    header_start = header_line_end + 2;
  }
  
  // Read body if Content-Length is specified
  if (content_length > 0 && content_length <= 200 * 1024) {
    size_t body_start_pos = header_end_pos + 4;
    int body_already_read = total_read - (int)body_start_pos;
    
    if (body_already_read > 0 && body_start_pos < all_data.size()) {
      req.body = all_data.substr(body_start_pos, body_already_read);
    }
    
    // Read remaining body data if needed
    while ((int)req.body.size() < content_length) {
      int remaining = content_length - (int)req.body.size();
      if (remaining <= 0) break;
      
      std::vector<char> body_buffer(remaining);
      int body_bytes = recv(sock, body_buffer.data(), remaining, 0);
      if (body_bytes <= 0) {
        break;
      }
      req.body.append(body_buffer.data(), body_bytes);
    }
  }
  
  return true;
}

// Send HTTP response to socket
bool send_http_response(socket_t sock, const HttpResponse& res) {
  std::string response_str = res.to_string();
  int sent = 0;
  while (sent < (int)response_str.size()) {
    int bytes_sent = send(sock, response_str.c_str() + sent, response_str.size() - sent, 0);
    if (bytes_sent <= 0) {
      return false;
    }
    sent += bytes_sent;
  }
  return true;
}

// HttpResponse methods implementation
void HttpResponse::set_header(const std::string& key, const std::string& value) {
  headers[key] = value;
}

void HttpResponse::set_content(const std::string& content, const std::string& content_type) {
  body = content;
  set_header("Content-Type", content_type);
  set_header("Content-Length", std::to_string(body.size()));
}

void HttpResponse::add_cors_headers() {
  set_header("Access-Control-Allow-Origin", "*");
  set_header("Access-Control-Allow-Methods", "POST, OPTIONS, GET");
  set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

std::string HttpResponse::to_string() const {
  std::stringstream ss;
  ss << "HTTP/1.1 " << status << " ";
  switch (status) {
    case 200: ss << "OK"; break;
    case 204: ss << "No Content"; break;
    case 400: ss << "Bad Request"; break;
    case 413: ss << "Payload Too Large"; break;
    case 502: ss << "Bad Gateway"; break;
    case 404: ss << "Not Found"; break;
    default: ss << "OK"; break;
  }
  ss << "\r\n";
  
  for (const auto& h : headers) {
    ss << h.first << ": " << h.second << "\r\n";
  }
  ss << "\r\n";
  ss << body;
  return ss.str();
}

