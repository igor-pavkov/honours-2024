char *must_append_path(char *first, ...)
{
	char *cur;
	size_t full_len;
	va_list args;
	char *dest = first;

	full_len = strlen(first);
	va_start(args, first);
	while ((cur = va_arg(args, char *)) != NULL) {
		full_len += strlen(cur);

		if (cur[0] != '/')
			full_len++;

		dest = must_realloc(dest, full_len + 1);

		if (cur[0] != '/')
			strcat(dest, "/");

		strcat(dest, cur);
	}
	va_end(args);

	return dest;
}