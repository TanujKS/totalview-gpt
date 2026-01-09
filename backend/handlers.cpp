// handlers.cpp
// HTTP endpoint handlers implementation

#include "handlers.h"
#include "logger.h"
#include "openai_client.h"
#include "json.hpp"

using json = nlohmann::json;

// Handle chat endpoint (POST /chat)
void handle_chat_endpoint(const HttpRequest& req, HttpResponse& res, const std::string& openai_api_key) {
  res.add_cors_headers();

  // Basic size guard
  if (req.body.size() > 200 * 1024) {
    log(1, "Request payload too large: " + std::to_string(req.body.size()) + " bytes");
    res.status = 413;
    res.set_content(R"({"error":"payload too large"})", "application/json");
    return;
  }

  json body;
  bool parse_success = false;
  try {
    body = json::parse(req.body);
    parse_success = true;
  } catch (...) {
    // Handle parsing errors without exceptions
  }

  if (!parse_success) {
    log(1, "JSON parse error in request body");
    res.status = 400;
    res.set_content(R"({"error":"invalid JSON"})", "application/json");
    return;
  }

  const std::string user_message = body.value("user_message", "");
  const std::string system_message = body.value("system_message", "");
  const json context = body.value("context", json::object());

  // Validate that we have at least a user message
  if (user_message.empty()) {
    log(1, "Request missing required field: user_message");
    res.status = 400;
    res.set_content(R"({"error":"user_message is required"})", "application/json");
    return;
  }

  log(1, "Processing chat request - user_message length: " + std::to_string(user_message.length()));

  // Build messages array for OpenAI
  json messages = json::array();
  
  // Add system message first if provided
  if (!system_message.empty()) {
    messages.push_back({
      {"role", "system"},
      {"content", system_message}
    });
  }
  
  // Add context messages if provided (conversation history)
  if (context.contains("messages") && context["messages"].is_array()) {
    for (const auto& msg : context["messages"]) {
      messages.push_back(msg);
    }
  }
  
  // Add current user message
  if (!user_message.empty()) {
    messages.push_back({
      {"role", "user"},
      {"content", user_message}
    });
  }

  // Prepare OpenAI request
  json openai_request = {
    {"model", "gpt-3.5-turbo"},
    {"messages", messages}
  };

  std::string request_body = openai_request.dump();
  
  // Call OpenAI API using WinHTTP (with retry logic)
  auto api_result = call_openai_api(openai_api_key, request_body);
  int status_code = api_result.first;
  std::string response_body = api_result.second;

  if (status_code != 200) {
    log(0, "OpenAI API error - status: " + std::to_string(status_code));
    res.status = 502;
    json error_out;
    bool parse_error_success = false;
    try {
      error_out = json::parse(response_body);
      parse_error_success = true;
    } catch (...) {
      // Handle parsing errors
    }
    
    if (!parse_error_success) {
      error_out = {
        {"error", "OpenAI API error"},
        {"status", status_code},
        {"details", response_body}
      };
    }
    res.set_content(error_out.dump(), "application/json");
    return;
  }

  // Parse OpenAI response
  json openai_response;
  bool parse_response_success = false;
  try {
    openai_response = json::parse(response_body);
    parse_response_success = true;
  } catch (...) {
    // Handle parsing errors
  }

  if (!parse_response_success) {
    log(0, "Failed to parse OpenAI response");
    res.status = 502;
    res.set_content(R"({"error":"failed to parse OpenAI response"})", "application/json");
    return;
  }

  // Extract assistant message
  std::string assistant = "";
  if (openai_response.contains("choices") && 
      openai_response["choices"].is_array() && 
      !openai_response["choices"].empty()) {
    const auto& choice = openai_response["choices"][0];
    if (choice.contains("message") && 
        choice["message"].contains("content")) {
      assistant = choice["message"]["content"].get<std::string>();
    }
  }

  if (assistant.empty()) {
    log(0, "Empty response from OpenAI");
    res.status = 502;
    res.set_content(R"({"error":"empty response from OpenAI"})", "application/json");
    return;
  }

  log(1, "Successfully received response from OpenAI - message length: " + std::to_string(assistant.length()));

  json out = {
    {"conversation_id", body.value("conversation_id", "")},
    {"assistant_message", assistant}
  };

  res.set_content(out.dump(), "application/json");
}

// Handle health check endpoint (GET /health)
void handle_health_endpoint(const HttpRequest& req, HttpResponse& res) {
  res.add_cors_headers();
  res.set_content(R"({"ok":true})", "application/json");
}

// Handle CORS preflight (OPTIONS *)
void handle_options_endpoint(const HttpRequest& req, HttpResponse& res) {
  res.add_cors_headers();
  res.status = 204;
}

// Handle 404 Not Found
void handle_not_found(const HttpRequest& req, HttpResponse& res) {
  res.add_cors_headers();
  res.status = 404;
  res.set_content(R"({"error":"not found"})", "application/json");
}

