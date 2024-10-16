static int h2c_frt_recv_preface(struct h2c *h2c)
{
	int ret1;
	int ret2;

	ret1 = b_isteq(&h2c->dbuf, 0, b_data(&h2c->dbuf), ist(H2_CONN_PREFACE));

	if (unlikely(ret1 <= 0)) {
		if (ret1 < 0)
			sess_log(h2c->conn->owner);

		if (ret1 < 0 || conn_xprt_read0_pending(h2c->conn))
			h2c_error(h2c, H2_ERR_PROTOCOL_ERROR);
		return 0;
	}

	ret2 = h2c_send_settings(h2c);
	if (ret2 > 0)
		b_del(&h2c->dbuf, ret1);

	return ret2;
}