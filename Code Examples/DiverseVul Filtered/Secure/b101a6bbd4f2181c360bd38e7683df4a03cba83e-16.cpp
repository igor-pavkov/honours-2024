static void zend_throw_or_error(int fetch_type, zend_class_entry *exception_ce, const char *format, ...) /* {{{ */
{
	va_list va;
	char *message = NULL;

	va_start(va, format);
	zend_vspprintf(&message, 0, format, va);

	if (fetch_type & ZEND_FETCH_CLASS_EXCEPTION) {
		zend_throw_error(exception_ce, "%s", message);
	} else {
		zend_error(E_ERROR, "%s", message);
	}

	efree(message);
	va_end(va);
}