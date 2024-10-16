static int h2c_frt_recv_preface(struct h2c *h2c)
{
	int ret1;
	int ret2;

	ret1 = b_isteq(h2c->dbuf, 0, h2c->dbuf->i, ist(H2_CONN_PREFACE));

	if (unlikely(ret1 <= 0)) {
		if (ret1 < 0 || conn_xprt_read0_pending(h2c->conn))
			h2c_error(h2c, H2_ERR_PROTOCOL_ERROR);
		return 0;
	}

	ret2 = h2c_snd_settings(h2c);
	if (ret2 > 0)
		bi_del(h2c->dbuf, ret1);

	return ret2;
}