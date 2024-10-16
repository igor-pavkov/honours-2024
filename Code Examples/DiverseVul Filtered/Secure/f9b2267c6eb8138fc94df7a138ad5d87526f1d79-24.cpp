static void ldapsrv_process_call_trigger(struct tevent_req *req,
					 void *private_data)
{
	struct ldapsrv_process_call_state *state =
		tevent_req_data(req,
		struct ldapsrv_process_call_state);
	struct ldapsrv_connection *conn = state->call->conn;
	NTSTATUS status;

	if (conn->deferred_expire_disconnect != NULL) {
		/*
		 * Just drop this on the floor
		 */
		tevent_req_done(req);
		return;
	}

	/* make the call */
	status = ldapsrv_do_call(state->call);

	if (NT_STATUS_EQUAL(status, NT_STATUS_NETWORK_SESSION_EXPIRED)) {
		/*
		 * For testing purposes, defer the TCP disconnect
		 * after having sent the msgid 0
		 * 1.3.6.1.4.1.1466.20036 exop response. LDAP clients
		 * should not wait for the TCP connection to close but
		 * handle this packet equivalent to a TCP
		 * disconnect. This delay enables testing both cases
		 * in LDAP client libraries.
		 */

		int defer_msec = lpcfg_parm_int(
			conn->lp_ctx,
			NULL,
			"ldap_server",
			"delay_expire_disconnect",
			0);

		conn->deferred_expire_disconnect = tevent_wakeup_send(
			conn,
			conn->connection->event.ctx,
			timeval_current_ofs_msec(defer_msec));
		if (tevent_req_nomem(conn->deferred_expire_disconnect, req)) {
			return;
		}
		tevent_req_set_callback(
			conn->deferred_expire_disconnect,
			ldapsrv_disconnect_ticket_expired,
			conn);

		tevent_req_done(req);
		return;
	}

	if (!NT_STATUS_IS_OK(status)) {
		tevent_req_nterror(req, status);
		return;
	}

	tevent_req_done(req);
}