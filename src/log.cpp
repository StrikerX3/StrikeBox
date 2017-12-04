#include <stdarg.h>
#include "log.h"

/*!
 * Print a message to the log
 */
int log_print(int level, char const *fmt, ...)
{
    int status;
    va_list args;

    va_start(args, fmt);

    status = vfprintf(stdout, fmt, args);

    va_end(args);

    return status;
}
