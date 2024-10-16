static char *lxc_attach_getpwshell(uid_t uid)
{
	/* local variables */
	pid_t pid;
	int pipes[2];
	int ret;
	int fd;
	char *result = NULL;

	/* we need to fork off a process that runs the
	 * getent program, and we need to capture its
	 * output, so we use a pipe for that purpose
	 */
	ret = pipe(pipes);
	if (ret < 0)
		return NULL;

	pid = fork();
	if (pid < 0) {
		close(pipes[0]);
		close(pipes[1]);
		return NULL;
	}

	if (pid) {
		/* parent process */
		FILE *pipe_f;
		char *line = NULL;
		size_t line_bufsz = 0;
		int found = 0;
		int status;

		close(pipes[1]);

		pipe_f = fdopen(pipes[0], "r");
		while (getline(&line, &line_bufsz, pipe_f) != -1) {
			char *token;
			char *saveptr = NULL;
			long value;
			char *endptr = NULL;
			int i;

			/* if we already found something, just continue
			 * to read until the pipe doesn't deliver any more
			 * data, but don't modify the existing data
			 * structure
			 */
			if (found)
				continue;

			/* trim line on the right hand side */
			for (i = strlen(line); i > 0 && (line[i - 1] == '\n' || line[i - 1] == '\r'); --i)
				line[i - 1] = '\0';

			/* split into tokens: first user name */
			token = strtok_r(line, ":", &saveptr);
			if (!token)
				continue;
			/* next: dummy password field */
			token = strtok_r(NULL, ":", &saveptr);
			if (!token)
				continue;
			/* next: user id */
			token = strtok_r(NULL, ":", &saveptr);
			value = token ? strtol(token, &endptr, 10) : 0;
			if (!token || !endptr || *endptr || value == LONG_MIN || value == LONG_MAX)
				continue;
			/* dummy sanity check: user id matches */
			if ((uid_t) value != uid)
				continue;
			/* skip fields: gid, gecos, dir, go to next field 'shell' */
			for (i = 0; i < 4; i++) {
				token = strtok_r(NULL, ":", &saveptr);
				if (!token)
					break;
			}
			if (!token)
				continue;
			if (result)
				free(result);
			result = strdup(token);

			/* sanity check that there are no fields after that */
			token = strtok_r(NULL, ":", &saveptr);
			if (token)
				continue;

			found = 1;
		}

		free(line);
		fclose(pipe_f);
	again:
		if (waitpid(pid, &status, 0) < 0) {
			if (errno == EINTR)
				goto again;
			return NULL;
		}

		/* some sanity checks: if anything even hinted at going
		 * wrong: we can't be sure we have a valid result, so
		 * we assume we don't
		 */

		if (!WIFEXITED(status))
			return NULL;

		if (WEXITSTATUS(status) != 0)
			return NULL;

		if (!found)
			return NULL;

		return result;
	} else {
		/* child process */
		char uid_buf[32];
		char *arguments[] = {
			"getent",
			"passwd",
			uid_buf,
			NULL
		};

		close(pipes[0]);

		/* we want to capture stdout */
		dup2(pipes[1], 1);
		close(pipes[1]);

		/* get rid of stdin/stderr, so we try to associate it
		 * with /dev/null
		 */
		fd = open("/dev/null", O_RDWR);
		if (fd < 0) {
			close(0);
			close(2);
		} else {
			dup2(fd, 0);
			dup2(fd, 2);
			close(fd);
		}

		/* finish argument list */
		ret = snprintf(uid_buf, sizeof(uid_buf), "%ld", (long) uid);
		if (ret <= 0)
			exit(-1);

		/* try to run getent program */
		(void) execvp("getent", arguments);
		exit(-1);
	}
}