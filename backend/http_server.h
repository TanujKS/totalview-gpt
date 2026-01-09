// http_server.h
// HTTP server structures and request/response handling

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include <map>

// Forward declaration for socket type
#ifdef _WIN32
#include <winsock2.h>
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif

// HTTP request structure
struct HttpRequest {
  std::string method;
  std::string path;
  std::map<std::string, std::string> headers;
  std::string body;
};

// HTTP response structure
struct HttpResponse {
  int status = 200;
  std::map<std::string, std::string> headers;
  std::string body;
  
  void set_header(const std::string& key, const std::string& value);
  void set_content(const std::string& content, const std::string& content_type);
  void add_cors_headers();
  std::string to_string() const;
};

// Parse HTTP request from socket
bool parse_http_request(socket_t sock, HttpRequest& req);

// Send HTTP response to socket
bool send_http_response(socket_t sock, const HttpResponse& res);

#endif // HTTP_SERVER_H

