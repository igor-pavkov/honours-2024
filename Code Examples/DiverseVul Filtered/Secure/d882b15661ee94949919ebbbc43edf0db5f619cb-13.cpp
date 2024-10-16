int connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
	struct sockaddr_in sin;
	int result, plug, port;
	test_param_t *test;

	test = get_test_param();

#ifdef GG_CONFIG_HAVE_GNUTLS
	/* GnuTLS may want to connect */
	if (!gnutls_initialized)
		return __connect(socket, address, address_len);
#endif

	if (address_len < sizeof(sin)) {
		debug("Invalid argument for connect(): sa_len < %d\n", sizeof(sin));
		errno = EINVAL;
		return -1;
	}

	memcpy(&sin, address, address_len);

	if (sin.sin_family != AF_INET) {
		debug("Invalid argument for connect(): sa_family = %d\n", sin.sin_family);
		errno = EINVAL;
		return -1;
	}

	if (sin.sin_addr.s_addr != inet_addr(HOST_LOCAL)) {
		debug("Invalid argument for connect(): sin_addr = %s\n", inet_ntoa(sin.sin_addr));
		errno = EINVAL;
		return -1;
	}

	if (ntohs(sin.sin_port) != 8080)
		test->tried_non_8080 = 1;

	switch (ntohs(sin.sin_port)) {
		case 80:
			plug = test->plug_80;
			port = server_ports[PORT_80];
			test->tried_80 = 1;
			break;
		case 443:
			plug = test->plug_443;
			port = server_ports[PORT_443];
			test->tried_443 = 1;
			break;
		case 8074:
			plug = test->plug_8074;
			port = server_ports[PORT_8074];
			test->tried_8074 = 1;
			break;
		case 8080:
			plug = test->plug_8080;
			port = server_ports[PORT_8080];
			test->tried_8080 = 1;
			break;
		default:
			debug("Invalid argument for connect(): sin_port = %d\n", ntohs(sin.sin_port));
			errno = EINVAL;
			return -1;
	}

	if (test->proxy_mode && ntohs(sin.sin_port) != 8080)
		plug = PLUG_RESET;

	switch (plug) {
		case PLUG_NONE:
			sin.sin_port = htons(port);
			break;
		case PLUG_RESET:
			sin.sin_port = htons(server_ports[PORT_CLOSED]);
			break;
		case PLUG_TIMEOUT:
			if (!test->async_mode) {
				errno = ETIMEDOUT;
			} else {
				int res;
				if ((res = write(timeout_pipe[1], "", 1)) != 1) {
					debug("write() returned %d\n", res);
					errno = EBADF;
					return -1;
				}
				errno = EINPROGRESS;
			}
			return -1;
	}

	result = __connect(socket, (struct sockaddr*) &sin, address_len);

	return result;
}