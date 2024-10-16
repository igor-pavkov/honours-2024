void php_filter_magic_quotes(PHP_INPUT_FILTER_PARAM_DECL)
{
	char *buf;
	int   len;
	
	/* just call php_addslashes quotes */
	buf = php_addslashes(Z_STRVAL_P(value), Z_STRLEN_P(value), &len, 0 TSRMLS_CC);

	efree(Z_STRVAL_P(value));
	Z_STRVAL_P(value) = buf;
	Z_STRLEN_P(value) = len;
}