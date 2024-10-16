static char *php_ap_getword_conf(const zend_encoding *encoding, char *str TSRMLS_DC)
{
	while (*str && isspace(*str)) {
		++str;
	}

	if (!*str) {
		return estrdup("");
	}

	if (*str == '"' || *str == '\'') {
		char quote = *str;

		str++;
		return substring_conf(str, strlen(str), quote);
	} else {
		char *strend = str;

		while (*strend && !isspace(*strend)) {
			++strend;
		}
		return substring_conf(str, strend - str, 0);
	}
}