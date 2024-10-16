static int h2c_frt_handle_headers(struct h2c *h2c, struct h2s *h2s)
{
	int error;

	if (!h2c->dfl) {
		error = H2_ERR_PROTOCOL_ERROR; // empty headers frame!
		goto strm_err;
	}

	if (!h2c->dbuf->size)
		return 0; // empty buffer

	if (h2c->dbuf->i < h2c->dfl && h2c->dbuf->i < h2c->dbuf->size)
		return 0; // incomplete frame

	/* now either the frame is complete or the buffer is complete */
	if (h2s->st != H2_SS_IDLE) {
		/* FIXME: stream already exists, this is only allowed for
		 * trailers (not supported for now).
		 */
		error = H2_ERR_PROTOCOL_ERROR;
		goto conn_err;
	}
	else if (h2c->dsi <= h2c->max_id || !(h2c->dsi & 1)) {
		/* RFC7540#5.1.1 stream id > prev ones, and must be odd here */
		error = H2_ERR_PROTOCOL_ERROR;
		goto conn_err;
	}

	h2s = h2c_stream_new(h2c, h2c->dsi);
	if (!h2s) {
		error = H2_ERR_INTERNAL_ERROR;
		goto conn_err;
	}

	h2s->st = H2_SS_OPEN;
	if (h2c->dff & H2_F_HEADERS_END_STREAM) {
		h2s->st = H2_SS_HREM;
		h2s->flags |= H2_SF_ES_RCVD;
	}

	/* call the upper layers to process the frame, then let the upper layer
	 * notify the stream about any change.
	 */
	h2s->cs->data_cb->recv(h2s->cs);

	if (h2s->cs->data_cb->wake(h2s->cs) < 0) {
		/* FIXME: cs has already been destroyed, but we have to kill h2s. */
		error = H2_ERR_INTERNAL_ERROR;
		goto conn_err;
	}

	if (h2c->st0 >= H2_CS_ERROR)
		return 0;

	if (h2s->st >= H2_SS_ERROR) {
		/* stream error : send RST_STREAM */
		h2c->st0 = H2_CS_FRAME_E;
	}
	else {
		/* update the max stream ID if the request is being processed */
		if (h2s->id > h2c->max_id)
			h2c->max_id = h2s->id;
	}

	return 1;

 conn_err:
	h2c_error(h2c, error);
	return 0;

 strm_err:
	if (h2s) {
		h2s_error(h2s, error);
		h2c->st0 = H2_CS_FRAME_E;
	}
	else
		h2c_error(h2c, error);
	return 0;
}