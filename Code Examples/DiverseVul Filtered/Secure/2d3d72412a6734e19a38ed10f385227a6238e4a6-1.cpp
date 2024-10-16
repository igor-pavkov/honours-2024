PHPAPI char *php_replace_controlchars_ex(char *str, size_t len)
{
	unsigned char *s = (unsigned char *)str;
	unsigned char *e = (unsigned char *)str + len;

	if (!str) {
		return (NULL);
	}

	while (s < e) {

		if (iscntrl(*s)) {
			*s='_';
		}
		s++;
	}

	return (str);
}