void php_gd_error(const char *format, ...)
{
	va_list args;

	TSRMLS_FETCH();

	va_start(args, format);
	php_verror(NULL, "", E_WARNING, format, args TSRMLS_CC);
	va_end(args);
}