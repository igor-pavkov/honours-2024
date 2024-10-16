static void php_ldap_do_escape(const zend_bool *map, const char *value, size_t valuelen, char **result, size_t *resultlen)
{
	char hex[] = "0123456789abcdef";
	int i, p = 0;
	size_t len = 0;

	for (i = 0; i < valuelen; i++) {
		len += (map[(unsigned char) value[i]]) ? 3 : 1;
	}

	(*result) = (char *) safe_emalloc_string(1, len, 1);
	(*resultlen) = len;

	for (i = 0; i < valuelen; i++) {
		unsigned char v = (unsigned char) value[i];

		if (map[v]) {
			(*result)[p++] = '\\';
			(*result)[p++] = hex[v >> 4];
			(*result)[p++] = hex[v & 0x0f];
		} else {
			(*result)[p++] = v;
		}
	}

	(*result)[p++] = '\0';
}