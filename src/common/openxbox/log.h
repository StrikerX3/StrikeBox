#pragma once

#include <stdio.h>

namespace openxbox {

#define LOG_LEVEL_FATAL   (1)
#define LOG_LEVEL_ERROR   (2)
#define LOG_LEVEL_WARNING (3)
#define LOG_LEVEL_INFO    (4)
#define LOG_LEVEL_DEBUG   (5)
#define LOG_LEVEL_SPEW    (6)

#define LOG_LEVEL LOG_LEVEL_SPEW

#if 1
#define log_fatal(...)   log_print(LOG_LEVEL_FATAL,   __VA_ARGS__)
#define log_error(...)   log_print(LOG_LEVEL_ERROR,   __VA_ARGS__)
#define log_warning(...) log_print(LOG_LEVEL_WARNING, __VA_ARGS__)
#define log_info(...)    log_print(LOG_LEVEL_INFO,    __VA_ARGS__)
#define log_debug(...)   log_print(LOG_LEVEL_DEBUG,   __VA_ARGS__)
#define log_spew(...)    log_print(LOG_LEVEL_SPEW,    __VA_ARGS__)
#else
#define log_fatal(...)
#define log_error(...)
#define log_warning(...)
#define log_info(...)
#define log_debug(...)
#define log_spew(...)
#endif

int log_print(int level, const char *fmt, ...);

}
