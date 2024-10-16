static void ToUpperCase(
	/*! Input string to be converted. */
	char *s)
{
	while (*s) {
		*s = (char)toupper(*s);
		++s;
	}
}