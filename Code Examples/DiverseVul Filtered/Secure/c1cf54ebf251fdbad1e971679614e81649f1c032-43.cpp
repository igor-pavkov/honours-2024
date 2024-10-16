char *get_rundir()
{
	char *rundir;
	const char *homedir;
	struct stat sb;

	if (stat(RUNTIME_PATH, &sb) < 0)
		return NULL;

	if (geteuid() == sb.st_uid || getegid() == sb.st_gid) {
		rundir = strdup(RUNTIME_PATH);
		return rundir;
	}

	rundir = getenv("XDG_RUNTIME_DIR");
	if (rundir) {
		rundir = strdup(rundir);
		return rundir;
	}

	INFO("XDG_RUNTIME_DIR isn't set in the environment.");
	homedir = getenv("HOME");
	if (!homedir) {
		ERROR("HOME isn't set in the environment.");
		return NULL;
	}

	rundir = malloc(sizeof(char) * (17 + strlen(homedir)));
	if (!rundir)
		return NULL;

	sprintf(rundir, "%s/.cache/lxc/run/", homedir);

	return rundir;
}