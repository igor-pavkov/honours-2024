static int get_mnt_opts(int flags, char *opts, char **mnt_optsp)
{
	int i;
	int l;

	if (!(flags & MS_RDONLY) && add_option(mnt_optsp, "rw", 0) == -1)
		return -1;

	for (i = 0; mount_flags[i].opt != NULL; i++) {
		if (mount_flags[i].on && (flags & mount_flags[i].flag) &&
		    add_option(mnt_optsp, mount_flags[i].opt, 0) == -1)
			return -1;
	}

	if (add_option(mnt_optsp, opts, 0) == -1)
		return -1;
	/* remove comma from end of opts*/
	l = strlen(*mnt_optsp);
	if ((*mnt_optsp)[l-1] == ',')
		(*mnt_optsp)[l-1] = '\0';
	if (getuid() != 0) {
		const char *user = get_user_name();
		if (user == NULL)
			return -1;

		if (add_option(mnt_optsp, "user=", strlen(user)) == -1)
			return -1;
		strcat(*mnt_optsp, user);
	}
	return 0;
}