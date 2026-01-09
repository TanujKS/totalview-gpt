// main.h
// Header file for the backend server

#ifndef MAIN_H
#define MAIN_H

#include "logger.h"

// Main entry point with logging callback
// Returns 0 on success, non-zero on error
// This function does not throw exceptions
extern "C" int main_with_logging(LogCallback log_cb);

#endif // MAIN_H

