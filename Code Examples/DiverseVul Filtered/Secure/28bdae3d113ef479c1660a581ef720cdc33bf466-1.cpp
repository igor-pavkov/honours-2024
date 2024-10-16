static void strip_line(char *line)
{
	char *s = strchr(line, '#');
	if (s != NULL)
		s[0] = '\0';
	for (s = line + strlen(line) - 1;
	     s >= line && isspace((unsigned char) *s); s--);
	s[1] = '\0';
	for (s = line; isspace((unsigned char) *s); s++);
	if (s != line)
		memmove(line, s, strlen(s)+1);
}