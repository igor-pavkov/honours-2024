static void parse_line(char *line, int linenum)
{
	int tmp;
	if (strcmp(line, "user_allow_other") == 0)
		user_allow_other = 1;
	else if (sscanf(line, "mount_max = %i", &tmp) == 1)
		mount_max = tmp;
	else if(line[0])
		fprintf(stderr,
			"%s: unknown parameter in %s at line %i: '%s'\n",
			progname, FUSE_CONF, linenum, line);
}