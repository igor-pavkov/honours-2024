int main(int argc, char *argv[])
{
	sigset_t sigset;
	int ch;
	int fd;
	int res;
	char *origmnt;
	char *mnt;
	static int unmount = 0;
	static int lazy = 0;
	static int quiet = 0;
	char *commfd;
	int cfd;
	const char *opts = "";

	static const struct option long_opts[] = {
		{"unmount", no_argument, NULL, 'u'},
		{"lazy",    no_argument, NULL, 'z'},
		{"quiet",   no_argument, NULL, 'q'},
		{"help",    no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'V'},
		{0, 0, 0, 0}};

	progname = strdup(argv[0]);
	if (progname == NULL) {
		fprintf(stderr, "%s: failed to allocate memory\n", argv[0]);
		exit(1);
	}

	while ((ch = getopt_long(argc, argv, "hVo:uzq", long_opts,
				 NULL)) != -1) {
		switch (ch) {
		case 'h':
			usage();
			break;

		case 'V':
			show_version();
			break;

		case 'o':
			opts = optarg;
			break;

		case 'u':
			unmount = 1;
			break;

		case 'z':
			lazy = 1;
			break;

		case 'q':
			quiet = 1;
			break;

		default:
			exit(1);
		}
	}

	if (lazy && !unmount) {
		fprintf(stderr, "%s: -z can only be used with -u\n", progname);
		exit(1);
	}

	if (optind >= argc) {
		fprintf(stderr, "%s: missing mountpoint argument\n", progname);
		exit(1);
	} else if (argc > optind + 1) {
		fprintf(stderr, "%s: extra arguments after the mountpoint\n",
			progname);
		exit(1);
	}

	origmnt = argv[optind];

	drop_privs();
	mnt = fuse_mnt_resolve_path(progname, origmnt);
	if (mnt != NULL) {
		res = chdir("/");
		if (res == -1) {
			fprintf(stderr, "%s: failed to chdir to '/'\n", progname);
			goto err_out;
		}
	}
	restore_privs();
	if (mnt == NULL)
		exit(1);

	umask(033);
	if (unmount)
		goto do_unmount;

	commfd = getenv(FUSE_COMMFD_ENV);
	if (commfd == NULL) {
		fprintf(stderr, "%s: old style mounting not supported\n",
			progname);
		goto err_out;
	}

	fd = mount_fuse(mnt, opts);
	if (fd == -1)
		goto err_out;

	cfd = atoi(commfd);
	res = send_fd(cfd, fd);
	if (res == -1)
		goto err_out;
	close(fd);

	if (!auto_unmount) {
		free(mnt);
		return 0;
	}

	/* Become a daemon and wait for the parent to exit or die.
	   ie For the control socket to get closed.
	   btw We don't want to use daemon() function here because
	   it forks and messes with the file descriptors. */
	setsid();
	res = chdir("/");
	if (res == -1) {
		fprintf(stderr, "%s: failed to chdir to '/'\n", progname);
		goto err_out;
	}

	sigfillset(&sigset);
	sigprocmask(SIG_BLOCK, &sigset, NULL);

	lazy  = 1;
	quiet = 1;

	while (1) {
		unsigned char buf[16];
		int n = recv(cfd, buf, sizeof(buf), 0);
		if (!n)
			break;

		if (n < 0) {
			if (errno == EINTR)
				continue;
			break;
		}
	}

do_unmount:
	if (geteuid() == 0)
		res = unmount_fuse(mnt, quiet, lazy);
	else {
		res = umount2(mnt, lazy ? UMOUNT_DETACH : 0);
		if (res == -1 && !quiet)
			fprintf(stderr,
				"%s: failed to unmount %s: %s\n",
				progname, mnt, strerror(errno));
	}
	if (res == -1)
		goto err_out;
	return 0;

err_out:
	free(mnt);
	exit(1);
}