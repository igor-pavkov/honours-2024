static int php_iconv_output_conflict(const char *handler_name, size_t handler_name_len)
{
	if (php_output_get_level()) {
		if (php_output_handler_conflict(handler_name, handler_name_len, ZEND_STRL("ob_iconv_handler"))
		||	php_output_handler_conflict(handler_name, handler_name_len, ZEND_STRL("mb_output_handler"))) {
			return FAILURE;
		}
	}
	return SUCCESS;
}