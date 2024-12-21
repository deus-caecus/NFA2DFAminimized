#pragma once

#define CURRENT_DEBUG_LEVEL 4
// Debug levels
#define DEBUG_LEVEL_NONE 0
#define DEBUG_LEVEL_ERROR 1
#define DEBUG_LEVEL_WARNING 2
#define DEBUG_LEVEL_INFO 3
#define DEBUG_LEVEL_DEBUG 4

#include <iostream>

// Helper macro to check if the current debug level is sufficient for logging
#define LOG_IF_ENABLED(level, ...) \
    do { \
        if (level <= CURRENT_DEBUG_LEVEL) { \
            std::cout << __VA_ARGS__ << std::endl; \
        } \
    } while (0)

// Define macros for different log levels
#define ERROR(...)   LOG_IF_ENABLED(DEBUG_LEVEL_ERROR, "ERROR: " __VA_ARGS__)
#define WARNING(...) LOG_IF_ENABLED(DEBUG_LEVEL_WARNING, "WARNING: " __VA_ARGS__)
#define INFO(...)    LOG_IF_ENABLED(DEBUG_LEVEL_INFO, "INFO: " __VA_ARGS__)
#define DEBUG(...)   LOG_IF_ENABLED(DEBUG_LEVEL_DEBUG, "DEBUG: " __VA_ARGS__)

#ifdef NDEBUG // If NDEBUG is defined, it's a release build
#define DEBUG(...)
#define INFO(...)
#else // Debug build
#define DEBUG(...)   LOG_IF_ENABLED(DEBUG_LEVEL_DEBUG, "DEBUG: " __VA_ARGS__)
#define INFO(...)    LOG_IF_ENABLED(DEBUG_LEVEL_INFO, "INFO: " __VA_ARGS__)
#endif
