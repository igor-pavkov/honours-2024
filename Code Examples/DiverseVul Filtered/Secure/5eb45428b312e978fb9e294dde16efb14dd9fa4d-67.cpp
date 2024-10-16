static char *get_eol(char *s, char *e)
{
	while ((s < e) && *s && (*s != '\n'))
		s++;
	return s;
}