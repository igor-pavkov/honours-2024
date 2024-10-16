int lxc_set_death_signal(int signal)
{
	int ret;
	pid_t ppid;

	ret = prctl(PR_SET_PDEATHSIG, signal, 0, 0, 0);

	/* Check whether we have been orphaned. */
	ppid = (pid_t)syscall(SYS_getppid);
	if (ppid == 1) {
		pid_t self;

		self = lxc_raw_getpid();
		ret = kill(self, SIGKILL);
		if (ret < 0)
			return -1;
	}

	if (ret < 0) {
		SYSERROR("Failed to set PR_SET_PDEATHSIG to %d", signal);
		return -1;
	}

	return 0;
}