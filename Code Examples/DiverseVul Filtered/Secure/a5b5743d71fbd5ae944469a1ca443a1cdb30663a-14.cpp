static void filter_map_apply(zval *value, filter_map *map)
{
	unsigned char *buf, *str;
	int   i, c;
	
	str = (unsigned char *)Z_STRVAL_P(value);
	buf = safe_emalloc(1, Z_STRLEN_P(value) + 1, 1);
	c = 0;
	for (i = 0; i < Z_STRLEN_P(value); i++) {
		if ((*map)[str[i]]) {
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