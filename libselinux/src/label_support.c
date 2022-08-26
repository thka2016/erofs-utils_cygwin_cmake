/*
 * This file contains helper functions for labeling support.
 *
 * Author : Richard Haines <richard_c_haines@btinternet.com>
 */

#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include "label_internal.h"

/*
 * The read_spec_entries and read_spec_entry functions may be used to
 * replace sscanf to read entries from spec files. The file and
 * property services now use these.
 */

/*
 * Read an entry from a spec file (e.g. file_contexts)
 * entry - Buffer to allocate for the entry.
 * ptr - current location of the line to be processed.
 * returns  - 0 on success and *entry is set to be a null
 *            terminated value. On Error it returns -1 and
              errno will be set.
 *
 */
static inline int read_spec_entry(char **entry, char **ptr, int *len, const char **errbuf)
{
	*entry = NULL;
	char *tmp_buf = NULL;

	while (isspace(**ptr) && **ptr != '\0')
		(*ptr)++;

	tmp_buf = *ptr;
	*len = 0;

	while (!isspace(**ptr) && **ptr != '\0') {
		if (!isascii(**ptr)) {
			errno = EINVAL;
			*errbuf = "Non-ASCII characters found";
			return -1;
		}
		(*ptr)++;
		(*len)++;
	}

	if (*len) {
		*entry = strndup(tmp_buf, *len);
		if (!*entry)
			return -1;
	}

	return 0;
}

/*
 * line_buf - Buffer containing the spec entries .
 * errbuf   - Double pointer used for passing back specific error messages.
 * num_args - The number of spec parameter entries to process.
 * ...      - A 'char **spec_entry' for each parameter.
 * returns  - The number of items processed. On error, it returns -1 with errno
 *            set and may set errbuf to a specific error message.
 *
 * This function calls read_spec_entry() to do the actual string processing.
 * As such, can return anything from that function as well.
 */
int hidden read_spec_entries(char *line_buf, const char **errbuf, int num_args, ...)
{
	char **spec_entry, *buf_p;
	int len, rc, items, entry_len = 0;
	va_list ap;

	*errbuf = NULL;

	len = strlen(line_buf);
	if (line_buf[len - 1] == '\n')
		line_buf[len - 1] = '\0';
	else
		/* Handle case if line not \n terminated by bumping
		 * the len for the check below (as the line is NUL
		 * terminated by getline(3)) */
		len++;

	buf_p = line_buf;
	while (isspace(*buf_p))
		buf_p++;

	/* Skip comment lines and empty lines. */
	if (*buf_p == '#' || *buf_p == '\0')
		return 0;

	/* Process the spec file entries */
	va_start(ap, num_args);

	items = 0;
	while (items < num_args) {
		spec_entry = va_arg(ap, char **);

		if (len - 1 == buf_p - line_buf) {
			va_end(ap);
			return items;
		}

		rc = read_spec_entry(spec_entry, &buf_p, &entry_len, errbuf);
		if (rc < 0) {
			va_end(ap);
			return rc;
		}
		if (entry_len)
			items++;
	}
	va_end(ap);
	return items;
}
