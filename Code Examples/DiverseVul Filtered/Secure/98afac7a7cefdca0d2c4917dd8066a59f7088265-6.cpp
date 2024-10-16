static int chop_last_dir(char **remoteurl, int is_relative)
{
	char *rfind = find_last_dir_sep(*remoteurl);
	if (rfind) {
		*rfind = '\0';
		return 0;
	}

	rfind = strrchr(*remoteurl, ':');
	if (rfind) {
		*rfind = '\0';
		return 1;
	}

	if (is_relative || !strcmp(".", *remoteurl))
		die(_("cannot strip one component off url '%s'"),
			*remoteurl);

	free(*remoteurl);
	*remoteurl = xstrdup(".");
	return 0;
}