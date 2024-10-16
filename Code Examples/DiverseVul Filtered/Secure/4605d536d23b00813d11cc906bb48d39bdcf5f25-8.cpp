static char *php_ap_basename(const zend_encoding *encoding, char *path TSRMLS_DC)
{
	char *s = strrchr(path, '\\');
	char *s2 = strrchr(path, '/');

	if (s && s2) {
		if (s > s2) {
			++s;
		} else {
			s = ++s2;
		}
		return s;
	} else if (s) {
		return ++s;
	} else if (s2) {
		return ++s2;
	}
	return path;
}