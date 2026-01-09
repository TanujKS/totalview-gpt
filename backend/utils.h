// utils.h
// Utility functions for string conversion and manipulation

#ifndef UTILS_H
#define UTILS_H

#include <string>

// Convert UTF-8 string to wide string (Windows)
std::wstring utf8_to_wstring(const std::string& str);

// Convert wide string to UTF-8 string (Windows)
std::string wstring_to_utf8(const std::wstring& wstr);

// Convert string to lowercase
std::string to_lower(const std::string& str);

#endif // UTILS_H

