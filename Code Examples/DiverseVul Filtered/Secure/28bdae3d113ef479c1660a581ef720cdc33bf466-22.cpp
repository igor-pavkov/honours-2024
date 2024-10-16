static int check_is_mount(const char *last, const char *mnt)
{
	pid_t pid, p;
	int status;
	const char *a[2] = { last, mnt };

	pid = clone_newns((void *) a);
	if (pid == (pid_t) -1) {
		fprintf(stderr, "%s: failed to clone namespace: %s\n",
			progname, strerror(errno));
		return -1;
	}
	p = waitpid(pid, &status, __WCLONE);
	if (p == (pid_t) -1) {
		fprintf(stderr, "%s: waitpid failed: %s\n",
			progname, strerror(errno));
		return -1;
	}
	if (!WIFEXITED(status)) {
		fprintf(stderr, "%s: child terminated abnormally (status %i)\n",
			progname, status);
		return -1;
	}
	if (WEXITSTATUS(status) != 0)
		return -1;

	return 0;
}