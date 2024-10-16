char *must_make_path(const char *first, ...)
{
	va_list args;
	char *cur, *dest;
	size_t full_len = strlen(first);
	size_t buf_len;

	dest = must_copy_string(first);

	va_start(args, first);
	while ((cur = va_arg(args, char *)) != NULL) {
		full_len += strlen(cur);
		if (cur[0] != '/')
			full_len++;

		buf_len = full_len + 1;
		dest = must_realloc(dest, buf_len);

		if (cur[0] != '/')
			(void)strlcat(dest, "/", buf_len);
		(void)strlcat(dest, cur, buf_len);
	}
	va_end(args);

	return dest;
}