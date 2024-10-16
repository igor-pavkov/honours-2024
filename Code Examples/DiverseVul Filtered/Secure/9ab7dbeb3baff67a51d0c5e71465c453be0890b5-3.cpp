int main(int argc, char *argv[]) {
	char *path = NULL;
	char buf[256];
	char pbuf[256];
	int start_argv = 1;
	int quiet = 0;
	size_t i;
	const char *prefix = NULL;

	for(i = 0; i < MAX_COMMANDLINE_FLAGS; i++) {
		if(start_argv < argc && argv[start_argv][0] == '-') {
			if(argv[start_argv][1] == 'q') {
				quiet = 1;
				start_argv++;
			} else if(argv[start_argv][1] == 'f') {

				if(start_argv + 1 < argc)
					path = argv[start_argv + 1];
				else
					return usage(argv);

				start_argv += 2;
			}
		} else
			break;
	}

	if(start_argv >= argc)
		return usage(argv);

	/* check if path of config file has not been passed via command line */
	path = get_config_path(path, pbuf, sizeof(pbuf));

	if(!quiet)
		fprintf(stderr, LOG_PREFIX "config file found: %s\n", path);

	/* Set PROXYCHAINS_CONF_FILE to get proxychains lib to use new config file. */
	setenv(PROXYCHAINS_CONF_FILE_ENV_VAR, path, 1);

	if(quiet)
		setenv(PROXYCHAINS_QUIET_MODE_ENV_VAR, "1", 1);


	// search DLL

	set_own_dir(argv[0]);

	i = 0;

	while(dll_dirs[i]) {
		snprintf(buf, sizeof(buf), "%s/%s", dll_dirs[i], dll_name);
		if(access(buf, R_OK) != -1) {
			prefix = dll_dirs[i];
			break;
		}
		i++;
	}

	if(!prefix) {
		fprintf(stderr, "couldnt locate %s\n", dll_name);
		return EXIT_FAILURE;
	}
	if(!quiet)
		fprintf(stderr, LOG_PREFIX "preloading %s/%s\n", prefix, dll_name);

#ifdef IS_MAC
	putenv("DYLD_FORCE_FLAT_NAMESPACE=1");
#define LD_PRELOAD_ENV "DYLD_INSERT_LIBRARIES"
#define LD_PRELOAD_SEP ":"
#else
#define LD_PRELOAD_ENV "LD_PRELOAD"
/* all historic implementations of BSD and linux dynlinkers seem to support
   space as LD_PRELOAD separator, with colon added only recently.
   we use the old syntax for maximum compat */
#define LD_PRELOAD_SEP " "
#endif
	char *old_val = getenv(LD_PRELOAD_ENV);
	snprintf(buf, sizeof(buf), LD_PRELOAD_ENV "=%s/%s%s%s",
	         prefix, dll_name,
	         /* append previous LD_PRELOAD content, if existent */
	         old_val ? LD_PRELOAD_SEP : "",
	         old_val ? old_val : "");
	putenv(buf);
	execvp(argv[start_argv], &argv[start_argv]);
	perror("proxychains can't load process....");

	return EXIT_FAILURE;
}