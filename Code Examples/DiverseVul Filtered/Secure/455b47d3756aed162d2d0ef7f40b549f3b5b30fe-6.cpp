ctrl_t *new_session(uev_ctx_t *ctx, int sd, int *rc)
{
	ctrl_t *ctrl = NULL;
	static int privs_dropped = 0;

	if (!inetd) {
		pid_t pid = fork();

		if (pid) {
			DBG("Created new client session as PID %d", pid);
			*rc = pid;
			return NULL;
		}

		/*
		 * Set process group to parent, so uftpd can call
		 * killpg() on all of us when it exits.
		 */
		setpgid(0, getppid());
		/* Create new uEv context for the child. */
		ctx = calloc(1, sizeof(uev_ctx_t));
		if (!ctx) {
			ERR(errno, "Failed allocating session event context");
			exit(1);
		}

		uev_init(ctx);
	}

	ctrl = calloc(1, sizeof(ctrl_t));
	if (!ctrl) {
		ERR(errno, "Failed allocating session context");
		goto fail;
	}

	ctrl->sd = set_nonblock(sd);
	ctrl->ctx = ctx;
	strlcpy(ctrl->cwd, "/", sizeof(ctrl->cwd));

	/* Chroot to FTP root */
	if (!chrooted && geteuid() == 0) {
		if (chroot(home) || chdir("/")) {
			ERR(errno, "Failed chrooting to FTP root, %s, aborting", home);
			goto fail;
		}
		chrooted = 1;
	} else if (!chrooted) {
		if (chdir(home)) {
			WARN(errno, "Failed changing to FTP root, %s, aborting", home);
			goto fail;
		}
	}

	/* If ftp user exists and we're running as root we can drop privs */
	if (!privs_dropped && pw && geteuid() == 0) {
		int fail1, fail2;

		initgroups(pw->pw_name, pw->pw_gid);
		if ((fail1 = setegid(pw->pw_gid)))
			WARN(errno, "Failed dropping group privileges to gid %d", pw->pw_gid);
		if ((fail2 = seteuid(pw->pw_uid)))
			WARN(errno, "Failed dropping user privileges to uid %d", pw->pw_uid);

		setenv("HOME", pw->pw_dir, 1);

		if (!fail1 && !fail2)
			INFO("Successfully dropped privilges to %d:%d (uid:gid)", pw->pw_uid, pw->pw_gid);

		/*
		 * Check we don't have write access to the FTP root,
		 * unless explicitly allowed
		 */
		if (!do_insecure && !access(home, W_OK)) {
			ERR(0, "FTP root %s writable, possible security violation, aborting session!", home);
			goto fail;
		}

		/* On failure, we tried at least.  Only warn once. */
		privs_dropped = 1;
	}

	/* Session timeout handler */
	uev_timer_init(ctrl->ctx, &ctrl->timeout_watcher, inactivity_cb, ctrl->ctx, INACTIVITY_TIMER, 0);

	return ctrl;
fail:
	if (ctrl)
		free(ctrl);
	if (!inetd)
		free(ctx);
	*rc = -1;

	return NULL;
}