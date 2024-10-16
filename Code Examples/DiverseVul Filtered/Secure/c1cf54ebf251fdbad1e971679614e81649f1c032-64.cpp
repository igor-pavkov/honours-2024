int run_command(char *buf, size_t buf_size, int (*child_fn)(void *), void *args)
{
	pid_t child;
	int ret, fret, pipefd[2];
	ssize_t bytes;

	/* Make sure our callers do not receive uninitialized memory. */
	if (buf_size > 0 && buf)
		buf[0] = '\0';

	if (pipe(pipefd) < 0) {
		SYSERROR("failed to create pipe");
		return -1;
	}

	child = lxc_raw_clone(0);
	if (child < 0) {
		close(pipefd[0]);
		close(pipefd[1]);
		SYSERROR("failed to create new process");
		return -1;
	}

	if (child == 0) {
		/* Close the read-end of the pipe. */
		close(pipefd[0]);

		/* Redirect std{err,out} to write-end of the
		 * pipe.
		 */
		ret = dup2(pipefd[1], STDOUT_FILENO);
		if (ret >= 0)
			ret = dup2(pipefd[1], STDERR_FILENO);

		/* Close the write-end of the pipe. */
		close(pipefd[1]);

		if (ret < 0) {
			SYSERROR("failed to duplicate std{err,out} file descriptor");
			_exit(EXIT_FAILURE);
		}

		/* Does not return. */
		child_fn(args);
		ERROR("failed to exec command");
		_exit(EXIT_FAILURE);
	}

	/* close the write-end of the pipe */
	close(pipefd[1]);

	if (buf && buf_size > 0) {
		bytes = lxc_read_nointr(pipefd[0], buf, buf_size - 1);
		if (bytes > 0)
			buf[bytes - 1] = '\0';
	}

	fret = wait_for_pid(child);
	/* close the read-end of the pipe */
	close(pipefd[0]);

	return fret;
}