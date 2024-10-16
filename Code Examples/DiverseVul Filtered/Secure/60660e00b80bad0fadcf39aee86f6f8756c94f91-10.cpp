static int pppd_run(struct tunnel *tunnel)
{
	pid_t pid;
	int amaster;
	int slave_stderr;

#ifdef HAVE_STRUCT_TERMIOS
	struct termios termp = {
		.c_cflag = B9600,
		.c_cc[VTIME] = 0,
		.c_cc[VMIN] = 1
	};
#endif

	static const char ppp_path[] = PPP_PATH;
	if (access(ppp_path, F_OK) != 0) {
		log_error("%s: %s.\n", ppp_path, strerror(errno));
		return 1;
	}
	log_debug("ppp_path: %s\n", ppp_path);

	slave_stderr = dup(STDERR_FILENO);

	if (slave_stderr < 0) {
		log_error("slave stderr %s\n", strerror(errno));
		return 1;
	}

#ifdef HAVE_STRUCT_TERMIOS
	pid = forkpty(&amaster, NULL, &termp, NULL);
#else
	pid = forkpty(&amaster, NULL, NULL, NULL);
#endif

	if (pid == 0) { // child process

		struct ofv_varr pppd_args = { 0, 0, NULL };

		dup2(slave_stderr, STDERR_FILENO);
		close(slave_stderr);

#if HAVE_USR_SBIN_PPP
		/*
		 * assume there is a default configuration to start.
		 * Support for taking options from the command line
		 * e.g. the name of the configuration or options
		 * to send interactively to ppp will be added later
		 */
		static const char *const v[] = {
			ppp_path,
			"-direct"
		};
		for (unsigned int i = 0; i < ARRAY_SIZE(v); i++)
			if (ofv_append_varr(&pppd_args, v[i]))
				return 1;
#endif
#if HAVE_USR_SBIN_PPPD
		if (tunnel->config->pppd_call) {
			if (ofv_append_varr(&pppd_args, ppp_path))
				return 1;
			if (ofv_append_varr(&pppd_args, "call"))
				return 1;
			if (ofv_append_varr(&pppd_args, tunnel->config->pppd_call))
				return 1;
		} else {
			static const char *const v[] = {
				ppp_path,
				"115200", // speed
				":192.0.2.1", // <local_IP_address>:<remote_IP_address>
				"noipdefault",
				"noaccomp",
				"noauth",
				"default-asyncmap",
				"nopcomp",
				"receive-all",
				"nodefaultroute",
				"nodetach",
				"lcp-max-configure", "40",
				"mru", "1354"
			};
			for (unsigned int i = 0; i < ARRAY_SIZE(v); i++)
				if (ofv_append_varr(&pppd_args, v[i]))
					return 1;
		}
		if (tunnel->config->pppd_use_peerdns)
			if (ofv_append_varr(&pppd_args, "usepeerdns"))
				return 1;
		if (tunnel->config->pppd_log) {
			if (ofv_append_varr(&pppd_args, "debug"))
				return 1;
			if (ofv_append_varr(&pppd_args, "logfile"))
				return 1;
			if (ofv_append_varr(&pppd_args, tunnel->config->pppd_log))
				return 1;
		} else {
			/*
			 * pppd defaults to logging to fd=1, clobbering the
			 * actual PPP data
			 */
			if (ofv_append_varr(&pppd_args, "logfd"))
				return 1;
			if (ofv_append_varr(&pppd_args, "2"))
				return 1;
		}
		if (tunnel->config->pppd_plugin) {
			if (ofv_append_varr(&pppd_args, "plugin"))
				return 1;
			if (ofv_append_varr(&pppd_args, tunnel->config->pppd_plugin))
				return 1;
		}
		if (tunnel->config->pppd_ipparam) {
			if (ofv_append_varr(&pppd_args, "ipparam"))
				return 1;
			if (ofv_append_varr(&pppd_args, tunnel->config->pppd_ipparam))
				return 1;
		}
		if (tunnel->config->pppd_ifname) {
			if (ofv_append_varr(&pppd_args, "ifname"))
				return 1;
			if (ofv_append_varr(&pppd_args, tunnel->config->pppd_ifname))
				return 1;
		}
#endif
#if HAVE_USR_SBIN_PPP
		if (tunnel->config->ppp_system) {
			if (ofv_append_varr(&pppd_args, tunnel->config->ppp_system))
				return 1;
		}
#endif

		close(tunnel->ssl_socket);
		execv(pppd_args.data[0], (char *const *)pppd_args.data);
		free(pppd_args.data);

		fprintf(stderr, "execvp: %s\n", strerror(errno));
		_exit(EXIT_FAILURE);
	} else {
		close(slave_stderr);
		if (pid == -1) {
			log_error("forkpty: %s\n", strerror(errno));
			return 1;
		}
	}

	// Set non-blocking
	int flags = fcntl(amaster, F_GETFL, 0);
	if (flags == -1)
		flags = 0;
	if (fcntl(amaster, F_SETFL, flags | O_NONBLOCK) == -1) {
		log_error("fcntl: %s\n", strerror(errno));
		return 1;
	}

	tunnel->pppd_pid = pid;
	tunnel->pppd_pty = amaster;

	return 0;
}