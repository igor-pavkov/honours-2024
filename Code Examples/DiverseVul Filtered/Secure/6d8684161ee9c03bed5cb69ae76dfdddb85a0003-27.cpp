static int try_shell_exec(const char *cmd, char *const *argv)
{
	const char *interpr = parse_interpreter(cmd);
	char *prog;
	int pid = 0;

	if (!interpr)
		return 0;
	prog = path_lookup(interpr, 1);
	if (prog) {
		int argc = 0;
		const char **argv2;
		while (argv[argc]) argc++;
		ALLOC_ARRAY(argv2, argc + 1);
		argv2[0] = (char *)cmd;	/* full path to the script file */
		memcpy(&argv2[1], &argv[1], sizeof(*argv) * argc);
		pid = mingw_spawnv(prog, argv2, 1);
		if (pid >= 0) {
			int status;
			if (waitpid(pid, &status, 0) < 0)
				status = 255;
			exit(status);
		}
		pid = 1;	/* indicate that we tried but failed */
		free(prog);
		free(argv2);
	}
	return pid;
}