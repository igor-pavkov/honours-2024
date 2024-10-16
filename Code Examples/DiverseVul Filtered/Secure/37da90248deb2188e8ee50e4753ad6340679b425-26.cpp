ZEND_API double zend_hex_strtod(const char *str, char **endptr)
{
	const char *s = str;
	char c;
	int any = 0;
	double value = 0;

	if (*s == '0' && (s[1] == 'x' || s[1] == 'X')) {
		s += 2;
	}

	while ((c = *s++)) {
		if (c >= '0' && c <= '9') {
			c -= '0';
		} else if (c >= 'A' && c <= 'F') {
			c -= 'A' - 10;
		} else if (c >= 'a' && c <= 'f') {
			c -= 'a' - 10;
		} else {
			break;
		}

		any = 1;
		value = value * 16 + c;
	}

	if (endptr != NULL) {
		*endptr = (char *)(any ? s - 1 : str);
	}

	return value;
}