#ifndef LOGGER_H
#define LOGGER_H
#include <cstdio>
#include <cstdlib>
#include <stdarg.h>

// maximum 247 characters
// 256 at most for log buffer
// 8 characters for level
// 1 character for null teriminator
#define MSG_LEN 247

#define LOG_INFO(format, ...) Logger::log("[INFO] " format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) Logger::log("[DEBUG] " format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) Logger::log("[WARN] " format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) Logger::log("[ERROR] " format, ##__VA_ARGS__)
#define LOG_FATAL(format, ...) Logger::log_fatal("[FATAL] " format, ##__VA_ARGS__)

namespace Logger {
  inline void log(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
  }

  inline void log_fatal(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    exit(EXIT_FAILURE);
  }
}

#endif // LOGGER_H