static int h2c_handle_rst_stream(struct h2c *h2c, struct h2s *h2s)
{
	int error;

	if (h2c->dsi == 0) {
		error = H2_ERR_PROTOCOL_ERROR;
		goto conn_err;
	}

	if (h2c->dfl != 4) {
		error = H2_ERR_FRAME_SIZE_ERROR;
		goto conn_err;
	}

	/* process full frame only */
	if (h2c->dbuf->i < h2c->dfl)
		return 0;

	/* late RST, already handled */
	if (h2s->st == H2_SS_CLOSED)
		return 1;

	h2s->errcode = h2_get_n32(h2c->dbuf, 0);
	h2s_close(h2s);

	if (h2s->cs) {
		h2s->cs->flags |= CS_FL_EOS | CS_FL_ERROR;
		/* recv is used to force to detect CS_FL_EOS that wake()
		 * doesn't handle in the stream-int code.
		 */
		h2s->cs->data_cb->recv(h2s->cs);
		h2s->cs->data_cb->wake(h2s->cs);
	}

	h2s->flags |= H2_SF_RST_RCVD;
	return 1;

 conn_err:
	h2c_error(h2c, error);
	return 0;
}