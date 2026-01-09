// logger.cpp
// Logging infrastructure implementation

#include "logger.h"

// Global log callback (set before calling main_impl)
static LogCallback g_log_callback = nullptr;

// Set the global log callback
void set_log_callback(LogCallback log_cb) {
    g_log_callback = log_cb;
}

// Internal log function
void log(int level, const std::string& msg) {
    if (g_log_callback) {
        g_log_callback(level, msg);
    }
}

