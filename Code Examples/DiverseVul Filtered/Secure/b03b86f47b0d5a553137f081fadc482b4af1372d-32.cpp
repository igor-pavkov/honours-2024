static void connection_close(connection *con) {
	if (con->fd < 0) con->fd = -con->fd;

	plugins_call_handle_connection_close(con);

	server * const srv = con->srv;
	request_st * const r = &con->request;
	request_reset_ex(r); /*(r->conf.* is still valid below)*/
	connection_set_state(r, CON_STATE_CONNECT);

	chunkqueue_reset(con->read_queue);
	con->request_count = 0;
	con->is_ssl_sock = 0;
	con->revents_err = 0;

	fdevent_fdnode_event_del(srv->ev, con->fdn);
	fdevent_unregister(srv->ev, con->fd);
	con->fdn = NULL;
#ifdef __WIN32
	if (0 == closesocket(con->fd))
#else
	if (0 == close(con->fd))
#endif
		--srv->cur_fds;
	else
		log_perror(r->conf.errh, __FILE__, __LINE__,
		  "(warning) close: %d", con->fd);

	if (r->conf.log_state_handling) {
		log_error(r->conf.errh, __FILE__, __LINE__,
		  "connection closed for fd %d", con->fd);
	}
	con->fd = -1;

	connection_del(srv, con);
}