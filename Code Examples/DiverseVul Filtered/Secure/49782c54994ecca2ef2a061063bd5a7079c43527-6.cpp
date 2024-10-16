static int _ldap_strlen_max(const char *str, uint max_len)
{
	int i;

	for (i = 0; i < max_len; ++i) {
		if (str[i] == '\0') {
			return i;
		}
	}

	return max_len;
}