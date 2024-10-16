static int h2_process(struct h2c *h2c)
{
	struct connection *conn = h2c->conn;

	if (b_data(&h2c->dbuf) && !(h2c->flags & H2_CF_DEM_BLOCK_ANY)) {
		h2_process_demux(h2c);

		if (h2c->st0 >= H2_CS_ERROR || conn->flags & CO_FL_ERROR)
			b_reset(&h2c->dbuf);

		if (!b_full(&h2c->dbuf))
			h2c->flags &= ~H2_CF_DEM_DFULL;
	}
	h2_send(h2c);

	if (unlikely(h2c->proxy->state == PR_STSTOPPED)) {
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
			if (h2s->cs && h2s->cs->flags & CS_FL_WAIT_FOR_HS)
				h2s_notify_recv(h2s);
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
	}

	if (!b_data(&h2c->dbuf))
		h2_release_buf(h2c, &h2c->dbuf);

	if ((conn->flags & CO_FL_SOCK_WR_SH) ||
	    h2c->st0 == H2_CS_ERROR2 || (h2c->flags & H2_CF_GOAWAY_FAILED) ||
	    (h2c->st0 != H2_CS_ERROR &&
	     !b_data(&h2c->mbuf) &&
	     (h2c->mws <= 0 || LIST_ISEMPTY(&h2c->fctl_list)) &&
	     ((h2c->flags & H2_CF_MUX_BLOCK_ANY) || LIST_ISEMPTY(&h2c->send_list))))
		h2_release_buf(h2c, &h2c->mbuf);

	if (h2c->task) {
		if (eb_is_empty(&h2c->streams_by_id) || b_data(&h2c->mbuf)) {
			h2c->task->expire = tick_add(now_ms, h2c->last_sid < 0 ? h2c->timeout : h2c->shut_timeout);
			task_queue(h2c->task);
		}
		else
			h2c->task->expire = TICK_ETERNITY;
	}

	h2_send(h2c);
	return 0;
}