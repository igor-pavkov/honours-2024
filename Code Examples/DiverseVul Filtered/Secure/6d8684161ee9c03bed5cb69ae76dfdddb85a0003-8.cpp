int mingw_execv(const char *cmd, char *const *argv)
{
	/* check if git_command is a shell script */
	if (!try_shell_exec(cmd, argv)) {
		int pid, status;

		pid = mingw_spawnv(cmd, (const char **)argv, 0);
		if (pid < 0)
			return -1;
		if (waitpid(pid, &status, 0) < 0)
			status = 255;
		exit(status);
	}
	return -1;
}