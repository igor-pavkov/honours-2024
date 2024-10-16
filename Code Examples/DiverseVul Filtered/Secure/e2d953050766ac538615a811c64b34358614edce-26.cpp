static void usage(void) {
	printf("\nusage:\n");
	printf("\ttcmu-runner [options]\n");
	printf("\noptions:\n");
	printf("\t-h, --help: print this message and exit\n");
	printf("\t-V, --version: print version and exit\n");
	printf("\t-d, --debug: enable debug messages\n");
	printf("\t--handler-path: set path to search for handler modules\n");
	printf("\t\tdefault is %s\n", DEFAULT_HANDLER_PATH);
	printf("\t-l, --tcmu-log-dir: tcmu log dir\n");
	printf("\t\tdefault is %s\n", TCMU_LOG_DIR_DEFAULT);
	printf("\n");
}