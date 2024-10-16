PHPAPI zend_string *php_url_encode(char const *s, size_t len)
{
	register unsigned char c;
	unsigned char *to;
	unsigned char const *from, *end;
	zend_string *start;

	from = (unsigned char *)s;
	end = (unsigned char *)s + len;
	start = zend_string_safe_alloc(3, len, 0, 0);
	to = (unsigned char*)ZSTR_VAL(start);

	while (from < end) {
		c = *from++;

		if (c == ' ') {
			*to++ = '+';
#ifndef CHARSET_EBCDIC
		} else if ((c < '0' && c != '-' && c != '.') ||
				   (c < 'A' && c > '9') ||
				   (c > 'Z' && c < 'a' && c != '_') ||
				   (c > 'z')) {
			to[0] = '%';
			to[1] = hexchars[c >> 4];
			to[2] = hexchars[c & 15];
			to += 3;
#else /*CHARSET_EBCDIC*/
		} else if (!isalnum(c) && strchr("_-.", c) == NULL) {
			/* Allow only alphanumeric chars and '_', '-', '.'; escape the rest */
			to[0] = '%';
			to[1] = hexchars[os_toascii[c] >> 4];
			to[2] = hexchars[os_toascii[c] & 15];
			to += 3;
#endif /*CHARSET_EBCDIC*/
		} else {
			*to++ = c;
		}
	}
	*to = '\0';

	start = zend_string_truncate(start, to - (unsigned char*)ZSTR_VAL(start), 0);

	return start;
}