static int h2c_handle_goaway(struct h2c *h2c)
{
	int error;
	int last;

	if (h2c->dsi != 0) {
		error = H2_ERR_PROTOCOL_ERROR;
		goto conn_err;
	}

	if (h2c->dfl < 8) {
		error = H2_ERR_FRAME_SIZE_ERROR;
		goto conn_err;
	}

	/* process full frame only */
	if (b_data(&h2c->dbuf) < h2c->dfl)
		return 0;

	last = h2_get_n32(&h2c->dbuf, 0);
	h2c->errcode = h2_get_n32(&h2c->dbuf, 4);
	h2_wake_some_streams(h2c, last, CS_FL_ERR_PENDING);
	if (h2c->last_sid < 0)
		h2c->last_sid = last;
	return 1;

 conn_err:
	h2c_error(h2c, error);
	return 0;
}