static int h2_recv(struct h2c *h2c)
{
	struct connection *conn = h2c->conn;
	struct buffer *buf;
	int max;
	size_t ret;

	if (h2c->wait_event.events & SUB_RETRY_RECV)
		return (b_data(&h2c->dbuf));

	if (!h2_recv_allowed(h2c))
		return 1;

	buf = h2_get_buf(h2c, &h2c->dbuf);
	if (!buf) {
		h2c->flags |= H2_CF_DEM_DALLOC;
		return 0;
	}

	do {
		b_realign_if_empty(buf);
		if (!b_data(buf) && (h2c->proxy->options2 & PR_O2_USE_HTX)) {
			/* HTX in use : try to pre-align the buffer like the
			 * rxbufs will be to optimize memory copies. We'll make
			 * sure that the frame header lands at the end of the
			 * HTX block to alias it upon recv. We cannot use the
			 * head because rcv_buf() will realign the buffer if
			 * it's empty. Thus we cheat and pretend we already
			 * have a few bytes there.
			 */
			max = buf_room_for_htx_data(buf) + 9;
			buf->head = sizeof(struct htx) - 9;
		}
		else
			max = b_room(buf);

		if (max)
			ret = conn->xprt->rcv_buf(conn, buf, max, 0);
		else
			ret = 0;
	} while (ret > 0);

	if (h2_recv_allowed(h2c) && (b_data(buf) < buf->size))
		conn->xprt->subscribe(conn, SUB_RETRY_RECV, &h2c->wait_event);

	if (!b_data(buf)) {
		h2_release_buf(h2c, &h2c->dbuf);
		return (conn->flags & CO_FL_ERROR || conn_xprt_read0_pending(conn));
	}

	if (b_data(buf) == buf->size)
		h2c->flags |= H2_CF_DEM_DFULL;
	return 1;
}