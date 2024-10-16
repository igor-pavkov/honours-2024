char *must_concat(const char *first, ...)
{
	va_list args;
	char *cur, *dest;
	size_t cur_len, it_len;

	dest = must_copy_string(first);
	cur_len = it_len = strlen(first);

	va_start(args, first);
	while ((cur = va_arg(args, char *)) != NULL) {
		it_len = strlen(cur);

		dest = must_realloc(dest, cur_len + it_len + 1);

		(void)memcpy(dest + cur_len, cur, it_len);
		cur_len += it_len;
	}
	va_end(args);

	dest[cur_len] = 0;
	return dest;
}