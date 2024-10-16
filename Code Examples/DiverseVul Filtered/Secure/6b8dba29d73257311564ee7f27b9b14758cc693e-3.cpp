static void delete_name_file(pid_t pid) {
	char *fname;
	if (asprintf(&fname, "%s/%d", RUN_FIREJAIL_NAME_DIR, pid) == -1)
		errExit("asprintf");
	int rv = unlink(fname);
	(void) rv;
	free(fname);
}