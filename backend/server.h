// server.h
// Main server implementation

#ifndef SERVER_H
#define SERVER_H

#include "config.h"

// Main server implementation (exception-free)
// Returns 0 on success, non-zero on error
int server_main();

// Start server with configuration
int server_main(const Config& config);

#endif // SERVER_H

