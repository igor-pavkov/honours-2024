static void php_filter_strip(zval *value, long flags)
{
	unsigned char *buf, *str;
	int   i, c;
	
	/* Optimization for if no strip flags are set */
	if (! ((flags & FILTER_FLAG_STRIP_LOW) || (flags & FILTER_FLAG_STRIP_HIGH)) ) {
		return;
	}

	str = (unsigned char *)Z_STRVAL_P(value);
	buf = safe_emalloc(1, Z_STRLEN_P(value) + 1, 1);
	c = 0;
	for (i = 0; i < Z_STRLEN_P(value); i++) {
		if ((str[i] > 127) && (flags & FILTER_FLAG_STRIP_HIGH)) {
		} else if ((str[i] < 32) && (flags & FILTER_FLAG_STRIP_LOW)) {
		} else if ((str[i] == '`') && (flags & FILTER_FLAG_STRIP_BACKTICK)) {
		} else {
			buf[c] = str[i];
			++c;
		}
	}
	/* update zval string data */
	buf[c] = '\0';
	efree(Z_STRVAL_P(value));
	Z_STRVAL_P(value) = (char *)buf;
	Z_STRLEN_P(value) = c;
}