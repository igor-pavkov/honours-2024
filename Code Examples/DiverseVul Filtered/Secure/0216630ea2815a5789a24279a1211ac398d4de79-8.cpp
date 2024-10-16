static int php_openssl_spki_cleanup(const char *src, char *dest)
{
	int removed = 0;

	while (*src) {
		if (*src != '\n' && *src != '\r') {
			*dest++ = *src;
		} else {
			++removed;
		}
		++src;
	}
	*dest = 0;
	return removed;
}