static void php_ldap_escape_map_set_chars(zend_bool *map, const char *chars, const int charslen, char escape)
{
	int i = 0;
	while (i < charslen) {
		map[(unsigned char) chars[i++]] = escape;
	}
}