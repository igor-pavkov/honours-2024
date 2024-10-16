void php_gd_error_ex(int type, const char *format, ...)
{
	va_list args;

	TSRMLS_FETCH();

	va_start(args, format);
	php_verror(NULL, "", type, format, args TSRMLS_CC);
	va_end(args);
}