char *guess_shell(void) {
	char *shell = NULL;
	// shells in order of preference
	char *shells[] = {"/bin/bash", "/bin/csh", "/usr/bin/zsh", "/bin/sh", "/bin/ash", NULL };

	int i = 0;
	while (shells[i] != NULL) {
		struct stat s;
		// access call checks as real UID/GID, not as effective UID/GID
		if (stat(shells[i], &s) == 0 && access(shells[i], R_OK) == 0) {
			shell = shells[i];
			break;
		}
		i++;
	}

	return shell;
}