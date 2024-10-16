static int dns_connect_namesaver(struct dns_nameserver *ns)
{
	struct dgram_conn *dgram = ns->dgram;
	int fd;

	/* Already connected */
	if (dgram->t.sock.fd != -1)
		return 0;

	/* Create an UDP socket and connect it on the nameserver's IP/Port */
	if ((fd = socket(ns->addr.ss_family, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		send_log(NULL, LOG_WARNING,
			 "DNS : resolvers '%s': can't create socket for nameserver '%s'.\n",
			 ns->resolvers->id, ns->id);
		return -1;
	}
	if (connect(fd, (struct sockaddr*)&ns->addr, get_addr_len(&ns->addr)) == -1) {
		send_log(NULL, LOG_WARNING,
			 "DNS : resolvers '%s': can't connect socket for nameserver '%s'.\n",
			 ns->resolvers->id, ns->id);
		close(fd);
		return -1;
	}

	/* Make the socket non blocking */
	fcntl(fd, F_SETFL, O_NONBLOCK);

	/* Add the fd in the fd list and update its parameters */
	dgram->t.sock.fd = fd;
	fd_insert(fd, dgram, dgram_fd_handler, MAX_THREADS_MASK);
	fd_want_recv(fd);
	return 0;
}