connection *connection_accepted(server *srv, server_socket *srv_socket, sock_addr *cnt_addr, int cnt) {
		connection *con;

		srv->cur_fds++;

		/* ok, we have the connection, register it */
#if 0
		log_error(srv->errh, __FILE__, __LINE__, "accepted() %d", cnt);
#endif
		srv->con_opened++;

		con = connections_get_new_connection(srv);

		con->fd = cnt;
		con->fdn = fdevent_register(srv->ev, con->fd, connection_handle_fdevent, con);
		con->network_read = connection_read_cq;
		con->network_write = connection_write_cq;
		con->reqbody_read = connection_handle_read_post_state;

		request_st * const r = &con->request;
		connection_set_state(r, CON_STATE_REQUEST_START);

		con->connection_start = log_epoch_secs;
		con->dst_addr = *cnt_addr;
		sock_addr_cache_inet_ntop_copy_buffer(con->dst_addr_buf,&con->dst_addr);
		con->srv_socket = srv_socket;
		con->is_ssl_sock = srv_socket->is_ssl;
		con->proto_default_port = 80; /* "http" */

		config_cond_cache_reset(r);
		r->conditional_is_valid = (1 << COMP_SERVER_SOCKET)
		                        | (1 << COMP_HTTP_REMOTE_IP);

		if (HANDLER_GO_ON != plugins_call_handle_connection_accept(con)) {
			connection_reset(con);
			connection_close(con);
			return NULL;
		}
		if (r->http_status < 0) connection_set_state(r, CON_STATE_WRITE);
		return con;
}