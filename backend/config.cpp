// config.cpp
// Configuration management implementation

#include "config.h"
#include "logger.h"
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;

// Load configuration file
bool load_config(const std::string& config_path, Config& config) {
  std::ifstream file(config_path);
  if (!file.is_open()) {
    log(0, "Failed to open config file: " + config_path);
    return false;
  }

  json config_json;
  try {
    file >> config_json;
  } catch (...) {
    log(0, "Failed to parse config file: " + config_path);
    return false;
  }

  config.openai_api_key = config_json.value("openai_api_key", "");
  if (config.openai_api_key.empty() || config.openai_api_key == "your-api-key-here") {
    log(0, "OpenAI API key not configured in config.json");
    return false;
  }

  // Get port (default to 3000 if not specified, or use 8080 if legacy config)
  config.port = config_json.value("port", config_json.value("server_port", 3000));
  
  // Validate port range
  if (config.port < 1 || config.port > 65535) {
    log(0, "Invalid port number: " + std::to_string(config.port) + ". Using default port 3000.");
    config.port = 3000;
  }

  log(1, "Configuration loaded successfully");
  log(1, "Server port: " + std::to_string(config.port));
  return true;
}

// Legacy function for backward compatibility (returns API key only)
std::string load_config(const std::string& config_path) {
  Config config;
  if (load_config(config_path, config)) {
    return config.openai_api_key;
  }
  return "";
}

