// main.cpp
// ISO C++17 compliant, 32-bit Windows application
// UTF-8 encoded strings

#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <functional>
#include <cstdio>
#include <curl/curl.h>
#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

// Log callback function type
using LogCallback = void(*)(int log_level, const std::string& message);

// Global log callback (set before calling main_impl)
static LogCallback g_log_callback = nullptr;

// Internal log function
static void log(int level, const std::string& msg) {
    if (g_log_callback) {
        g_log_callback(level, msg);
    }
}

static void add_cors_headers(httplib::Response& res) {
  // For POC: allow all. In production, set an explicit origin.
  res.set_header("Access-Control-Allow-Origin", "*");
  res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
  res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

// Load configuration file (returns empty string on error)
static std::string load_config(const std::string& config_path) {
  std::ifstream file(config_path);
  if (!file.is_open()) {
    log(0, "Failed to open config file: " + config_path);
    return "";
  }

  json config;
  try {
    file >> config;
  } catch (...) {
    log(0, "Failed to parse config file: " + config_path);
    return "";
  }

  std::string api_key = config.value("openai_api_key", "");
  if (api_key.empty() || api_key == "your-api-key-here") {
    log(0, "OpenAI API key not configured in config.json");
    return "";
  }

  log(1, "Configuration loaded successfully");
  return api_key;
}

// Callback function for libcurl to write response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

// Function to call OpenAI API using libcurl with retry logic
static std::pair<int, std::string> call_openai_api(const std::string& api_key, const std::string& request_body) {
  const int max_retries = 3;
  const int retry_delay_ms = 1000;
  int retry_count = 0;
  
  log(1, "Calling OpenAI API...");
  log(2, "Request body: " + request_body);

  while (retry_count <= max_retries) {
    CURL* curl = nullptr;
    CURLcode res;
    std::string response_string;
    long response_code = 0;

    curl = curl_easy_init();
    if (!curl) {
      log(0, "Failed to initialize curl");
      json error_json = {{"error", "Failed to initialize curl"}};
      return std::make_pair(500, error_json.dump());
    }

    struct curl_slist* headers = nullptr;
    std::string auth_header = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth_header.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

    res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
      std::string error_msg = curl_easy_strerror(res);
      curl_slist_free_all(headers);
      curl_easy_cleanup(curl);
      log(0, "curl_easy_perform() failed: " + std::string(error_msg));
      
      // Retry on network errors
      if (retry_count < max_retries) {
        retry_count++;
        log(1, "Retrying request (attempt " + std::to_string(retry_count) + "/" + std::to_string(max_retries) + ")...");
        std::this_thread::sleep_for(std::chrono::milliseconds(retry_delay_ms * retry_count));
        continue;
      }
      
      json error_json = {{"error", "curl_easy_perform() failed: " + std::string(error_msg)}};
      return std::make_pair(502, error_json.dump());
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    log(1, "OpenAI API response status: " + std::to_string(response_code));
    log(2, "Response body: " + response_string);

    // Handle rate limiting (HTTP 429) with exponential backoff
    if (response_code == 429) {
      if (retry_count < max_retries) {
        retry_count++;
        int delay = retry_delay_ms * (1 << retry_count); // Exponential backoff: 2s, 4s, 8s
        log(1, "Rate limit encountered. Retrying after " + std::to_string(delay) + "ms (attempt " + std::to_string(retry_count) + "/" + std::to_string(max_retries) + ")...");
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        continue;
      }
    }

    // Return immediately for non-429 responses
    return std::make_pair(static_cast<int>(response_code), response_string);
  }

  // Should not reach here, but handle it
  json error_json = {{"error", "Max retries exceeded"}};
  return std::make_pair(429, error_json.dump());
}

// Main implementation (exception-free)
// Returns 0 on success, non-zero on error
static int main_impl() {
  // Initialize libcurl
  if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
    log(0, "Failed to initialize curl");
    return 1;
  }

  // Load configuration
  std::string openai_api_key = load_config("config.json");
  if (openai_api_key.empty()) {
    curl_global_cleanup();
    return 1;
  }

  httplib::Server svr;

  // Preflight handler
  svr.Options(R"(.*)", [](const httplib::Request&, httplib::Response& res) {
    add_cors_headers(res);
    res.status = 204;
  });

  // Health check
  svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
    add_cors_headers(res);
    res.set_content(R"({"ok":true})", "application/json");
  });

  // Chat endpoint
  svr.Post("/chat", [openai_api_key](const httplib::Request& req, httplib::Response& res) {
    add_cors_headers(res);

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
    
    // Call OpenAI API using libcurl (with retry logic)
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
  });

  log(1, "Server starting on http://0.0.0.0:8080");
  
  bool listen_success = svr.listen("0.0.0.0", 8080);
  
  // Cleanup libcurl
  curl_global_cleanup();
  
  if (!listen_success) {
    log(0, "Failed to start server on port 8080");
    return 1;
  }
  
  return 0;
}

// Main entry point (exception-free wrapper)
// Log callback must be set before calling this function
extern "C" int main_with_logging(LogCallback log_cb) {
  g_log_callback = log_cb;
  return main_impl();
}

// Standard main for standalone execution (with default logging to stderr)
int main() {
  // Default log callback that writes to stderr (for standalone execution)
  g_log_callback = [](int level, const std::string& msg) {
    if (level == 0) {  // Errors only
      std::fprintf(stderr, "[ERROR] %s\n", msg.c_str());
    } else if (level == 1) {  // Communication steps
      std::fprintf(stderr, "[INFO] %s\n", msg.c_str());
    } else {  // Detailed info
      std::fprintf(stderr, "[DEBUG] %s\n", msg.c_str());
    }
  };
  
  return main_impl();
}
