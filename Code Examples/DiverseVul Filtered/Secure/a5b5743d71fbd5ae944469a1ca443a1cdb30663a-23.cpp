void php_filter_full_special_chars(PHP_INPUT_FILTER_PARAM_DECL)
{
	char *buf;
	int   len, quotes;
	
	if (!(flags & FILTER_FLAG_NO_ENCODE_QUOTES)) {
		quotes = ENT_QUOTES;
	} else {
		quotes = ENT_NOQUOTES;
	}
	buf = php_escape_html_entities_ex(Z_STRVAL_P(value), Z_STRLEN_P(value), &len, 1, quotes, SG(default_charset), 0 TSRMLS_CC);
	efree(Z_STRVAL_P(value));
	Z_STRVAL_P(value) = buf;
	Z_STRLEN_P(value) = len;
}