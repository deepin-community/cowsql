#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sqlite3.h>
#include <uv.h>

#include "../include/cowsql.h"

#include "./lib/assert.h"

#include "error.h"

/* Fallback message returned when failing to allocate the error message
 * itself. */
static char *cowsql__error_oom_msg =
    "error message unavailable (out of memory)";

void cowsql__error_init(cowsql__error *e)
{
	*e = NULL;
}

void cowsql__error_close(cowsql__error *e)
{
	if (*e != NULL && *e != cowsql__error_oom_msg) {
		sqlite3_free(*e);
	}
}

/* Set an error message by rendering the given format against the given
 * parameters.
 *
 * Any previously set error message will be cleared. */
static void cowsql__error_vprintf(cowsql__error *e,
				  const char *fmt,
				  va_list args)
{
	assert(fmt != NULL);

	/* If a previous error was set (other than the hard-coded OOM fallback
	 * fallback), let's free it. */
	if (*e != NULL && *e != cowsql__error_oom_msg) {
		sqlite3_free(*e);
	}

	/* Render the message. In case of error we fallback to the hard-coded
	 * OOM fallback message. */
	*e = sqlite3_vmprintf(fmt, args);
	if (*e == NULL) {
		*e = cowsql__error_oom_msg;
	}
}

void cowsql__error_printf(cowsql__error *e, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	cowsql__error_vprintf(e, fmt, args);
	va_end(args);
}

static void cowsql__error_vwrapf(cowsql__error *e,
				 const char *cause,
				 const char *fmt,
				 va_list args)
{
	cowsql__error tmp;
	char *msg;

	/* First, print the format and arguments, using a temporary error. */
	cowsql__error_init(&tmp);

	cowsql__error_vprintf(&tmp, fmt, args);

	if (cause == NULL) {
		/* Special case the cause error being empty. */
		cowsql__error_printf(e, "%s: (null)", tmp);
	} else if (cause == *e) {
		/* When the error is wrapping itself, we need to make a copy */
		cowsql__error_copy(e, &msg);
		cowsql__error_printf(e, "%s: %s", tmp, msg);
		sqlite3_free(msg);
	} else {
		cowsql__error_printf(e, "%s: %s", tmp, cause);
	}

	cowsql__error_close(&tmp);
}

void cowsql__error_wrapf(cowsql__error *e,
			 const cowsql__error *cause,
			 const char *fmt,
			 ...)
{
	va_list args;

	va_start(args, fmt);
	cowsql__error_vwrapf(e, (const char *)(*cause), fmt, args);
	va_end(args);
}

void cowsql__error_oom(cowsql__error *e, const char *msg, ...)
{
	va_list args;

	va_start(args, msg);
	cowsql__error_vwrapf(e, "out of memory", msg, args);
	va_end(args);
}

void cowsql__error_sys(cowsql__error *e, const char *msg)
{
	cowsql__error_printf(e, "%s: %s", msg, strerror(errno));
}

void cowsql__error_uv(cowsql__error *e, int err, const char *msg)
{
	cowsql__error_printf(e, "%s: %s (%s)", msg, uv_strerror(err),
			     uv_err_name(err));
}

int cowsql__error_copy(cowsql__error *e, char **msg)
{
	char *copy;
	size_t len;

	assert(e != NULL);
	assert(msg != NULL);

	/* Trying to copy an empty error message is an error. */
	if (*e == NULL) {
		*msg = NULL;
		return COWSQL_ERROR;
	}

	len = strlen(*e) + 1;

	copy = sqlite3_malloc((int)(len * sizeof *copy));
	if (copy == NULL) {
		*msg = NULL;
		return COWSQL_NOMEM;
	}

	memcpy(copy, *e, len);

	*msg = copy;

	return 0;
}

int cowsql__error_is_null(cowsql__error *e)
{
	return *e == NULL;
}

int cowsql__error_is_disconnect(cowsql__error *e)
{
	if (*e == NULL)
		return 0;

	if (strstr(*e, uv_err_name(UV_EOF)) != NULL)
		return 1;

	if (strstr(*e, uv_err_name(UV_ECONNRESET)) != NULL)
		return 1;

	return 0;
}
