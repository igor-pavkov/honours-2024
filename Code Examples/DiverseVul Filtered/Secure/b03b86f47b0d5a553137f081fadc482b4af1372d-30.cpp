connection *connection_accept(server *srv, server_socket *srv_socket) {
	int cnt;
	sock_addr cnt_addr;
	size_t cnt_len = sizeof(cnt_addr); /*(size_t intentional; not socklen_t)*/

	/**
	 * check if we can still open a new connections
	 *
	 * see #1216
	 */

	if (srv->conns.used >= srv->max_conns) {
		return NULL;
	}

	cnt = fdevent_accept_listenfd(srv_socket->fd, (struct sockaddr *) &cnt_addr, &cnt_len);
	if (-1 == cnt) {
		switch (errno) {
		case EAGAIN:
#if EWOULDBLOCK != EAGAIN
		case EWOULDBLOCK:
#endif
		case EINTR:
			/* we were stopped _before_ we had a connection */
		case ECONNABORTED: /* this is a FreeBSD thingy */
			/* we were stopped _after_ we had a connection */
			break;
		case EMFILE:
			/* out of fds */
			break;
		default:
			log_perror(srv->errh, __FILE__, __LINE__, "accept failed");
		}
		return NULL;
	} else {
		if (sock_addr_get_family(&cnt_addr) != AF_UNIX) {
			network_accept_tcp_nagle_disable(cnt);
		}
		return connection_accepted(srv, srv_socket, &cnt_addr, cnt);
	}
}