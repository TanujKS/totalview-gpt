// main.h
// Header file for the backend server

#ifndef MAIN_H
#define MAIN_H

#include <string>

// Log callback function type
// log_level: 0 (errors only), 1 (communication steps), 2 (detailed request/response)
using LogCallback = void(*)(int log_level, const std::string& message);

// Main entry point with logging callback
// Returns 0 on success, non-zero on error
// This function does not throw exceptions
extern "C" int main_with_logging(LogCallback log_cb);

#endif // MAIN_H

