void gd_error_ex(int priority, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	_gd_error_ex(priority, format, args);
	va_end(args);
}