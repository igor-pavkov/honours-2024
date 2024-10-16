static void connection_handle_response_end_state(request_st * const r, connection * const con) {
	if (r->http_version > HTTP_VERSION_1_1) {
		h2_retire_con(r, con);
		r->keep_alive = 0;
		/* set a status so that mod_accesslog, mod_rrdtool hooks are called
		 * in plugins_call_handle_request_done() (XXX: or set to 0 to omit) */
		r->http_status = 100; /* XXX: what if con->state == CON_STATE_ERROR? */
	}

	/* call request_done hook if http_status set (e.g. to log request) */
	/* (even if error, connection dropped, as long as http_status is set) */
	if (r->http_status) plugins_call_handle_request_done(r);

	if (r->state != CON_STATE_ERROR) ++con->srv->con_written;

	if (r->reqbody_length != r->reqbody_queue.bytes_in
	    || r->state == CON_STATE_ERROR) {
		/* request body may not have been read completely */
		r->keep_alive = 0;
		/* clean up failed partial write of 1xx intermediate responses*/
		if (&r->write_queue != con->write_queue) { /*(for HTTP/1.1)*/
			chunkqueue_free(con->write_queue);
			con->write_queue = &r->write_queue;
		}
	}

        if (r->keep_alive) {
		request_reset(r);
		config_reset_config(r);
		con->is_readable = 1; /* potentially trigger optimistic read */
		/*(accounting used by mod_accesslog for HTTP/1.0 and HTTP/1.1)*/
		r->bytes_read_ckpt = con->bytes_read;
		r->bytes_written_ckpt = con->bytes_written;
#if 0
		r->start_hp.tv_sec = con->read_idle_ts = log_epoch_secs;
#endif
		connection_set_state(r, CON_STATE_REQUEST_START);
	} else {
		connection_handle_shutdown(con);
	}
}