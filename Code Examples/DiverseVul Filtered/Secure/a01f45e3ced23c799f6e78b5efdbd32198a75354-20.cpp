static int h2c_handle_priority(struct h2c *h2c)
{
	int error;

	if (h2c->dsi == 0) {
		error = H2_ERR_PROTOCOL_ERROR;
		goto conn_err;
	}

	if (h2c->dfl != 5) {
		error = H2_ERR_FRAME_SIZE_ERROR;
		goto conn_err;
	}

	/* process full frame only */
	if (b_data(&h2c->dbuf) < h2c->dfl)
		return 0;

	if (h2_get_n32(&h2c->dbuf, 0) == h2c->dsi) {
		/* 7540#5.3 : can't depend on itself */
		error = H2_ERR_PROTOCOL_ERROR;
		goto conn_err;
	}
	return 1;

 conn_err:
	h2c_error(h2c, error);
	return 0;
}