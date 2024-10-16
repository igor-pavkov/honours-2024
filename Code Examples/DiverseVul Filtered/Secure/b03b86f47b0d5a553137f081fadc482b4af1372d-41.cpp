static void connection_handle_shutdown(connection *con) {
	plugins_call_handle_connection_shut_wr(con);

	connection_reset(con);
	++con->srv->con_closed;

	/* close the connection */
	if (con->fd >= 0
	    && (con->is_ssl_sock || 0 == shutdown(con->fd, SHUT_WR))) {
		con->close_timeout_ts = log_epoch_secs;

		request_st * const r = &con->request;
		connection_set_state(r, CON_STATE_CLOSE);
		if (r->conf.log_state_handling) {
			log_error(r->conf.errh, __FILE__, __LINE__,
			  "shutdown for fd %d", con->fd);
		}
	} else {
		connection_close(con);
	}
}