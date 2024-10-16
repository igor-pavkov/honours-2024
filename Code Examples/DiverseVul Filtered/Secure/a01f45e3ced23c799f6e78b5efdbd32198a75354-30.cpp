static int h2c_frt_handle_data(struct h2c *h2c, struct h2s *h2s)
{
	int error;

	/* note that empty DATA frames are perfectly valid and sometimes used
	 * to signal an end of stream (with the ES flag).
	 */

	if (!b_size(&h2c->dbuf) && h2c->dfl)
		return 0; // empty buffer

	if (b_data(&h2c->dbuf) < h2c->dfl && !b_full(&h2c->dbuf))
		return 0; // incomplete frame

	/* now either the frame is complete or the buffer is complete */

	if (!h2c->dsi) {
		/* RFC7540#6.1 */
		error = H2_ERR_PROTOCOL_ERROR;
		goto conn_err;
	}

	if (h2s->st != H2_SS_OPEN && h2s->st != H2_SS_HLOC) {
		/* RFC7540#6.1 */
		error = H2_ERR_STREAM_CLOSED;
		goto strm_err;
	}

	if (!h2_frt_transfer_data(h2s))
		return 0;

	/* call the upper layers to process the frame, then let the upper layer
	 * notify the stream about any change.
	 */
	if (!h2s->cs) {
		error = H2_ERR_STREAM_CLOSED;
		goto strm_err;
	}

	if (h2c->st0 >= H2_CS_ERROR)
		return 0;

	if (h2s->st >= H2_SS_ERROR) {
		/* stream error : send RST_STREAM */
		h2c->st0 = H2_CS_FRAME_E;
	}

	/* check for completion : the callee will change this to FRAME_A or
	 * FRAME_H once done.
	 */
	if (h2c->st0 == H2_CS_FRAME_P)
		return 0;


	/* last frame */
	if (h2c->dff & H2_F_DATA_END_STREAM) {
		h2s->st = H2_SS_HREM;
		h2s->flags |= H2_SF_ES_RCVD;
		h2s->cs->flags |= CS_FL_REOS;
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