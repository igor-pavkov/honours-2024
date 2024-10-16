static void php_filter_encode_html(zval *value, const unsigned char *chars)
{
	smart_str str = {0};
	int len = Z_STRLEN_P(value);
	unsigned char *s = (unsigned char *)Z_STRVAL_P(value);
	unsigned char *e = s + len;

	if (Z_STRLEN_P(value) == 0) {
		return;
	}

	while (s < e) {
		if (chars[*s]) {
			smart_str_appendl(&str, "&#", 2);
			smart_str_append_unsigned(&str, (unsigned long)*s);
			smart_str_appendc(&str, ';');
		} else {
			/* XXX: this needs to be optimized to work with blocks of 'safe' chars */
			smart_str_appendc(&str, *s);
		}
		s++;
	}

	smart_str_0(&str);
	efree(Z_STRVAL_P(value));
	Z_STRVAL_P(value) = str.c;
	Z_STRLEN_P(value) = str.len;
}