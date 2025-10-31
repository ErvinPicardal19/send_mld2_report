#include <stdio.h>
#include <stdarg.h>

#include "logger.h"

level_t LOG_LEVEL = DEBUG;

static uint8_t is_valid_level(level_t lvl)
{
    return(lvl == DEBUG || lvl == INFO || lvl == WARN || lvl == ERROR);
}

void set_log_level(level_t level)
{
    if(is_valid_level(level))
    {
        LOG_LEVEL = level;
    }
}

void logger(level_t level, const char * __restrict fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    if(level <= LOG_LEVEL)
    {
        vfprintf(((level == ERROR) ? stderr : stdout), fmt, ap);
    }
    va_end(ap);
}
