static void spawn_ssl_client(const char *host, int network_fd, int flags)
{
	int sp[2];
	int pid;
	char *servername, *p;

	if (!(option_mask32 & WGET_OPT_NO_CHECK_CERT)) {
		option_mask32 |= WGET_OPT_NO_CHECK_CERT;
		bb_simple_error_msg("note: TLS certificate validation not implemented");
	}

	servername = xstrdup(host);
	p = strrchr(servername, ':');
	if (p) *p = '\0';

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sp) != 0)
		/* Kernel can have AF_UNIX support disabled */
		bb_simple_perror_msg_and_die("socketpair");

	fflush_all();
	pid = BB_MMU ? xfork() : xvfork();
	if (pid == 0) {
		/* Child */
		close(sp[0]);
		xmove_fd(sp[1], 0);
		xdup2(0, 1);
		if (BB_MMU) {
			tls_state_t *tls = new_tls_state();
			tls->ifd = tls->ofd = network_fd;
			tls_handshake(tls, servername);
			tls_run_copy_loop(tls, flags);
			exit(0);
		} else {
			char *argv[6];

			xmove_fd(network_fd, 3);
			argv[0] = (char*)"ssl_client";
			argv[1] = (char*)"-s3";
			//TODO: if (!is_ip_address(servername))...
			argv[2] = (char*)"-n";
			argv[3] = servername;
			argv[4] = (flags & TLSLOOP_EXIT_ON_LOCAL_EOF ? (char*)"-e" : NULL);
			argv[5] = NULL;
			BB_EXECVP(argv[0], argv);
			bb_perror_msg_and_die("can't execute '%s'", argv[0]);
		}
		/* notreached */
	}

	/* Parent */
	free(servername);
	close(sp[1]);
	xmove_fd(sp[0], network_fd);
}