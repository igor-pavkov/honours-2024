static void timelib_eat_until_separator(char **ptr)
{
	while (strchr(" \t.,:;/-0123456789", **ptr) == NULL) {
		++*ptr;
	}
}