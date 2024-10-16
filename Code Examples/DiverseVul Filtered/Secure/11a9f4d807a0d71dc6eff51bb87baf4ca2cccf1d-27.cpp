const char *mkpath(const char *fmt, ...)
{
	va_list args;
	struct strbuf *pathname = get_pathname();
	va_start(args, fmt);
	strbuf_vaddf(pathname, fmt, args);
	va_end(args);
	return cleanup_path(pathname->buf);
}