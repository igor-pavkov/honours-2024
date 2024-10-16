static bool ldapsrv_call_read_next(struct ldapsrv_connection *conn)
{
	struct tevent_req *subreq;

	if (conn->pending_calls != NULL) {
		conn->limits.endtime = timeval_zero();

		ldapsrv_notification_retry_setup(conn->service, false);
	} else if (timeval_is_zero(&conn->limits.endtime)) {
		conn->limits.endtime =
			timeval_current_ofs(conn->limits.initial_timeout, 0);
	} else {
		conn->limits.endtime =
			timeval_current_ofs(conn->limits.conn_idle_time, 0);
	}

	if (conn->sockets.read_req != NULL) {
		return true;
	}

	/*
	 * The minimum size of a LDAP pdu is 7 bytes
	 *
	 * dumpasn1 -hh ldap-unbind-min.dat
	 *
	 *     <30 05 02 01 09 42 00>
	 *    0    5: SEQUENCE {
	 *     <02 01 09>
	 *    2    1:   INTEGER 9
	 *     <42 00>
	 *    5    0:   [APPLICATION 2]
	 *          :     Error: Object has zero length.
	 *          :   }
	 *
	 * dumpasn1 -hh ldap-unbind-windows.dat
	 *
	 *     <30 84 00 00 00 05 02 01 09 42 00>
	 *    0    5: SEQUENCE {
	 *     <02 01 09>
	 *    6    1:   INTEGER 9
	 *     <42 00>
	 *    9    0:   [APPLICATION 2]
	 *          :     Error: Object has zero length.
	 *          :   }
	 *
	 * This means using an initial read size
	 * of 7 is ok.
	 */
	subreq = tstream_read_pdu_blob_send(conn,
					    conn->connection->event.ctx,
					    conn->sockets.active,
					    7, /* initial_read_size */
					    ldapsrv_packet_check,
					    conn);
	if (subreq == NULL) {
		ldapsrv_terminate_connection(conn, "ldapsrv_call_read_next: "
				"no memory for tstream_read_pdu_blob_send");
		return false;
	}
	if (!timeval_is_zero(&conn->limits.endtime)) {
		bool ok;
		ok = tevent_req_set_endtime(subreq,
					    conn->connection->event.ctx,
					    conn->limits.endtime);
		if (!ok) {
			ldapsrv_terminate_connection(
				conn,
				"ldapsrv_call_read_next: "
				"no memory for tevent_req_set_endtime");
			return false;
		}
	}
	tevent_req_set_callback(subreq, ldapsrv_call_read_done, conn);
	conn->sockets.read_req = subreq;
	return true;
}