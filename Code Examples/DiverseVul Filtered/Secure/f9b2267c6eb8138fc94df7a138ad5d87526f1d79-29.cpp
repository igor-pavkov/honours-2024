static void ldapsrv_accept_tls_done(struct tevent_req *subreq)
{
	struct ldapsrv_connection *conn =
		tevent_req_callback_data(subreq,
		struct ldapsrv_connection);
	int ret;
	int sys_errno;

	ret = tstream_tls_accept_recv(subreq, &sys_errno,
				      conn, &conn->sockets.tls);
	TALLOC_FREE(subreq);
	if (ret == -1) {
		const char *reason;

		reason = talloc_asprintf(conn, "ldapsrv_accept_tls_loop: "
					 "tstream_tls_accept_recv() - %d:%s",
					 sys_errno, strerror(sys_errno));
		if (!reason) {
			reason = "ldapsrv_accept_tls_loop: "
				 "tstream_tls_accept_recv() - failed";
		}

		ldapsrv_terminate_connection(conn, reason);
		return;
	}

	conn->sockets.active = conn->sockets.tls;
	conn->referral_scheme = LDAP_REFERRAL_SCHEME_LDAPS;
	ldapsrv_call_read_next(conn);
}