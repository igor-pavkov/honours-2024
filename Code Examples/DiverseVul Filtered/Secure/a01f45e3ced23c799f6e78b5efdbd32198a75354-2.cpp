static size_t h2s_htx_make_trailers(struct h2s *h2s, struct htx *htx)
{
	struct http_hdr list[MAX_HTTP_HDR];
	struct h2c *h2c = h2s->h2c;
	struct htx_blk *blk;
	struct htx_blk *blk_end;
	struct buffer outbuf;
	struct h1m h1m;
	enum htx_blk_type type;
	uint32_t size;
	int ret = 0;
	int hdr;
	int idx;
	void *start;

	if (h2c_mux_busy(h2c, h2s)) {
		h2s->flags |= H2_SF_BLK_MBUSY;
		goto end;
	}

	if (!h2_get_buf(h2c, &h2c->mbuf)) {
		h2c->flags |= H2_CF_MUX_MALLOC;
		h2s->flags |= H2_SF_BLK_MROOM;
		goto end;
	}

	/* The principle is that we parse each and every trailers block using
	 * the H1 headers parser, and append it to the list. We don't proceed
	 * until EOM is met. blk_end will point to the EOM block.
	 */
	hdr = 0;
	memset(list, 0, sizeof(list));
	blk_end = NULL;

	for (idx = htx_get_head(htx); idx != -1; idx = htx_get_next(htx, idx)) {
		blk = htx_get_blk(htx, idx);
		type = htx_get_blk_type(blk);

		if (type == HTX_BLK_UNUSED)
			continue;

		if (type != HTX_BLK_TLR) {
			if (type == HTX_BLK_EOM)
				blk_end = blk;
			break;
		}

		if (unlikely(hdr >= sizeof(list)/sizeof(list[0]) - 1))
			goto fail;

		size = htx_get_blksz(blk);
		start = htx_get_blk_ptr(htx, blk);

		h1m.flags = H1_MF_HDRS_ONLY | H1_MF_TOLOWER;
		h1m.err_pos = 0;
		ret = h1_headers_to_hdr_list(start, start + size,
					     list + hdr, sizeof(list)/sizeof(list[0]) - hdr,
					     &h1m, NULL);
		if (ret < 0)
			goto fail;

		/* ret == 0 if an incomplete trailers block was found (missing
		 * empty line), or > 0 if it was found. We have to continue on
		 * incomplete messages because the trailers block might be
		 * incomplete.
		 */

		/* search the new end */
		while (hdr <= sizeof(list)/sizeof(list[0])) {
			if (!list[hdr].n.len)
				break;
			hdr++;
		}
	}

	if (!blk_end)
		goto end; // end not found yet

	if (!hdr)
		goto done;

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

	/* len: 0x000000 (fill later), type: 1(HEADERS), flags: ENDH=4,ES=1 */
	memcpy(outbuf.area, "\x00\x00\x00\x01\x05", 5);
	write_n32(outbuf.area + 5, h2s->id); // 4 bytes
	outbuf.data = 9;

	/* encode status, which necessarily is the first one */
	if (!hpack_encode_int_status(&outbuf, h2s->status)) {
		if (b_space_wraps(&h2c->mbuf))
			goto realign_again;
		goto full;
	}

	/* encode all headers */
	for (idx = 0; idx < hdr; idx++) {
		/* these ones do not exist in H2 or must not appear in
		 * trailers and must be dropped.
		 */
		if (isteq(list[idx].n, ist("host")) ||
		    isteq(list[idx].n, ist("content-length")) ||
		    isteq(list[idx].n, ist("connection")) ||
		    isteq(list[idx].n, ist("proxy-connection")) ||
		    isteq(list[idx].n, ist("keep-alive")) ||
		    isteq(list[idx].n, ist("upgrade")) ||
		    isteq(list[idx].n, ist("te")) ||
		    isteq(list[idx].n, ist("transfer-encoding")))
			continue;

		if (!hpack_encode_header(&outbuf, list[idx].n, list[idx].v)) {
			/* output full */
			if (b_space_wraps(&h2c->mbuf))
				goto realign_again;
			goto full;
		}
	}

	/* update the frame's size */
	h2_set_frame_size(outbuf.area, outbuf.data - 9);

	/* commit the H2 response */
	b_add(&h2c->mbuf, outbuf.data);
	h2s->flags |= H2_SF_ES_SENT;

	if (h2s->st == H2_SS_OPEN)
		h2s->st = H2_SS_HLOC;
	else
		h2s_close(h2s);

	/* OK we could properly deliver the response */
 done:
	/* remove all header blocks including EOM and compute the corresponding size. */
	ret = 0;
	idx = htx_get_head(htx);
	blk = htx_get_blk(htx, idx);
	while (blk != blk_end) {
		ret += htx_get_blksz(blk);
		blk = htx_remove_blk(htx, blk);
	}
	blk = htx_remove_blk(htx, blk);
 end:
	return ret;
 full:
	h2c->flags |= H2_CF_MUX_MFULL;
	h2s->flags |= H2_SF_BLK_MROOM;
	ret = 0;
	goto end;
 fail:
	/* unparsable HTX messages, too large ones to be produced in the local
	 * list etc go here (unrecoverable errors).
	 */
	h2s_error(h2s, H2_ERR_INTERNAL_ERROR);
	ret = 0;
	goto end;
}