static int h2_buf_available(void *target)
{
	struct h2c *h2c = target;
	struct h2s *h2s;

	if ((h2c->flags & H2_CF_DEM_DALLOC) && b_alloc_margin(&h2c->dbuf, 0)) {
		h2c->flags &= ~H2_CF_DEM_DALLOC;
		h2c_restart_reading(h2c);
		return 1;
	}

	if ((h2c->flags & H2_CF_MUX_MALLOC) && b_alloc_margin(&h2c->mbuf, 0)) {
		h2c->flags &= ~H2_CF_MUX_MALLOC;

		if (h2c->flags & H2_CF_DEM_MROOM) {
			h2c->flags &= ~H2_CF_DEM_MROOM;
			h2c_restart_reading(h2c);
		}
		return 1;
	}

	if ((h2c->flags & H2_CF_DEM_SALLOC) &&
	    (h2s = h2c_st_by_id(h2c, h2c->dsi)) && h2s->cs &&
	    b_alloc_margin(&h2s->rxbuf, 0)) {
		h2c->flags &= ~H2_CF_DEM_SALLOC;
		h2c_restart_reading(h2c);
		return 1;
	}

	return 0;
}