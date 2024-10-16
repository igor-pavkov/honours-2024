size_t format_append(std::string& dest, const char *format, ...) {
	char* buffer;
	int len = 0;
	va_list ap;

	va_start(ap, format);
	ut_a((len = vasprintf(&buffer, format, ap)) != -1 );
	va_end(ap);

	dest += buffer;
	free(buffer);
	return (size_t)len;
}