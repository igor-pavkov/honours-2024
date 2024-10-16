static inline int read_pid(char *str, pid_t *pid) {
	char *endptr;
	errno = 0;
	long int pidtmp = strtol(str, &endptr, 10);
	if ((errno == ERANGE && (pidtmp == LONG_MAX || pidtmp == LONG_MIN))
		|| (errno != 0 && pidtmp == 0)) {
		return 1;
	}
	// endptr points to '\0' char in str if the entire string is valid
	if (endptr == NULL || endptr[0]!='\0') {
		return 1;
	}
	*pid = (pid_t)pidtmp;
	return 0;
}