// handlers.h
// HTTP endpoint handlers

#ifndef HANDLERS_H
#define HANDLERS_H

#include "http_server.h"
#include <string>

// Handle chat endpoint (POST /chat)
void handle_chat_endpoint(const HttpRequest& req, HttpResponse& res, const std::string& openai_api_key);

// Handle health check endpoint (GET /health)
void handle_health_endpoint(const HttpRequest& req, HttpResponse& res);

// Handle CORS preflight (OPTIONS *)
void handle_options_endpoint(const HttpRequest& req, HttpResponse& res);

// Handle 404 Not Found
void handle_not_found(const HttpRequest& req, HttpResponse& res);

#endif // HANDLERS_H

