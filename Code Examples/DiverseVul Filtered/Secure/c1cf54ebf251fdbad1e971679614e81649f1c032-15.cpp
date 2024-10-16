void remove_trailing_newlines(char *l)
{
	char *p = l;

	while (*p)
		p++;

	while (--p >= l && *p == '\n')
		*p = '\0';
}