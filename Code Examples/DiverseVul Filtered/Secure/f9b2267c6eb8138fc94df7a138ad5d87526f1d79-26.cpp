static void ldapsrv_call_writev_start(struct ldapsrv_call *call)
{
	struct ldapsrv_connection *conn = call->conn;
	struct ldapsrv_reply *reply = NULL;
	struct tevent_req *subreq = NULL;
	size_t length = 0;
	size_t i;

	call->iov_count = 0;

	/* build all the replies into an IOV (no copy) */
	for (reply = call->replies;
	     reply != NULL;
	     reply = reply->next) {

		/* Cap output at 25MB per writev() */
		if (length > length + reply->blob.length
		    || length + reply->blob.length > LDAP_SERVER_MAX_CHUNK_SIZE) {
			break;
		}

		/*
		 * Overflow is harmless here, just used below to
		 * decide if to read or write, but checked above anyway
		 */
		length += reply->blob.length;

		/*
		 * At worst an overflow would mean we send less
		 * replies
		 */
		call->iov_count++;
	}

	if (length == 0) {
		if (!call->notification.busy) {
			TALLOC_FREE(call);
		}

		ldapsrv_call_read_next(conn);
		return;
	}

	/* Cap call->iov_count at IOV_MAX */
	call->iov_count = MIN(call->iov_count, IOV_MAX);

	call->out_iov = talloc_array(call,
				     struct iovec,
				     call->iov_count);
	if (!call->out_iov) {
		/* This is not ideal */
		ldapsrv_terminate_connection(conn,
					     "failed to allocate "
					     "iovec array");
		return;
	}

	/* We may have had to cap the number of replies at IOV_MAX */
	for (i = 0;
	     i < call->iov_count && call->replies != NULL;
	     i++) {
		reply = call->replies;
		call->out_iov[i].iov_base = reply->blob.data;
		call->out_iov[i].iov_len = reply->blob.length;

		/* Keep only the ASN.1 encoded data */
		talloc_steal(call->out_iov, reply->blob.data);

		DLIST_REMOVE(call->replies, reply);
		TALLOC_FREE(reply);
	}

	if (i > call->iov_count) {
		/* This is not ideal, but also (essentially) impossible */
		ldapsrv_terminate_connection(conn,
					     "call list ended"
					     "before iov_count");
		return;
	}

	subreq = tstream_writev_queue_send(call,
					   conn->connection->event.ctx,
					   conn->sockets.active,
					   conn->sockets.send_queue,
					   call->out_iov, call->iov_count);
	if (subreq == NULL) {
		ldapsrv_terminate_connection(conn, "stream_writev_queue_send failed");
		return;
	}
	tevent_req_set_callback(subreq, ldapsrv_call_writev_done, call);
}