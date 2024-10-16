static int h2_send(struct h2c *h2c)
{
	struct connection *conn = h2c->conn;
	int done;
	int sent = 0;

	if (conn->flags & CO_FL_ERROR)
		return 1;


	if (conn->flags & (CO_FL_HANDSHAKE|CO_FL_WAIT_L4_CONN|CO_FL_WAIT_L6_CONN)) {
		/* a handshake was requested */
		goto schedule;
	}

	/* This loop is quite simple : it tries to fill as much as it can from
	 * pending streams into the existing buffer until it's reportedly full
	 * or the end of send requests is reached. Then it tries to send this
	 * buffer's contents out, marks it not full if at least one byte could
	 * be sent, and tries again.
	 *
	 * The snd_buf() function normally takes a "flags" argument which may
	 * be made of a combination of CO_SFL_MSG_MORE to indicate that more
	 * data immediately comes and CO_SFL_STREAMER to indicate that the
	 * connection is streaming lots of data (used to increase TLS record
	 * size at the expense of latency). The former can be sent any time
	 * there's a buffer full flag, as it indicates at least one stream
	 * attempted to send and failed so there are pending data. An
	 * alternative would be to set it as long as there's an active stream
	 * but that would be problematic for ACKs until we have an absolute
	 * guarantee that all waiters have at least one byte to send. The
	 * latter should possibly not be set for now.
	 */

	done = 0;
	while (!done) {
		unsigned int flags = 0;

		/* fill as much as we can into the current buffer */
		while (((h2c->flags & (H2_CF_MUX_MFULL|H2_CF_MUX_MALLOC)) == 0) && !done)
			done = h2_process_mux(h2c);

		if (conn->flags & CO_FL_ERROR)
			break;

		if (h2c->flags & (H2_CF_MUX_MFULL | H2_CF_DEM_MBUSY | H2_CF_DEM_MROOM))
			flags |= CO_SFL_MSG_MORE;

		if (b_data(&h2c->mbuf)) {
			int ret = conn->xprt->snd_buf(conn, &h2c->mbuf, b_data(&h2c->mbuf), flags);
			if (!ret)
				break;
			sent = 1;
			b_del(&h2c->mbuf, ret);
			b_realign_if_empty(&h2c->mbuf);
		}

		/* wrote at least one byte, the buffer is not full anymore */
		h2c->flags &= ~(H2_CF_MUX_MFULL | H2_CF_DEM_MROOM);
	}

	if (conn->flags & CO_FL_SOCK_WR_SH) {
		/* output closed, nothing to send, clear the buffer to release it */
		b_reset(&h2c->mbuf);
	}
	/* We're not full anymore, so we can wake any task that are waiting
	 * for us.
	 */
	if (!(h2c->flags & (H2_CF_MUX_MFULL | H2_CF_DEM_MROOM))) {
		while (!LIST_ISEMPTY(&h2c->send_list)) {
			struct h2s *h2s = LIST_ELEM(h2c->send_list.n,
			    struct h2s *, list);
			LIST_DEL(&h2s->list);
			LIST_INIT(&h2s->list);
			LIST_ADDQ(&h2c->sending_list, &h2s->list);
			h2s->send_wait->events &= ~SUB_RETRY_SEND;
			h2s->send_wait->events |= SUB_CALL_UNSUBSCRIBE;
			tasklet_wakeup(h2s->send_wait->task);
		}
	}
	/* We're done, no more to send */
	if (!b_data(&h2c->mbuf))
		return sent;
schedule:
	if (!(h2c->wait_event.events & SUB_RETRY_SEND))
		conn->xprt->subscribe(conn, SUB_RETRY_SEND, &h2c->wait_event);
	return sent;
}