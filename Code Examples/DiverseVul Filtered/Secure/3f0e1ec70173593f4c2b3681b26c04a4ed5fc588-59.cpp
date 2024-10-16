static int h2_wake(struct connection *conn)
{
	struct h2c *h2c = conn->mux_ctx;
	struct session *sess = conn->owner;

	if (h2c->dbuf->i && !(h2c->flags & H2_CF_DEM_BLOCK_ANY)) {
		h2_process_demux(h2c);

		if (h2c->st0 >= H2_CS_ERROR || conn->flags & CO_FL_ERROR)
			h2c->dbuf->i = 0;

		if (h2c->dbuf->i != h2c->dbuf->size)
			h2c->flags &= ~H2_CF_DEM_DFULL;
	}

	if (sess && unlikely(sess->fe->state == PR_STSTOPPED)) {
		/* frontend is stopping, reload likely in progress, let's try
		 * to announce a graceful shutdown if not yet done. We don't
		 * care if it fails, it will be tried again later.
		 */
		if (!(h2c->flags & (H2_CF_GOAWAY_SENT|H2_CF_GOAWAY_FAILED))) {
			if (h2c->last_sid < 0)
				h2c->last_sid = (1U << 31) - 1;
			h2c_send_goaway_error(h2c, NULL);
		}
	}

	/*
	 * If we received early data, and the handshake is done, wake
	 * any stream that was waiting for it.
	 */
	if (!(h2c->flags & H2_CF_WAIT_FOR_HS) &&
	    (conn->flags & (CO_FL_EARLY_SSL_HS | CO_FL_HANDSHAKE | CO_FL_EARLY_DATA)) == CO_FL_EARLY_DATA) {
		struct eb32_node *node;
		struct h2s *h2s;

		h2c->flags |= H2_CF_WAIT_FOR_HS;
		node = eb32_lookup_ge(&h2c->streams_by_id, 1);

		while (node) {
			h2s = container_of(node, struct h2s, by_id);
			if (h2s->cs->flags & CS_FL_WAIT_FOR_HS)
				h2s->cs->data_cb->wake(h2s->cs);
			node = eb32_next(node);
		}
	}

	if (conn->flags & CO_FL_ERROR || conn_xprt_read0_pending(conn) ||
	    h2c->st0 == H2_CS_ERROR2 || h2c->flags & H2_CF_GOAWAY_FAILED ||
	    (eb_is_empty(&h2c->streams_by_id) && h2c->last_sid >= 0 &&
	     h2c->max_id >= h2c->last_sid)) {
		h2_wake_some_streams(h2c, 0, 0);

		if (eb_is_empty(&h2c->streams_by_id)) {
			/* no more stream, kill the connection now */
			h2_release(conn);
			return -1;
		}
		else {
			/* some streams still there, we need to signal them all and
			 * wait for their departure.
			 */
			__conn_xprt_stop_recv(conn);
			__conn_xprt_stop_send(conn);
			return 0;
		}
	}

	if (!h2c->dbuf->i)
		h2_release_buf(h2c, &h2c->dbuf);

	/* stop being notified of incoming data if we can't process them */
	if (!h2_recv_allowed(h2c)) {
		__conn_xprt_stop_recv(conn);
	}
	else {
		__conn_xprt_want_recv(conn);
	}

	/* adjust output polling */
	if (!(conn->flags & CO_FL_SOCK_WR_SH) &&
	    (h2c->st0 == H2_CS_ERROR ||
	     h2c->mbuf->o ||
	     (h2c->mws > 0 && !LIST_ISEMPTY(&h2c->fctl_list)) ||
	     (!(h2c->flags & H2_CF_MUX_BLOCK_ANY) && !LIST_ISEMPTY(&h2c->send_list)))) {
		__conn_xprt_want_send(conn);
	}
	else {
		h2_release_buf(h2c, &h2c->mbuf);
		__conn_xprt_stop_send(conn);
	}

	if (h2c->task) {
		if (eb_is_empty(&h2c->streams_by_id) || h2c->mbuf->o) {
			h2c->task->expire = tick_add(now_ms, h2c->last_sid < 0 ? h2c->timeout : h2c->shut_timeout);
			task_queue(h2c->task);
		}
		else
			h2c->task->expire = TICK_ETERNITY;
	}
	return 0;
}