// main.cpp
// ISO C++17 compliant, 32-bit Windows application
// UTF-8 encoded strings
// Main entry point for the backend server

#include "logger.h"
#include "server.h"
#include <cstdio>
#include <string>


// Main entry point (exception-free wrapper)
// Log callback must be set before calling this function
extern "C" int main_with_logging(LogCallback log_cb) {
  set_log_callback(log_cb);
  return server_main();
}

// Standard main for standalone execution (with default logging to stderr)
int main() {
  // Default log callback that writes to stderr (for standalone execution)
  set_log_callback([](int level, const std::string& msg) {
    if (level == 0) {  // Errors only
      std::fprintf(stderr, "[ERROR] %s\n", msg.c_str());
    } else if (level == 1) {  // Communication steps
      std::fprintf(stderr, "[INFO] %s\n", msg.c_str());
    } else {  // Detailed info
      std::fprintf(stderr, "[DEBUG] %s\n", msg.c_str());
    }
  });
  
  return server_main();
}
