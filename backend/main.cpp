// main.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <curl/curl.h>
#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

static void add_cors_headers(httplib::Response& res) {
  // For POC: allow all. In production, set an explicit origin.
  res.set_header("Access-Control-Allow-Origin", "*");
  res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
  res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

static std::string load_config(const std::string& config_path) {
  std::ifstream file(config_path);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open config file: " + config_path);
  }

  json config;
  try {
    file >> config;
  } catch (...) {
    throw std::runtime_error("Failed to parse config file");
  }

  std::string api_key = config.value("openai_api_key", "");
  if (api_key.empty() || api_key == "your-api-key-here") {
    throw std::runtime_error("OpenAI API key not configured in config.json");
  }

  return api_key;
}

// Callback function for libcurl to write response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

// Function to call OpenAI API using libcurl
static std::pair<int, std::string> call_openai_api(const std::string& api_key, const std::string& request_body) {
  CURL* curl;
  CURLcode res;
  std::string response_string;
  long response_code = 0;

  curl = curl_easy_init();
  if (!curl) {
    return std::make_pair(500, R"({"error":"Failed to initialize curl"})");
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
    json error_json = {{"error", "curl_easy_perform() failed: " + std::string(error_msg)}};
    return std::make_pair(502, error_json.dump());
  }

  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

  return std::make_pair(static_cast<int>(response_code), response_string);
}

int main() {
  // Initialize libcurl
  curl_global_init(CURL_GLOBAL_DEFAULT);

  // Load configuration
  std::string openai_api_key;
  try {
    openai_api_key = load_config("config.json");
  } catch (const std::exception& e) {
    std::cerr << "Error loading config: " << e.what() << std::endl;
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
    try {
      add_cors_headers(res);

      // Basic size guard for POC
      if (req.body.size() > 200 * 1024) {
        res.status = 413;
        res.set_content(R"({"error":"payload too large"})", "application/json");
        return;
      }

      json body;
      try {
        body = json::parse(req.body);
      } catch (const std::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        res.status = 400;
        res.set_content(R"({"error":"invalid JSON"})", "application/json");
        return;
      } catch (...) {
        std::cerr << "Unknown JSON parse error" << std::endl;
        res.status = 400;
        res.set_content(R"({"error":"invalid JSON"})", "application/json");
        return;
      }

      const std::string user_message = body.value("user_message", "");
      const std::string system_message = body.value("system_message", "");
      const json context = body.value("context", json::object());

      // Validate that we have at least a user message
      if (user_message.empty()) {
        res.status = 400;
        res.set_content(R"({"error":"user_message is required"})", "application/json");
        return;
      }

      std::cerr << "Processing chat request - user_message length: " << user_message.length() << std::endl;

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
      std::cerr << "Sending request to OpenAI API..." << std::endl;
      
      // Call OpenAI API using libcurl
      auto [status_code, response_body] = call_openai_api(openai_api_key, request_body);
      
      std::cerr << "OpenAI API response status: " << status_code << std::endl;

      if (status_code != 200) {
        std::cerr << "OpenAI API error response: " << response_body << std::endl;
        res.status = 502;
        json error_out;
        try {
          error_out = json::parse(response_body);
        } catch (...) {
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
      try {
        openai_response = json::parse(response_body);
      } catch (const std::exception& e) {
        std::cerr << "Failed to parse OpenAI response: " << e.what() << std::endl;
        res.status = 502;
        res.set_content(R"({"error":"failed to parse OpenAI response"})", "application/json");
        return;
      } catch (...) {
        std::cerr << "Unknown error parsing OpenAI response" << std::endl;
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
        std::cerr << "Empty response from OpenAI" << std::endl;
        res.status = 502;
        res.set_content(R"({"error":"empty response from OpenAI"})", "application/json");
        return;
      }

      std::cerr << "Successfully received response from OpenAI" << std::endl;

      json out = {
        {"conversation_id", body.value("conversation_id", "")},
        {"assistant_message", assistant}
      };

      res.set_content(out.dump(), "application/json");
    } catch (const std::exception& e) {
      std::cerr << "Exception in /chat handler: " << e.what() << std::endl;
      res.status = 500;
      json error_out = {
        {"error", "Internal server error"},
        {"details", e.what()}
      };
      res.set_content(error_out.dump(), "application/json");
    } catch (...) {
      std::cerr << "Unknown exception in /chat handler" << std::endl;
      res.status = 500;
      res.set_content(R"({"error":"Internal server error"})", "application/json");
    }
  });

  std::cout << "C++ POC server listening on http://0.0.0.0:8080\n";
  svr.listen("0.0.0.0", 8080);
  
  // Cleanup libcurl
  curl_global_cleanup();
  
  return 0;
}
