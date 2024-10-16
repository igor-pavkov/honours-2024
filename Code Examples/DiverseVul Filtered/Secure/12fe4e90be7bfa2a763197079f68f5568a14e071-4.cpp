static void timelib_eat_spaces(char **ptr)
{
	while (**ptr == ' ' || **ptr == '\t') {
		++*ptr;
	}
}