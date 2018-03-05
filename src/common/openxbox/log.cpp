#include <stdarg.h>
#include "log.h"

namespace openxbox {

/*!
 * Print a message to the log
 */
int log_print(int level, char const *fmt, ...)
{
	if (level > LOG_LEVEL) {
		return 0;
	}

	int status;
	va_list args;

	va_start(args, fmt);

	status = vfprintf(stdout, fmt, args);

	va_end(args);

	return status;
}

}
