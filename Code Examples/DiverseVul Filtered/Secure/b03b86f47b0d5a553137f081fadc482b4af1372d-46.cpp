static int connection_handle_write(request_st * const r, connection * const con) {
	/*assert(!chunkqueue_is_empty(cq));*//* checked by callers */

	if (con->is_writable <= 0) return CON_STATE_WRITE;
	int rc = connection_write_chunkqueue(con, con->write_queue, MAX_WRITE_LIMIT);
	switch (rc) {
	case 0:
		if (r->resp_body_finished) {
			connection_set_state(r, CON_STATE_RESPONSE_END);
			return CON_STATE_RESPONSE_END;
		}
		break;
	case -1: /* error on our side */
		log_error(r->conf.errh, __FILE__, __LINE__,
		  "connection closed: write failed on fd %d", con->fd);
		connection_set_state_error(r, CON_STATE_ERROR);
		return CON_STATE_ERROR;
	case -2: /* remote close */
		connection_set_state_error(r, CON_STATE_ERROR);
		return CON_STATE_ERROR;
	case 1:
		/* do not spin trying to send HTTP/2 server Connection Preface
		 * while waiting for TLS negotiation to complete */
		if (con->write_queue->bytes_out)
			con->is_writable = 0;

		/* not finished yet -> WRITE */
		break;
	}

	return CON_STATE_WRITE; /*(state did not change)*/
}