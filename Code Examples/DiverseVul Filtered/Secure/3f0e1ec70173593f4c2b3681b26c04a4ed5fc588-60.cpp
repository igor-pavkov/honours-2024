static int h2_buf_available(void *target)
{
	struct h2c *h2c = target;

	if ((h2c->flags & H2_CF_DEM_DALLOC) && b_alloc_margin(&h2c->dbuf, 0)) {
		h2c->flags &= ~H2_CF_DEM_DALLOC;
		if (h2_recv_allowed(h2c))
			conn_xprt_want_recv(h2c->conn);
		return 1;
	}

	if ((h2c->flags & H2_CF_MUX_MALLOC) && b_alloc_margin(&h2c->mbuf, 0)) {
		h2c->flags &= ~H2_CF_MUX_MALLOC;
		if (!(h2c->flags & H2_CF_MUX_BLOCK_ANY))
			conn_xprt_want_send(h2c->conn);

		if (h2c->flags & H2_CF_DEM_MROOM) {
			h2c->flags &= ~H2_CF_DEM_MROOM;
			if (h2_recv_allowed(h2c))
				conn_xprt_want_recv(h2c->conn);
		}
		return 1;
	}
	return 0;
}