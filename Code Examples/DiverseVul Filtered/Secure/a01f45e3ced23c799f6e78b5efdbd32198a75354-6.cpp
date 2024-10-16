static struct h2s *h2c_frt_handle_headers(struct h2c *h2c, struct h2s *h2s)
{
	struct buffer rxbuf = BUF_NULL;
	uint32_t flags = 0;
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

	/* now either the frame is complete or the buffer is complete */
	if (h2s->st != H2_SS_IDLE) {
		/* The stream exists/existed, this must be a trailers frame */
		if (h2s->st != H2_SS_CLOSED) {
			if (!h2c_decode_headers(h2c, &h2s->rxbuf, &h2s->flags))
				goto out;
			goto done;
		}
		error = H2_ERR_PROTOCOL_ERROR;
		sess_log(h2c->conn->owner);
		goto conn_err;
	}
	else if (h2c->dsi <= h2c->max_id || !(h2c->dsi & 1)) {
		/* RFC7540#5.1.1 stream id > prev ones, and must be odd here */
		error = H2_ERR_PROTOCOL_ERROR;
		sess_log(h2c->conn->owner);
		goto conn_err;
	}
	else if (h2c->flags & H2_CF_DEM_TOOMANY)
		goto out; // IDLE but too many cs still present

	error = h2c_decode_headers(h2c, &rxbuf, &flags);

	/* unrecoverable error ? */
	if (h2c->st0 >= H2_CS_ERROR)
		goto out;

	if (error <= 0) {
		if (error == 0)
			goto out; // missing data

		/* Failed to decode this stream (e.g. too large request)
		 * but the HPACK decompressor is still synchronized.
		 */
		h2s = (struct h2s*)h2_error_stream;
		goto send_rst;
	}

	/* Note: we don't emit any other logs below because ff we return
	 * positively from h2c_frt_stream_new(), the stream will report the error,
	 * and if we return in error, h2c_frt_stream_new() will emit the error.
	 */
	h2s = h2c_frt_stream_new(h2c, h2c->dsi);
	if (!h2s) {
		h2s = (struct h2s*)h2_refused_stream;
		goto send_rst;
	}

	h2s->st = H2_SS_OPEN;
	h2s->rxbuf = rxbuf;
	h2s->flags |= flags;

 done:
	if (h2c->dff & H2_F_HEADERS_END_STREAM)
		h2s->flags |= H2_SF_ES_RCVD;

	if (h2s->flags & H2_SF_ES_RCVD) {
		h2s->st = H2_SS_HREM;
		h2s->cs->flags |= CS_FL_REOS;
	}

	/* update the max stream ID if the request is being processed */
	if (h2s->id > h2c->max_id)
		h2c->max_id = h2s->id;

	return h2s;

 conn_err:
	h2c_error(h2c, error);
	goto out;

 out:
	h2_release_buf(h2c, &rxbuf);
	return NULL;

 send_rst:
	/* make the demux send an RST for the current stream. We may only
	 * do this if we're certain that the HEADERS frame was properly
	 * decompressed so that the HPACK decoder is still kept up to date.
	 */
	h2_release_buf(h2c, &rxbuf);
	h2c->st0 = H2_CS_FRAME_E;
	return h2s;
}