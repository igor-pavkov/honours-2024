static int h2_frt_decode_headers(struct h2s *h2s, struct buffer *buf, int count)
{
	struct h2c *h2c = h2s->h2c;
	const uint8_t *hdrs = (uint8_t *)h2c->dbuf->p;
	struct chunk *tmp = get_trash_chunk();
	struct http_hdr list[MAX_HTTP_HDR * 2];
	struct chunk *copy = NULL;
	int flen = h2c->dfl;
	int outlen = 0;
	int wrap;
	int try;

	if (!h2c->dfl) {
		h2s_error(h2s, H2_ERR_PROTOCOL_ERROR); // empty headers frame!
		h2c->st0 = H2_CS_FRAME_E;
		return 0;
	}

	if (h2c->dbuf->i < h2c->dfl && h2c->dbuf->i < h2c->dbuf->size)
		return 0; // incomplete input frame

	/* if the input buffer wraps, take a temporary copy of it (rare) */
	wrap = h2c->dbuf->data + h2c->dbuf->size - h2c->dbuf->p;
	if (wrap < h2c->dfl) {
		copy = alloc_trash_chunk();
		if (!copy) {
			h2c_error(h2c, H2_ERR_INTERNAL_ERROR);
			goto fail;
		}
		memcpy(copy->str, h2c->dbuf->p, wrap);
		memcpy(copy->str + wrap, h2c->dbuf->data, h2c->dfl - wrap);
		hdrs = (uint8_t *)copy->str;
	}

	/* The padlen is the first byte before data, and the padding appears
	 * after data. padlen+data+padding are included in flen.
	 */
	if (h2c->dff & H2_F_HEADERS_PADDED) {
		h2c->dpl = *hdrs;
		if (h2c->dpl >= flen) {
			/* RFC7540#6.2 : pad length = length of frame payload or greater */
			h2c_error(h2c, H2_ERR_PROTOCOL_ERROR);
			return 0;
		}
		flen -= h2c->dpl + 1;
		hdrs += 1; // skip Pad Length
	}

	/* Skip StreamDep and weight for now (we don't support PRIORITY) */
	if (h2c->dff & H2_F_HEADERS_PRIORITY) {
		if (read_n32(hdrs) == h2s->id) {
			/* RFC7540#5.3.1 : stream dep may not depend on itself */
			h2c_error(h2c, H2_ERR_PROTOCOL_ERROR);
			return 0;//goto fail_stream;
		}

		hdrs += 5; // stream dep = 4, weight = 1
		flen -= 5;
	}

	/* FIXME: lack of END_HEADERS means there's a continuation frame, we
	 * don't support this for now and can't even decompress so we have to
	 * break the connection.
	 */
	if (!(h2c->dff & H2_F_HEADERS_END_HEADERS)) {
		h2c_error(h2c, H2_ERR_INTERNAL_ERROR);
		goto fail;
	}

	/* we can't retry a failed decompression operation so we must be very
	 * careful not to take any risks. In practice the output buffer is
	 * always empty except maybe for trailers, so these operations almost
	 * never happen.
	 */
	if (unlikely(buf->o)) {
		/* need to let the output buffer flush and
		 * mark the buffer for later wake up.
		 */
		goto fail;
	}

	if (unlikely(buffer_space_wraps(buf))) {
		/* it doesn't fit and the buffer is fragmented,
		 * so let's defragment it and try again.
		 */
		buffer_slow_realign(buf);
	}

	/* first check if we have some room after p+i */
	try = buf->data + buf->size - (buf->p + buf->i);

	/* otherwise continue between data and p-o */
	if (try <= 0) {
		try = buf->p - (buf->data + buf->o);
		if (try <= 0)
			goto fail;
	}
	if (try > count)
		try = count;

	outlen = hpack_decode_frame(h2c->ddht, hdrs, flen, list,
	                            sizeof(list)/sizeof(list[0]), tmp);
	if (outlen < 0) {
		h2c_error(h2c, H2_ERR_COMPRESSION_ERROR);
		goto fail;
	}

	/* OK now we have our header list in <list> */
	outlen = h2_make_h1_request(list, bi_end(buf), try);

	if (outlen < 0) {
		h2c_error(h2c, H2_ERR_COMPRESSION_ERROR);
		goto fail;
	}

	/* now consume the input data */
	bi_del(h2c->dbuf, h2c->dfl);
	h2c->st0 = H2_CS_FRAME_H;
	buf->i += outlen;

	/* don't send it before returning data!
	 * FIXME: should we instead try to send it much later, after the
	 * response ? This would require that we keep a copy of it in h2s.
	 */
	if (h2c->dff & H2_F_HEADERS_END_STREAM) {
		h2s->cs->flags |= CS_FL_EOS;
		h2s->flags |= H2_SF_ES_RCVD;
	}

 leave:
	free_trash_chunk(copy);
	return outlen;
 fail:
	outlen = 0;
	goto leave;
}