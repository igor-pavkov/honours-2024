void main_loop(void)
{
	struct pollfd fds[2];

	memset(fds, 0, sizeof(fds));

	fds[0].fd = sock;
	fds[0].events = POLLIN;
	fds[0].revents = 0;

#if HAVE_NETLINK
	fds[1].fd = netlink_socket();
	fds[1].events = POLLIN;
	fds[1].revents = 0;
#else
	fds[1].fd = -1;
	fds[1].events = 0;
	fds[1].revents = 0;
#endif

	for (;;) {
		struct Interface *next = NULL;
		struct Interface *iface;
		int timeout = -1;
		int rc;

		if (IfaceList) {
			timeout = next_time_msec(IfaceList);
			next = IfaceList;
			for (iface = IfaceList; iface; iface = iface->next) {
				int t;
				t = next_time_msec(iface);
				if (timeout > t) {
					timeout = t;
					next = iface;
				}
			}
		}

		dlog(LOG_DEBUG, 5, "polling for %g seconds.", timeout/1000.0);

		rc = poll(fds, sizeof(fds)/sizeof(fds[0]), timeout);

		if (rc > 0) {
			if (fds[0].revents & (POLLERR | POLLHUP | POLLNVAL)) {
				flog(LOG_WARNING, "socket error on fds[0].fd");
			}
			else if (fds[0].revents & POLLIN) {
				int len, hoplimit;
				struct sockaddr_in6 rcv_addr;
				struct in6_pktinfo *pkt_info = NULL;
				unsigned char msg[MSG_SIZE_RECV];

				len = recv_rs_ra(msg, &rcv_addr, &pkt_info, &hoplimit);
				if (len > 0) {
					process(IfaceList, msg, len,
						&rcv_addr, pkt_info, hoplimit);
				}
			}
#ifdef HAVE_NETLINK
			if (fds[1].revents & (POLLERR | POLLHUP | POLLNVAL)) {
				flog(LOG_WARNING, "socket error on fds[1].fd");
			}
			else if (fds[1].revents & POLLIN) {
				process_netlink_msg(fds[1].fd);
			}
#endif
		}
		else if ( rc == 0 ) {
			if (next)
				timer_handler(next);
		}
		else if ( rc == -1 ) {
			flog(LOG_ERR, "poll error: %s", strerror(errno));
		}

		if (sigterm_received || sigint_received) {
			flog(LOG_WARNING, "Exiting, sigterm or sigint received.\n");
			break;
		}

		if (sighup_received)
		{
			reload_config();
			sighup_received = 0;
		}

		if (sigusr1_received)
		{
			reset_prefix_lifetimes();
			sigusr1_received = 0;
		}

	}
}