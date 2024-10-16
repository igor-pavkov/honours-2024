PHP_FUNCTION(ldap_escape)
{
	char *value, *ignores, *result;
	int valuelen = 0, ignoreslen = 0, i;
	size_t resultlen;
	long flags = 0;
	zend_bool map[256] = {0}, havecharlist = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|sl", &value, &valuelen, &ignores, &ignoreslen, &flags) != SUCCESS) {
		return;
	}

	if (!valuelen) {
		RETURN_EMPTY_STRING();
	}

	if (flags & PHP_LDAP_ESCAPE_FILTER) {
		havecharlist = 1;
		php_ldap_escape_map_set_chars(map, "\\*()\0", sizeof("\\*()\0") - 1, 1);
	}

	if (flags & PHP_LDAP_ESCAPE_DN) {
		havecharlist = 1;
		php_ldap_escape_map_set_chars(map, "\\,=+<>;\"#", sizeof("\\,=+<>;\"#") - 1, 1);
	}

	if (!havecharlist) {
		for (i = 0; i < 256; i++) {
			map[i] = 1;
		}
	}

	if (ignoreslen) {
		php_ldap_escape_map_set_chars(map, ignores, ignoreslen, 0);
	}

	php_ldap_do_escape(map, value, valuelen, &result, &resultlen);

	RETURN_STRINGL(result, resultlen, 0);
}