static size_t h2s_frt_make_resp_headers(struct h2s *h2s, const struct buffer *buf, size_t ofs, size_t max)
{
	struct http_hdr list[MAX_HTTP_HDR];
	struct h2c *h2c = h2s->h2c;
	struct h1m *h1m = &h2s->h1m;
	struct buffer outbuf;
	union h1_sl sl;
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
	ret = h1_headers_to_hdr_list(b_peek(buf, ofs), b_peek(buf, ofs) + max,
	                             list, sizeof(list)/sizeof(list[0]), h1m, &sl);
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

	h2s->status = sl.st.status;

	/* certain statuses have no body or an empty one, regardless of
	 * what the headers say.
	 */
	if (sl.st.status >= 100 && sl.st.status < 200) {
		h1m->flags &= ~(H1_MF_CLEN | H1_MF_CHNK);
		h1m->curr_len = h1m->body_len = 0;
	}
	else if (sl.st.status == 204 || sl.st.status == 304) {
		/* no contents, claim c-len is present and set to zero */
		h1m->flags &= ~H1_MF_CHNK;
		h1m->flags |=  H1_MF_CLEN;
		h1m->curr_len = h1m->body_len = 0;
	}

	chunk_reset(&outbuf);

	while (1) {
		outbuf.area  = b_tail(&h2c->mbuf);
		outbuf.size = b_contig_space(&h2c->mbuf);
		outbuf.data = 0;

		if (outbuf.size >= 9 || !b_space_wraps(&h2c->mbuf))
			break;
	realign_again:
		b_slow_realign(&h2c->mbuf, trash.area, b_data(&h2c->mbuf));
	}

	if (outbuf.size < 9)
		goto full;

	/* len: 0x000000 (fill later), type: 1(HEADERS), flags: ENDH=4 */
	memcpy(outbuf.area, "\x00\x00\x00\x01\x04", 5);
	write_n32(outbuf.area + 5, h2s->id); // 4 bytes
	outbuf.data = 9;

	/* encode status, which necessarily is the first one */
	if (unlikely(list[0].v.len != 3)) {
		/* this is an unparsable response */
		h2s_error(h2s, H2_ERR_INTERNAL_ERROR);
		ret = 0;
		goto end;
	}

	if (!hpack_encode_str_status(&outbuf, h2s->status, list[0].v)) {
		if (b_space_wraps(&h2c->mbuf))
			goto realign_again;
		goto full;
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
			if (b_space_wraps(&h2c->mbuf))
				goto realign_again;
			goto full;
		}
	}

	/* we may need to add END_STREAM */
	if (((h1m->flags & H1_MF_CLEN) && !h1m->body_len) || h2s->cs->flags & CS_FL_SHW)
		es_now = 1;

	/* update the frame's size */
	h2_set_frame_size(outbuf.area, outbuf.data - 9);

	if (es_now)
		outbuf.area[4] |= H2_F_HEADERS_END_STREAM;

	/* consume incoming H1 response */
	max -= ret;

	/* commit the H2 response */
	b_add(&h2c->mbuf, outbuf.data);
	h2s->flags |= H2_SF_HEADERS_SENT;

	if (es_now) {
		// trim any possibly pending data (eg: inconsistent content-length)
		ret += max;

		h1m->state = H1_MSG_DONE;
		h2s->flags |= H2_SF_ES_SENT;
		if (h2s->st == H2_SS_OPEN)
			h2s->st = H2_SS_HLOC;
		else
			h2s_close(h2s);
	}
	else if (h2s->status >= 100 && h2s->status < 200) {
		/* we'll let the caller check if it has more headers to send */
		h1m_init_res(h1m);
		h1m->err_pos = -1; // don't care about errors on the response path
		h2s->h1m.flags |= H1_MF_TOLOWER;
		goto end;
	}

	/* now the h1m state is either H1_MSG_CHUNK_SIZE or H1_MSG_DATA */

 end:
	//fprintf(stderr, "[%d] sent simple H2 response (sid=%d) = %d bytes (%d in, ep=%u, es=%s)\n", h2c->st0, h2s->id, outbuf.len, ret, h1m->err_pos, h1m_state_str(h1m->err_state));
	return ret;
 full:
	h1m_init_res(h1m);
	h1m->err_pos = -1; // don't care about errors on the response path
	h2c->flags |= H2_CF_MUX_MFULL;
	h2s->flags |= H2_SF_BLK_MROOM;
	ret = 0;
	goto end;
}