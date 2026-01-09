// config.h
// Configuration management

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

// Configuration structure
struct Config {
  std::string openai_api_key;
  int port;
  
  Config() : port(3000) {} // Default port 3000
};

// Load configuration file
// Returns true on success, false on error
bool load_config(const std::string& config_path, Config& config);

// Legacy function for backward compatibility (returns API key only)
std::string load_config(const std::string& config_path);

#endif // CONFIG_H

