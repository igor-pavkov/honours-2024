static int begins_with(const char *s, const char *beg)
{
	if (strncmp(s, beg, strlen(beg)) == 0)
		return 1;
	else
		return 0;
}