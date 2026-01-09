// openai_client.h
// OpenAI API client implementation

#ifndef OPENAI_CLIENT_H
#define OPENAI_CLIENT_H

#include <string>
#include <utility>

// Call OpenAI API using WinHTTP with retry logic
// Returns pair of (status_code, response_body)
std::pair<int, std::string> call_openai_api(const std::string& api_key, const std::string& request_body);

#endif // OPENAI_CLIENT_H

