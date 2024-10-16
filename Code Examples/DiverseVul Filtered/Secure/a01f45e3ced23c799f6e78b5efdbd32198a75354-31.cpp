static inline int h2_recv_allowed(const struct h2c *h2c)
{
	if (b_data(&h2c->dbuf) == 0 &&
	    (h2c->st0 >= H2_CS_ERROR ||
	     h2c->conn->flags & CO_FL_ERROR ||
	     conn_xprt_read0_pending(h2c->conn)))
		return 0;

	if (!(h2c->flags & H2_CF_DEM_DALLOC) &&
	    !(h2c->flags & H2_CF_DEM_BLOCK_ANY))
		return 1;

	return 0;
}