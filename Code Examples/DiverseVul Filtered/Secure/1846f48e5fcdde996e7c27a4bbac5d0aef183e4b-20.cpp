static void _gd_error_ex(int priority, const char *format, va_list args)
{
	if (gd_error_method) {
		gd_error_method(priority, format, args);
	}
}