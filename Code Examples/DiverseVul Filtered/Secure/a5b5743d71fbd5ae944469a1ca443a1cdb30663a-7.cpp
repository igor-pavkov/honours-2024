void php_filter_unsafe_raw(PHP_INPUT_FILTER_PARAM_DECL)
{
	/* Only if no flags are set (optimization) */
	if (flags != 0 && Z_STRLEN_P(value) > 0) {
		unsigned char enc[256] = {0};

		php_filter_strip(value, flags);

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
	}
}