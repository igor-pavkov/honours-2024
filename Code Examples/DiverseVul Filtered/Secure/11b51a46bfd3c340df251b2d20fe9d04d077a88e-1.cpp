static const char *chdir_to_parent(const char *node, char **resbuf)
{
	char *tmp, *res;
	const char *parent;
	char buf[65536];

	*resbuf = xstrdup(node);

	tmp = strrchr(*resbuf, '/');
	if (!tmp)
		die (2, _("umount: internal error: invalid abs path: %s"), node);

	if (tmp != *resbuf) {
		*tmp = '\0';
		res = tmp + 1;
		parent = *resbuf;
	} else if (tmp[1] != '\0') {
		res = tmp + 1;
		parent = "/";
	} else {
		res = ".";
		parent = "/";
	}

	if (chdir(parent) == -1)
		die (2, _("umount: failed to chdir to %s: %s"),
					parent, strerror(errno));

	if (!getcwd(buf, sizeof(buf)))
		die (2, _("umount: failed to obtain current directory: %s"),
			strerror(errno));

	if (strcmp(buf, parent) != 0)
		die (2, _("umount: mountpoint moved (%s -> %s)"), parent, buf);

	if (verbose)
		printf(_("current directory moved to %s\n"), res);

	return res;
}