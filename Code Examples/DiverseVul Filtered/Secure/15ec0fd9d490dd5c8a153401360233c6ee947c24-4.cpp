static struct lxc_proc_context_info *lxc_proc_get_context_info(pid_t pid)
{
	struct lxc_proc_context_info *info = calloc(1, sizeof(*info));
	FILE *proc_file;
	char proc_fn[MAXPATHLEN];
	char *line = NULL;
	size_t line_bufsz = 0;
	int ret, found;

	if (!info) {
		SYSERROR("Could not allocate memory.");
		return NULL;
	}

	/* read capabilities */
	snprintf(proc_fn, MAXPATHLEN, "/proc/%d/status", pid);

	proc_file = fopen(proc_fn, "r");
	if (!proc_file) {
		SYSERROR("Could not open %s", proc_fn);
		goto out_error;
	}

	found = 0;
	while (getline(&line, &line_bufsz, proc_file) != -1) {
		ret = sscanf(line, "CapBnd: %llx", &info->capability_mask);
		if (ret != EOF && ret > 0) {
			found = 1;
			break;
		}
	}

	if (line)
		free(line);
	fclose(proc_file);

	if (!found) {
		SYSERROR("Could not read capability bounding set from %s", proc_fn);
		errno = ENOENT;
		goto out_error;
	}

	info->lsm_label = lsm_process_label_get(pid);

	return info;

out_error:
	free(info);
	return NULL;
}