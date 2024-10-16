int sys_popen(const char *command)
{
	int parent_end, child_end;
	int pipe_fds[2];
	popen_list *entry = NULL;
	char **argl = NULL;
	int ret;

	if (!*command) {
		errno = EINVAL;
		return -1;
	}

	ret = pipe(pipe_fds);
	if (ret < 0) {
		DEBUG(0, ("sys_popen: error opening pipe: %s\n",
			  strerror(errno)));
		return -1;
	}

	parent_end = pipe_fds[0];
	child_end = pipe_fds[1];

	entry = SMB_MALLOC_P(popen_list);
	if (entry == NULL) {
		DEBUG(0, ("sys_popen: malloc failed\n"));
		goto err_exit;
	}

	ZERO_STRUCTP(entry);

	/*
	 * Extract the command and args into a NULL terminated array.
	 */

	argl = extract_args(NULL, command);
	if (argl == NULL) {
		DEBUG(0, ("sys_popen: extract_args() failed: %s\n", strerror(errno)));
		goto err_exit;
	}

	entry->child_pid = fork();

	if (entry->child_pid == -1) {
		DEBUG(0, ("sys_popen: fork failed: %s\n", strerror(errno)));
		goto err_exit;
	}

	if (entry->child_pid == 0) {

		/*
		 * Child !
		 */

		int child_std_end = STDOUT_FILENO;
		popen_list *p;

		close(parent_end);
		if (child_end != child_std_end) {
			dup2 (child_end, child_std_end);
			close (child_end);
		}

		/*
		 * POSIX.2:  "popen() shall ensure that any streams from previous
		 * popen() calls that remain open in the parent process are closed
		 * in the new child process."
		 */

		for (p = popen_chain; p; p = p->next)
			close(p->fd);

		ret = execv(argl[0], argl);
		if (ret == -1) {
			DEBUG(0, ("sys_popen: ERROR executing command "
				  "'%s': %s\n", command, strerror(errno)));
		}
		_exit (127);
	}

	/*
	 * Parent.
	 */

	close (child_end);
	TALLOC_FREE(argl);

	/* Link into popen_chain. */
	entry->next = popen_chain;
	popen_chain = entry;
	entry->fd = parent_end;

	return entry->fd;

err_exit:

	SAFE_FREE(entry);
	TALLOC_FREE(argl);
	close(pipe_fds[0]);
	close(pipe_fds[1]);
	return -1;
}