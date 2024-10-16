static size_t h2s_htx_bck_make_req_headers(struct h2s *h2s, struct htx *htx)
{
	struct http_hdr list[MAX_HTTP_HDR];
	struct h2c *h2c = h2s->h2c;
	struct htx_blk *blk;
	struct htx_blk *blk_end;
	struct buffer outbuf;
	struct htx_sl *sl;
	struct ist meth, path;
	enum htx_blk_type type;
	int es_now = 0;
	int ret = 0;
	int hdr;
	int idx;

	if (h2c_mux_busy(h2c, h2s)) {
		h2s->flags |= H2_SF_BLK_MBUSY;
		return 0;
	}

	if (!h2_get_buf(h2c, &h2c->mbuf)) {
		h2c->flags |= H2_CF_MUX_MALLOC;
		h2s->flags |= H2_SF_BLK_MROOM;
		return 0;
	}

	/* determine the first block which must not be deleted, blk_end may
	 * be NULL if all blocks have to be deleted.
	 */
	idx = htx_get_head(htx);
	blk_end = NULL;
	while (idx != -1) {
		type = htx_get_blk_type(htx_get_blk(htx, idx));
		idx = htx_get_next(htx, idx);
		if (type == HTX_BLK_EOH) {
			if (idx != -1)
				blk_end = htx_get_blk(htx, idx);
			break;
		}
	}

	/* get the start line, we do have one */
	sl = htx_get_stline(htx);
	ALREADY_CHECKED(sl);
	meth = htx_sl_req_meth(sl);
	path = htx_sl_req_uri(sl);

	/* and the rest of the headers, that we dump starting at header 0 */
	hdr = 0;

	idx = htx_get_head(htx); // returns the SL that we skip
	while ((idx = htx_get_next(htx, idx)) != -1) {
		blk = htx_get_blk(htx, idx);
		type = htx_get_blk_type(blk);

		if (type == HTX_BLK_UNUSED)
			continue;

		if (type != HTX_BLK_HDR)
			break;

		if (unlikely(hdr >= sizeof(list)/sizeof(list[0]) - 1))
			goto fail;

		list[hdr].n = htx_get_blk_name(htx, blk);
		list[hdr].v = htx_get_blk_value(htx, blk);
		hdr++;
	}

	/* marker for end of headers */
	list[hdr].n = ist("");

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

	/* encode the method, which necessarily is the first one */
	if (!hpack_encode_method(&outbuf, sl->info.req.meth, meth)) {
		if (b_space_wraps(&h2c->mbuf))
			goto realign_again;
		goto full;
	}

	/* encode the scheme which is always "https" (or 0x86 for "http") */
	if (!hpack_encode_scheme(&outbuf, ist("https"))) {
		/* output full */
		if (b_space_wraps(&h2c->mbuf))
			goto realign_again;
		goto full;
	}

	/* encode the path, which necessarily is the second one */
	if (!hpack_encode_path(&outbuf, path)) {
		/* output full */
		if (b_space_wraps(&h2c->mbuf))
			goto realign_again;
		goto full;
	}

	/* encode all headers, stop at empty name */
	for (hdr = 0; hdr < sizeof(list)/sizeof(list[0]); hdr++) {
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

	/* we may need to add END_STREAM if we have no body :
	 *  - request already closed, or :
	 *  - no transfer-encoding, and :
	 *  - no content-length or content-length:0
	 * Fixme: this doesn't take into account CONNECT requests.
	 */
	if (blk_end && htx_get_blk_type(blk_end) == HTX_BLK_EOM)
		es_now = 1;

	if (sl->flags & HTX_SL_F_BODYLESS)
		es_now = 1;

	if (h2s->cs->flags & CS_FL_SHW)
		es_now = 1;

	/* update the frame's size */
	h2_set_frame_size(outbuf.area, outbuf.data - 9);

	if (es_now)
		outbuf.area[4] |= H2_F_HEADERS_END_STREAM;

	/* commit the H2 response */
	b_add(&h2c->mbuf, outbuf.data);
	h2s->flags |= H2_SF_HEADERS_SENT;
	h2s->st = H2_SS_OPEN;

	if (es_now) {
		// trim any possibly pending data (eg: inconsistent content-length)
		h2s->flags |= H2_SF_ES_SENT;
		h2s->st = H2_SS_HLOC;
	}

	/* remove all header blocks including the EOH and compute the
	 * corresponding size.
	 *
	 * FIXME: We should remove everything when es_now is set.
	 */
	ret = 0;
	idx = htx_get_head(htx);
	blk = htx_get_blk(htx, idx);
	while (blk != blk_end) {
		ret += htx_get_blksz(blk);
		blk = htx_remove_blk(htx, blk);
	}

	if (blk_end && htx_get_blk_type(blk_end) == HTX_BLK_EOM)
		htx_remove_blk(htx, blk_end);

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