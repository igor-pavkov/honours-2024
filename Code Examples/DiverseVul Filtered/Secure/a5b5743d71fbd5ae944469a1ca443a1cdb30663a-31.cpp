void php_filter_string(PHP_INPUT_FILTER_PARAM_DECL)
{
	size_t new_len;
	unsigned char enc[256] = {0};

	/* strip high/strip low ( see flags )*/
	php_filter_strip(value, flags);

	if (!(flags & FILTER_FLAG_NO_ENCODE_QUOTES)) {
		enc['\''] = enc['"'] = 1;
	}
	if (flags & FILTER_FLAG_ENCODE_AMP) {
		enc['&'] = 1;
	}
	if (flags & FILTER_FLAG_ENCODE_LOW) {
		memset(enc, 1, 32);
	}
	if (flags & FILTER_FLAG_ENCODE_HIGH) {
		memset(enc + 127, 1, sizeof(enc) - 127);
	}

	php_filter_encode_html(value, enc);

	/* strip tags, implicitly also removes \0 chars */
	new_len = php_strip_tags_ex(Z_STRVAL_P(value), Z_STRLEN_P(value), NULL, NULL, 0, 1);
	Z_STRLEN_P(value) = new_len;

	if (new_len == 0) {
		zval_dtor(value);
		ZVAL_EMPTY_STRING(value);
		return;
	}
}