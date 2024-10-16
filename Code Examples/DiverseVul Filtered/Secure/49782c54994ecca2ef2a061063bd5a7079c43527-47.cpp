static int _ldap_str_equal_to_const(const char *str, uint str_len, const char *cstr)
{
	int i;

	if (strlen(cstr) != str_len)
		return 0;

	for (i = 0; i < str_len; ++i) {
		if (str[i] != cstr[i]) {
			return 0;
		}
	}

	return 1;
}