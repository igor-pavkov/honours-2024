bool task_blocks_signal(pid_t pid, int signal)
{
	int ret;
	char status[__PROC_STATUS_LEN];
	FILE *f;
	uint64_t sigblk = 0, one = 1;
	size_t n = 0;
	bool bret = false;
	char *line = NULL;

	ret = snprintf(status, __PROC_STATUS_LEN, "/proc/%d/status", pid);
	if (ret < 0 || ret >= __PROC_STATUS_LEN)
		return bret;

	f = fopen(status, "r");
	if (!f)
		return bret;

	while (getline(&line, &n, f) != -1) {
		char *numstr;

		if (strncmp(line, "SigBlk:", 7))
			continue;

		numstr = lxc_trim_whitespace_in_place(line + 7);
		ret = lxc_safe_uint64(numstr, &sigblk, 16);
		if (ret < 0)
			goto out;

		break;
	}

	if (sigblk & (one << (signal - 1)))
		bret = true;

out:
	free(line);
	fclose(f);
	return bret;
}