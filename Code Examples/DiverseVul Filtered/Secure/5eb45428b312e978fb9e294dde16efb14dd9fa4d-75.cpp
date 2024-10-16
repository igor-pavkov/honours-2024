bool task_blocking_signal(pid_t pid, int signal)
{
	bool bret = false;
	char *line = NULL;
	long unsigned int sigblk = 0;
	size_t n = 0;
	int ret;
	FILE *f;

	char status[__PROC_STATUS_LEN];

	ret = snprintf(status, __PROC_STATUS_LEN, "/proc/%d/status", pid);
	if (ret < 0 || ret >= __PROC_STATUS_LEN)
		return bret;

	f = fopen(status, "r");
	if (!f)
		return bret;

	while (getline(&line, &n, f) != -1) {
		if (strncmp(line, "SigBlk:\t", 8))
			continue;

		if (sscanf(line + 8, "%lx", &sigblk) != 1)
			goto out;
	}

	if (sigblk & (1LU << (signal - 1)))
		bret = true;

out:
	free(line);
	fclose(f);
	return bret;
}