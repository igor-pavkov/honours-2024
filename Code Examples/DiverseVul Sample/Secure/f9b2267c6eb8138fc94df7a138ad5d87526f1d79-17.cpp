static void ldapsrv_call_read_done(struct tevent_req *subreq)
{
	struct ldapsrv_connection *conn =
		tevent_req_callback_data(subreq,
		struct ldapsrv_connection);
	NTSTATUS status;
	struct ldapsrv_call *call;
	struct asn1_data *asn1;
	DATA_BLOB blob;
	int ret = LDAP_SUCCESS;
	struct ldap_request_limits limits = {0};

	conn->sockets.read_req = NULL;

	call = talloc_zero(conn, struct ldapsrv_call);
	if (!call) {
		ldapsrv_terminate_connection(conn, "no memory");
		return;
	}
	talloc_set_destructor(call, ldapsrv_call_destructor);

	call->conn = conn;

	status = tstream_read_pdu_blob_recv(subreq,
					    call,
					    &blob);
	TALLOC_FREE(subreq);
	if (!NT_STATUS_IS_OK(status)) {
		const char *reason;

		reason = talloc_asprintf(call, "ldapsrv_call_loop: "
					 "tstream_read_pdu_blob_recv() - %s",
					 nt_errstr(status));
		if (!reason) {
			reason = nt_errstr(status);
		}

		ldapsrv_terminate_connection(conn, reason);
		return;
	}

	ret = ldapsrv_check_packet_size(conn, blob.length);
	if (ret != LDAP_SUCCESS) {
		ldapsrv_terminate_connection(
			conn,
			"Request packet too large");
		return;
	}

	asn1 = asn1_init(call, ASN1_MAX_TREE_DEPTH);
	if (asn1 == NULL) {
		ldapsrv_terminate_connection(conn, "no memory");
		return;
	}

	call->request = talloc(call, struct ldap_message);
	if (call->request == NULL) {
		ldapsrv_terminate_connection(conn, "no memory");
		return;
	}

	asn1_load_nocopy(asn1, blob.data, blob.length);

	limits.max_search_size =
		lpcfg_ldap_max_search_request_size(conn->lp_ctx);
	status = ldap_decode(
		asn1,
		&limits,
		samba_ldap_control_handlers(),
		call->request);
	if (!NT_STATUS_IS_OK(status)) {
		ldapsrv_terminate_connection(conn, nt_errstr(status));
		return;
	}

	data_blob_free(&blob);
	TALLOC_FREE(asn1);


	/* queue the call in the global queue */
	subreq = ldapsrv_process_call_send(call,
					   conn->connection->event.ctx,
					   conn->service->call_queue,
					   call);
	if (subreq == NULL) {
		ldapsrv_terminate_connection(conn, "ldapsrv_process_call_send failed");
		return;
	}
	tevent_req_set_callback(subreq, ldapsrv_call_process_done, call);
	conn->active_call = subreq;
}