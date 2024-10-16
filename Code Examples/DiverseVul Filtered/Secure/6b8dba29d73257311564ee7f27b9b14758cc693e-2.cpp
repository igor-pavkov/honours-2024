static void myexit(int rv) {
	logmsg("exiting...");
	if (!arg_command && !arg_quiet)
		printf("\nParent is shutting down, bye...\n");


	// delete sandbox files in shared memory
	EUID_ROOT();
	clear_run_files(sandbox_pid);
	appimage_clear();
	flush_stdin();
	exit(rv); 
}