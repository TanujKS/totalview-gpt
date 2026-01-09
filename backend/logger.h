// logger.h
// Logging infrastructure for the backend server

#ifndef LOGGER_H
#define LOGGER_H

#include <string>

// Log callback function type
// log_level: 0 (errors only), 1 (communication steps), 2 (detailed request/response)
using LogCallback = void(*)(int log_level, const std::string& message);

// Set the global log callback
void set_log_callback(LogCallback log_cb);

// Internal log function (used throughout the codebase)
void log(int level, const std::string& msg);

#endif // LOGGER_H

