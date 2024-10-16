static int h2s_frt_make_resp_headers(struct h2s *h2s, struct buffer *buf)
{
	struct http_hdr list[MAX_HTTP_HDR];
	struct h2c *h2c = h2s->h2c;
	struct h1m *h1m = &h2s->res;
	struct chunk outbuf;
	int es_now = 0;
	int ret = 0;
	int hdr;

	if (h2c_mux_busy(h2c, h2s)) {
		h2s->flags |= H2_SF_BLK_MBUSY;
		return 0;
	}

	if (!h2_get_buf(h2c, &h2c->mbuf)) {
		h2c->flags |= H2_CF_MUX_MALLOC;
		h2s->flags |= H2_SF_BLK_MROOM;
		return 0;
	}

	/* First, try to parse the H1 response and index it into <list>.
	 * NOTE! Since it comes from haproxy, we *know* that a response header
	 * block does not wrap and we can safely read it this way without
	 * having to realign the buffer.
	 */
	ret = h1_headers_to_hdr_list(bo_ptr(buf), bo_ptr(buf) + buf->o,
	                             list, sizeof(list)/sizeof(list[0]), h1m);
	if (ret <= 0) {
		/* incomplete or invalid response, this is abnormal coming from
		 * haproxy and may only result in a bad errorfile or bad Lua code
		 * so that won't be fixed, raise an error now.
		 *
		 * FIXME: we should instead add the ability to only return a
		 * 502 bad gateway. But in theory this is not supposed to
		 * happen.
		 */
		h2s_error(h2s, H2_ERR_INTERNAL_ERROR);
		ret = 0;
		goto end;
	}

	chunk_reset(&outbuf);

	while (1) {
		outbuf.str  = bo_end(h2c->mbuf);
		outbuf.size = bo_contig_space(h2c->mbuf);
		outbuf.len = 0;

		if (outbuf.size >= 9 || !buffer_space_wraps(h2c->mbuf))
			break;
	realign_again:
		buffer_slow_realign(h2c->mbuf);
	}

	if (outbuf.size < 9) {
		h2c->flags |= H2_CF_MUX_MFULL;
		h2s->flags |= H2_SF_BLK_MROOM;
		ret = 0;
		goto end;
	}

	/* len: 0x000000 (fill later), type: 1(HEADERS), flags: ENDH=4 */
	memcpy(outbuf.str, "\x00\x00\x00\x01\x04", 5);
	write_n32(outbuf.str + 5, h2s->id); // 4 bytes
	outbuf.len = 9;

	/* encode status, which necessarily is the first one */
	if (outbuf.len < outbuf.size && h1m->status == 200)
		outbuf.str[outbuf.len++] = 0x88; // indexed field : idx[08]=(":status", "200")
	else if (outbuf.len < outbuf.size && h1m->status == 304)
		outbuf.str[outbuf.len++] = 0x8b; // indexed field : idx[11]=(":status", "304")
	else if (unlikely(list[0].v.len != 3)) {
		/* this is an unparsable response */
		h2s_error(h2s, H2_ERR_INTERNAL_ERROR);
		ret = 0;
		goto end;
	}
	else if (unlikely(outbuf.len + 2 + 3 <= outbuf.size)) {
		/* basic encoding of the status code */
		outbuf.str[outbuf.len++] = 0x48; // indexed name -- name=":status" (idx 8)
		outbuf.str[outbuf.len++] = 0x03; // 3 bytes status
		outbuf.str[outbuf.len++] = list[0].v.ptr[0];
		outbuf.str[outbuf.len++] = list[0].v.ptr[1];
		outbuf.str[outbuf.len++] = list[0].v.ptr[2];
	}
	else {
		if (buffer_space_wraps(h2c->mbuf))
			goto realign_again;

		h2c->flags |= H2_CF_MUX_MFULL;
		h2s->flags |= H2_SF_BLK_MROOM;
		ret = 0;
		goto end;
	}

	/* encode all headers, stop at empty name */
	for (hdr = 1; hdr < sizeof(list)/sizeof(list[0]); hdr++) {
		/* these ones do not exist in H2 and must be dropped. */
		if (isteq(list[hdr].n, ist("connection")) ||
		    isteq(list[hdr].n, ist("proxy-connection")) ||
		    isteq(list[hdr].n, ist("keep-alive")) ||
		    isteq(list[hdr].n, ist("upgrade")) ||
		    isteq(list[hdr].n, ist("transfer-encoding")))
			continue;

		if (isteq(list[hdr].n, ist("")))
			break; // end

		if (!hpack_encode_header(&outbuf, list[hdr].n, list[hdr].v)) {
			/* output full */
			if (buffer_space_wraps(h2c->mbuf))
				goto realign_again;

			h2c->flags |= H2_CF_MUX_MFULL;
			h2s->flags |= H2_SF_BLK_MROOM;
			ret = 0;
			goto end;
		}
	}

	/* we may need to add END_STREAM */
	if (((h1m->flags & H1_MF_CLEN) && !h1m->body_len) || h2s->cs->flags & CS_FL_SHW)
		es_now = 1;

	/* update the frame's size */
	h2_set_frame_size(outbuf.str, outbuf.len - 9);

	if (es_now)
		outbuf.str[4] |= H2_F_HEADERS_END_STREAM;

	/* consume incoming H1 response */
	bo_del(buf, ret);

	/* commit the H2 response */
	h2c->mbuf->o += outbuf.len;
	h2c->mbuf->p = b_ptr(h2c->mbuf, outbuf.len);
	h2s->flags |= H2_SF_HEADERS_SENT;

	/* for now we don't implemented CONTINUATION, so we wait for a
	 * body or directly end in TRL2.
	 */
	if (es_now) {
		// trim any possibly pending data (eg: inconsistent content-length)
		bo_del(buf, buf->o);

		h1m->state = HTTP_MSG_DONE;
		h2s->flags |= H2_SF_ES_SENT;
		if (h2s->st == H2_SS_OPEN)
			h2s->st = H2_SS_HLOC;
		else
			h2s_close(h2s);
	}
	else if (h1m->status >= 100 && h1m->status < 200) {
		/* we'll let the caller check if it has more headers to send */
		h1m->state = HTTP_MSG_RPBEFORE;
		h1m->status = 0;
		h1m->flags = 0;
		goto end;
	}
	else
		h1m->state = (h1m->flags & H1_MF_CHNK) ? HTTP_MSG_CHUNK_SIZE : HTTP_MSG_BODY;

 end:
	//fprintf(stderr, "[%d] sent simple H2 response (sid=%d) = %d bytes (%d in, ep=%u, es=%s)\n", h2c->st0, h2s->id, outbuf.len, ret, h1m->err_pos, h1_msg_state_str(h1m->err_state));
	return ret;
}