#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdint.h>

typedef enum
{
    DEBUG = 0,
    INFO,
    WARN,
    ERROR,
} level_t;

void set_log_level(level_t level);
void logger(level_t level, const char * __restrict fmt, ...);

#define LOG_ERROR(fmt, ...) logger(ERROR, "\033[31m[ERROR]\033[0m" fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_WARN(fmt, ...) logger(WARN, "\033[33m[WARN]\033[0m" fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_INFO(fmt, ...) logger(INFO, "[INFO]" fmt __VA_OPT__(,) __VA_ARGS__)
#define LOG_DEBUG(fmt, ...) logger(DEBUG, "[DEBUG]" fmt __VA_OPT__(,) __VA_ARGS__)

#define LOG(level, fmt, ...) do\
{\
    LOG_##level("[%s][%s:%d] " fmt "\n", __FILE__, __func__, __LINE__ __VA_OPT__(,) __VA_ARGS__);\
} while(0)

#endif//__LOGGER_H__
