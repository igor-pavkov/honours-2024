static void debug(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	debug_handler(0, fmt, ap);
	va_end(ap);
}