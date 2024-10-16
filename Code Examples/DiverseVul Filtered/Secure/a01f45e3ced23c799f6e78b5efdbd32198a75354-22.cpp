static int h2c_bck_send_preface(struct h2c *h2c)
{
	struct buffer *res;

	if (h2c_mux_busy(h2c, NULL)) {
		h2c->flags |= H2_CF_DEM_MBUSY;
		return 0;
	}

	res = h2_get_buf(h2c, &h2c->mbuf);
	if (!res) {
		h2c->flags |= H2_CF_MUX_MALLOC;
		h2c->flags |= H2_CF_DEM_MROOM;
		return 0;
	}

	if (!b_data(res)) {
		/* preface not yet sent */
		b_istput(res, ist(H2_CONN_PREFACE));
	}

	return h2c_send_settings(h2c);
}