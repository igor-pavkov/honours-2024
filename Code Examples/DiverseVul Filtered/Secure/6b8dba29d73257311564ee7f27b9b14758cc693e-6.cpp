static void detect_quiet(int argc, char **argv) {
	int i;
	
	// detect --quiet
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--quiet") == 0) {
			arg_quiet = 1;
			break;
		}
		
		// detect end of firejail params
		if (strcmp(argv[i], "--") == 0)
			break;
		if (strncmp(argv[i], "--", 2) != 0)
			break;
	}
}