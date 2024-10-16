static struct h2s *h2c_bck_handle_headers(struct h2c *h2c, struct h2s *h2s)
{
	int error;

	if (!h2c->dfl) {
		/* RFC7540#4.2 */
		error = H2_ERR_FRAME_SIZE_ERROR; // empty headers frame!
		sess_log(h2c->conn->owner);
		goto conn_err;
	}

	if (!b_size(&h2c->dbuf))
		return NULL; // empty buffer

	if (b_data(&h2c->dbuf) < h2c->dfl && !b_full(&h2c->dbuf))
		return NULL; // incomplete frame

	error = h2c_decode_headers(h2c, &h2s->rxbuf, &h2s->flags);

	/* unrecoverable error ? */
	if (h2c->st0 >= H2_CS_ERROR)
		return NULL;

	if (error <= 0) {
		if (error == 0)
			return NULL; // missing data

		/* stream error : send RST_STREAM */
		h2s_error(h2s, H2_ERR_PROTOCOL_ERROR);
		h2c->st0 = H2_CS_FRAME_E;
		return NULL;
	}

	if (h2c->dff & H2_F_HEADERS_END_STREAM) {
		h2s->flags |= H2_SF_ES_RCVD;
		h2s->cs->flags |= CS_FL_REOS;
	}

	if (h2s->cs->flags & CS_FL_ERROR && h2s->st < H2_SS_ERROR)
		h2s->st = H2_SS_ERROR;
	else if (h2s->cs->flags & CS_FL_REOS && h2s->st == H2_SS_OPEN)
		h2s->st = H2_SS_HREM;
	else if (h2s->cs->flags & CS_FL_REOS && h2s->st == H2_SS_HLOC)
		h2s_close(h2s);

	return h2s;

 conn_err:
	h2c_error(h2c, error);
	return NULL;
}