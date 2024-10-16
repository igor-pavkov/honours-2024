static void configure(
	struct recvbuf *rbufp,
	int restrict_mask
	)
{
	size_t data_count;
	int retval;

	/* I haven't yet implemented changes to an existing association.
	 * Hence check if the association id is 0
	 */
	if (res_associd != 0) {
		ctl_error(CERR_BADVALUE);
		return;
	}

	if (RES_NOMODIFY & restrict_mask) {
		snprintf(remote_config.err_msg,
			 sizeof(remote_config.err_msg),
			 "runtime configuration prohibited by restrict ... nomodify");
		ctl_putdata(remote_config.err_msg,
			    strlen(remote_config.err_msg), 0);
		ctl_flushpkt(0);
		NLOG(NLOG_SYSINFO)
			msyslog(LOG_NOTICE,
				"runtime config from %s rejected due to nomodify restriction",
				stoa(&rbufp->recv_srcadr));
		sys_restricted++;
		return;
	}

	/* Initialize the remote config buffer */
	data_count = remoteconfig_cmdlength(reqpt, reqend);

	if (data_count > sizeof(remote_config.buffer) - 2) {
		snprintf(remote_config.err_msg,
			 sizeof(remote_config.err_msg),
			 "runtime configuration failed: request too long");
		ctl_putdata(remote_config.err_msg,
			    strlen(remote_config.err_msg), 0);
		ctl_flushpkt(0);
		msyslog(LOG_NOTICE,
			"runtime config from %s rejected: request too long",
			stoa(&rbufp->recv_srcadr));
		return;
	}
	/* Bug 2853 -- check if all characters were acceptable */
	if (data_count != (size_t)(reqend - reqpt)) {
		snprintf(remote_config.err_msg,
			 sizeof(remote_config.err_msg),
			 "runtime configuration failed: request contains an unprintable character");
		ctl_putdata(remote_config.err_msg,
			    strlen(remote_config.err_msg), 0);
		ctl_flushpkt(0);
		msyslog(LOG_NOTICE,
			"runtime config from %s rejected: request contains an unprintable character: %0x",
			stoa(&rbufp->recv_srcadr),
			reqpt[data_count]);
		return;
	}

	memcpy(remote_config.buffer, reqpt, data_count);
	/* The buffer has no trailing linefeed or NUL right now. For
	 * logging, we do not want a newline, so we do that first after
	 * adding the necessary NUL byte.
	 */
	remote_config.buffer[data_count] = '\0';
	DPRINTF(1, ("Got Remote Configuration Command: %s\n",
		remote_config.buffer));
	msyslog(LOG_NOTICE, "%s config: %s",
		stoa(&rbufp->recv_srcadr),
		remote_config.buffer);

	/* Now we have to make sure there is a NL/NUL sequence at the
	 * end of the buffer before we parse it.
	 */
	remote_config.buffer[data_count++] = '\n';
	remote_config.buffer[data_count] = '\0';
	remote_config.pos = 0;
	remote_config.err_pos = 0;
	remote_config.no_errors = 0;
	config_remotely(&rbufp->recv_srcadr);

	/*
	 * Check if errors were reported. If not, output 'Config
	 * Succeeded'.  Else output the error count.  It would be nice
	 * to output any parser error messages.
	 */
	if (0 == remote_config.no_errors) {
		retval = snprintf(remote_config.err_msg,
				  sizeof(remote_config.err_msg),
				  "Config Succeeded");
		if (retval > 0)
			remote_config.err_pos += retval;
	}

	ctl_putdata(remote_config.err_msg, remote_config.err_pos, 0);
	ctl_flushpkt(0);

	DPRINTF(1, ("Reply: %s\n", remote_config.err_msg));

	if (remote_config.no_errors > 0)
		msyslog(LOG_NOTICE, "%d error in %s config",
			remote_config.no_errors,
			stoa(&rbufp->recv_srcadr));
}