static int pppd_terminate(struct tunnel *tunnel)
{
	close(tunnel->pppd_pty);

	log_debug("Waiting for %s to exit...\n", PPP_DAEMON);

	int status;
	if (waitpid(tunnel->pppd_pid, &status, 0) == -1) {
		log_error("waitpid: %s\n", strerror(errno));
		return 1;
	}
	if (WIFEXITED(status)) {
		int exit_status = WEXITSTATUS(status);
		log_debug("waitpid: %s exit status code %d\n",
		          PPP_DAEMON, exit_status);
#if HAVE_USR_SBIN_PPPD
		if (exit_status >= ARRAY_SIZE(pppd_message) || exit_status < 0) {
			log_error("%s: Returned an unknown exit status: %d\n",
			          PPP_DAEMON, exit_status);
		} else {
			switch (exit_status) {
			case 0: // success
				log_debug("%s: %s\n",
				          PPP_DAEMON, pppd_message[exit_status]);
				break;
			case 16: // emitted when exiting normally
				log_info("%s: %s\n",
				         PPP_DAEMON, pppd_message[exit_status]);
				break;
			default:
				log_error("%s: %s\n",
				          PPP_DAEMON, pppd_message[exit_status]);
				break;
			}
		}
#else
		// ppp exit codes in the FreeBSD case
		switch (exit_status) {
		case 0: // success and EX_NORMAL as defined in ppp source directly
			log_debug("%s: %s\n", PPP_DAEMON, pppd_message[exit_status]);
			break;
		case 1:
		case 127:
		case 255: // abnormal exit with hard-coded error codes in ppp
			log_error("%s: exited with return value of %d\n",
			          PPP_DAEMON, exit_status);
			break;
		default:
			log_error("%s: %s (%d)\n", PPP_DAEMON, strerror(exit_status),
			          exit_status);
			break;
		}
#endif
	} else if (WIFSIGNALED(status)) {
		int signal_number = WTERMSIG(status);
		log_debug("waitpid: %s terminated by signal %d\n",
		          PPP_DAEMON, signal_number);
		log_error("%s: terminated by signal: %s\n",
		          PPP_DAEMON, strsignal(signal_number));
	}

	return 0;
}