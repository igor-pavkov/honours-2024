static const char *check_secret(int module, const char *user, const char *group,
				const char *challenge, const char *pass)
{
	char line[1024];
	char pass2[MAX_DIGEST_LEN*2];
	const char *fname = lp_secrets_file(module);
	STRUCT_STAT st;
	int fd, ok = 1;
	int user_len = strlen(user);
	int group_len = group ? strlen(group) : 0;
	char *err;

	if (!fname || !*fname || (fd = open(fname, O_RDONLY)) < 0)
		return "no secrets file";

	if (do_fstat(fd, &st) == -1) {
		rsyserr(FLOG, errno, "fstat(%s)", fname);
		ok = 0;
	} else if (lp_strict_modes(module)) {
		if ((st.st_mode & 06) != 0) {
			rprintf(FLOG, "secrets file must not be other-accessible (see strict modes option)\n");
			ok = 0;
		} else if (MY_UID() == 0 && st.st_uid != 0) {
			rprintf(FLOG, "secrets file must be owned by root when running as root (see strict modes)\n");
			ok = 0;
		}
	}
	if (!ok) {
		close(fd);
		return "ignoring secrets file";
	}

	if (*user == '#') {
		/* Reject attempt to match a comment. */
		close(fd);
		return "invalid username";
	}

	/* Try to find a line that starts with the user (or @group) name and a ':'. */
	err = "secret not found";
	while ((user || group) && read_line_old(fd, line, sizeof line, 1)) {
		const char **ptr, *s;
		int len;
		if (*line == '@') {
			ptr = &group;
			len = group_len;
			s = line+1;
		} else {
			ptr = &user;
			len = user_len;
			s = line;
		}
		if (!*ptr || strncmp(s, *ptr, len) != 0 || s[len] != ':')
			continue;
		generate_hash(s+len+1, challenge, pass2);
		if (strcmp(pass, pass2) == 0) {
			err = NULL;
			break;
		}
		err = "password mismatch";
		*ptr = NULL; /* Don't look for name again. */
	}

	close(fd);

	memset(line, 0, sizeof line);
	memset(pass2, 0, sizeof pass2);

	return err;
}