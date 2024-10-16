static void ldapsrv_accept(struct stream_connection *c,
			   struct auth_session_info *session_info,
			   bool is_privileged)
{
	struct ldapsrv_service *ldapsrv_service = 
		talloc_get_type(c->private_data, struct ldapsrv_service);
	struct ldapsrv_connection *conn;
	struct cli_credentials *server_credentials;
	struct socket_address *socket_address;
	NTSTATUS status;
	int port;
	int ret;
	struct tevent_req *subreq;
	struct timeval endtime;
	char *errstring = NULL;

	conn = talloc_zero(c, struct ldapsrv_connection);
	if (!conn) {
		stream_terminate_connection(c, "ldapsrv_accept: out of memory");
		return;
	}
	conn->is_privileged = is_privileged;

	conn->sockets.send_queue = tevent_queue_create(conn, "ldapsev send queue");
	if (conn->sockets.send_queue == NULL) {
		stream_terminate_connection(c,
					    "ldapsrv_accept: tevent_queue_create failed");
		return;
	}

	TALLOC_FREE(c->event.fde);

	ret = tstream_bsd_existing_socket(conn,
					  socket_get_fd(c->socket),
					  &conn->sockets.raw);
	if (ret == -1) {
		stream_terminate_connection(c,
					    "ldapsrv_accept: out of memory");
		return;
	}
	socket_set_flags(c->socket, SOCKET_FLAG_NOCLOSE);

	conn->connection  = c;
	conn->service     = ldapsrv_service;
	conn->lp_ctx      = ldapsrv_service->task->lp_ctx;

	c->private_data   = conn;

	socket_address = socket_get_my_addr(c->socket, conn);
	if (!socket_address) {
		ldapsrv_terminate_connection(conn, "ldapsrv_accept: failed to obtain local socket address!");
		return;
	}
	port = socket_address->port;
	talloc_free(socket_address);
	if (port == 3268 || port == 3269) /* Global catalog */ {
		conn->global_catalog = true;
	}

	server_credentials = cli_credentials_init(conn);
	if (!server_credentials) {
		stream_terminate_connection(c, "Failed to init server credentials\n");
		return;
	}

	cli_credentials_set_conf(server_credentials, conn->lp_ctx);
	status = cli_credentials_set_machine_account(server_credentials, conn->lp_ctx);
	if (!NT_STATUS_IS_OK(status)) {
		stream_terminate_connection(c, talloc_asprintf(conn, "Failed to obtain server credentials, perhaps a standalone server?: %s\n", nt_errstr(status)));
		return;
	}
	conn->server_credentials = server_credentials;

	conn->session_info = session_info;

	conn->sockets.active = conn->sockets.raw;

	if (conn->is_privileged) {
		conn->require_strong_auth = LDAP_SERVER_REQUIRE_STRONG_AUTH_NO;
	} else {
		conn->require_strong_auth = lpcfg_ldap_server_require_strong_auth(conn->lp_ctx);
	}

	ret = ldapsrv_backend_Init(conn, &errstring);
	if (ret != LDB_SUCCESS) {
		char *reason = talloc_asprintf(conn,
					       "LDB backend for LDAP Init "
					       "failed: %s: %s",
					       errstring, ldb_strerror(ret));
		ldapsrv_terminate_connection(conn, reason);
		return;
	}

	/* load limits from the conf partition */
	ldapsrv_load_limits(conn); /* should we fail on error ? */

	/* register the server */	
	irpc_add_name(c->msg_ctx, "ldap_server");

	DLIST_ADD_END(ldapsrv_service->connections, conn);

	if (port != 636 && port != 3269) {
		ldapsrv_call_read_next(conn);
		return;
	}

	endtime = timeval_current_ofs(conn->limits.conn_idle_time, 0);

	subreq = tstream_tls_accept_send(conn,
					 conn->connection->event.ctx,
					 conn->sockets.raw,
					 conn->service->tls_params);
	if (subreq == NULL) {
		ldapsrv_terminate_connection(conn, "ldapsrv_accept: "
				"no memory for tstream_tls_accept_send");
		return;
	}
	tevent_req_set_endtime(subreq,
			       conn->connection->event.ctx,
			       endtime);
	tevent_req_set_callback(subreq, ldapsrv_accept_tls_done, conn);
}