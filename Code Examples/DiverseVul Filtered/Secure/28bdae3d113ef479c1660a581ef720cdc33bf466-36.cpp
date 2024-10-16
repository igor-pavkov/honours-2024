static int opt_eq(const char *s, unsigned len, const char *opt)
{
	if(strlen(opt) == len && strncmp(s, opt, len) == 0)
		return 1;
	else
		return 0;
}