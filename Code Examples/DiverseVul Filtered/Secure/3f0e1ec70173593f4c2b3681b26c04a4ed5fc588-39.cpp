static void h2_recv(struct connection *conn)
{
	struct h2c *h2c = conn->mux_ctx;
	struct buffer *buf;
	int max;

	if (!h2_recv_allowed(h2c))
		return;

	buf = h2_get_buf(h2c, &h2c->dbuf);
	if (!buf) {
		h2c->flags |= H2_CF_DEM_DALLOC;
		return;
	}

	/* note: buf->o == 0 */
	max = buf->size - buf->i;
	if (max)
		conn->xprt->rcv_buf(conn, buf, max);

	if (!buf->i) {
		h2_release_buf(h2c, &h2c->dbuf);
		return;
	}

	if (buf->i == buf->size)
		h2c->flags |= H2_CF_DEM_DFULL;
	return;
}