static size_t h2s_htx_frt_make_resp_data(struct h2s *h2s, struct buffer *buf, size_t count)
{
	struct h2c *h2c = h2s->h2c;
	struct htx *htx;
	struct buffer outbuf;
	size_t total = 0;
	int es_now = 0;
	int bsize; /* htx block size */
	int fsize; /* h2 frame size  */
	struct htx_blk *blk;
	enum htx_blk_type type;
	int idx;

	if (h2c_mux_busy(h2c, h2s)) {
		h2s->flags |= H2_SF_BLK_MBUSY;
		goto end;
	}

	if (!h2_get_buf(h2c, &h2c->mbuf)) {
		h2c->flags |= H2_CF_MUX_MALLOC;
		h2s->flags |= H2_SF_BLK_MROOM;
		goto end;
	}

	htx = htx_from_buf(buf);

	/* We only come here with HTX_BLK_DATA or HTX_BLK_EOD blocks. However,
	 * while looping, we can meet an HTX_BLK_EOM block that we'll leave to
	 * the caller to handle.
	 */

 new_frame:
	if (!count || htx_is_empty(htx))
		goto end;

	idx   = htx_get_head(htx);
	blk   = htx_get_blk(htx, idx);
	type  = htx_get_blk_type(blk); // DATA or EOD or EOM
	bsize = htx_get_blksz(blk);
	fsize = bsize;

	if (type == HTX_BLK_EOD) {
		/* if we have an EOD, we're dealing with chunked data. We may
		 * have a set of trailers after us that the caller will want to
		 * deal with. Let's simply remove the EOD and return.
		 */
		htx_remove_blk(htx, blk);
		total++; // EOD counts as one byte
		count--;
		goto end;
	}
	else if (type == HTX_BLK_EOM) {
		if (h2s->flags & H2_SF_ES_SENT) {
			/* ES already sent */
			htx_remove_blk(htx, blk);
			total++; // EOM counts as one byte
			count--;
			goto end;
		}
	}
	else if (type != HTX_BLK_DATA)
		goto end;

	/* Perform some optimizations to reduce the number of buffer copies.
	 * First, if the mux's buffer is empty and the htx area contains
	 * exactly one data block of the same size as the requested count, and
	 * this count fits within the frame size, the stream's window size, and
	 * the connection's window size, then it's possible to simply swap the
	 * caller's buffer with the mux's output buffer and adjust offsets and
	 * length to match the entire DATA HTX block in the middle. In this
	 * case we perform a true zero-copy operation from end-to-end. This is
	 * the situation that happens all the time with large files. Second, if
	 * this is not possible, but the mux's output buffer is empty, we still
	 * have an opportunity to avoid the copy to the intermediary buffer, by
	 * making the intermediary buffer's area point to the output buffer's
	 * area. In this case we want to skip the HTX header to make sure that
	 * copies remain aligned and that this operation remains possible all
	 * the time. This goes for headers, data blocks and any data extracted
	 * from the HTX blocks.
	 */
	if (unlikely(fsize == count &&
	             htx->used == 1 && type == HTX_BLK_DATA &&
	             fsize <= h2s->mws && fsize <= h2c->mws && fsize <= h2c->mfs)) {
		void *old_area = h2c->mbuf.area;

		if (b_data(&h2c->mbuf)) {
			/* too bad there are data left there. If we have less
			 * than 1/4 of the mbuf's size and everything fits,
			 * we'll perform a copy anyway. Otherwise we'll pretend
			 * the mbuf is full and wait.
			 */
			if (fsize <= b_size(&h2c->mbuf) / 4 && fsize + 9 <= b_room(&h2c->mbuf))
				goto copy;
			h2c->flags |= H2_CF_MUX_MFULL;
			h2s->flags |= H2_SF_BLK_MROOM;
			goto end;
		}

		/* map an H2 frame to the HTX block so that we can put the
		 * frame header there.
		 */
		h2c->mbuf.area = buf->area;
		h2c->mbuf.head = sizeof(struct htx) + blk->addr - 9;
		h2c->mbuf.data = fsize + 9;
		outbuf.area    = b_head(&h2c->mbuf);

		/* prepend an H2 DATA frame header just before the DATA block */
		memcpy(outbuf.area, "\x00\x00\x00\x00\x00", 5);
		write_n32(outbuf.area + 5, h2s->id); // 4 bytes
		h2_set_frame_size(outbuf.area, fsize);

		/* update windows */
		h2s->mws -= fsize;
		h2c->mws -= fsize;

		/* and exchange with our old area */
		buf->area = old_area;
		buf->data = buf->head = 0;
		total += fsize;
		goto end;
	}

 copy:
	/* for DATA and EOM we'll have to emit a frame, even if empty */

	while (1) {
		outbuf.area  = b_tail(&h2c->mbuf);
		outbuf.size = b_contig_space(&h2c->mbuf);
		outbuf.data = 0;

		if (outbuf.size >= 9 || !b_space_wraps(&h2c->mbuf))
			break;
	realign_again:
		b_slow_realign(&h2c->mbuf, trash.area, b_data(&h2c->mbuf));
	}

	if (outbuf.size < 9) {
		h2c->flags |= H2_CF_MUX_MFULL;
		h2s->flags |= H2_SF_BLK_MROOM;
		goto end;
	}

	/* len: 0x000000 (fill later), type: 0(DATA), flags: none=0 */
	memcpy(outbuf.area, "\x00\x00\x00\x00\x00", 5);
	write_n32(outbuf.area + 5, h2s->id); // 4 bytes
	outbuf.data = 9;

	/* we have in <fsize> the exact number of bytes we need to copy from
	 * the HTX buffer. We need to check this against the connection's and
	 * the stream's send windows, and to ensure that this fits in the max
	 * frame size and in the buffer's available space minus 9 bytes (for
	 * the frame header). The connection's flow control is applied last so
	 * that we can use a separate list of streams which are immediately
	 * unblocked on window opening. Note: we don't implement padding.
	 */

	/* EOM is presented with bsize==1 but would lead to the emission of an
	 * empty frame, thus we force it to zero here.
	 */
	if (type == HTX_BLK_EOM)
		bsize = fsize = 0;

	if (!fsize)
		goto send_empty;

	if (h2s->mws <= 0) {
		h2s->flags |= H2_SF_BLK_SFCTL;
		if (h2s->send_wait) {
			LIST_DEL(&h2s->list);
			LIST_INIT(&h2s->list);
		}
		goto end;
	}

	if (fsize > count)
		fsize = count;

	if (fsize > h2s->mws)
		fsize = h2s->mws; // >0

	if (h2c->mfs && fsize > h2c->mfs)
		fsize = h2c->mfs; // >0

	if (fsize + 9 > outbuf.size) {
		/* we have an opportunity for enlarging the too small
		 * available space, let's try.
		 * FIXME: is this really interesting to do? Maybe we'll
		 * spend lots of time realigning instead of using two
		 * frames.
		 */
		if (b_space_wraps(&h2c->mbuf))
			goto realign_again;
		fsize = outbuf.size - 9;

		if (fsize <= 0) {
			/* no need to send an empty frame here */
			h2c->flags |= H2_CF_MUX_MFULL;
			h2s->flags |= H2_SF_BLK_MROOM;
			goto end;
		}
	}

	if (h2c->mws <= 0) {
		h2s->flags |= H2_SF_BLK_MFCTL;
		goto end;
	}

	if (fsize > h2c->mws)
		fsize = h2c->mws;

	/* now let's copy this this into the output buffer */
	memcpy(outbuf.area + 9, htx_get_blk_ptr(htx, blk), fsize);
	h2s->mws -= fsize;
	h2c->mws -= fsize;
	count    -= fsize;

 send_empty:
	/* update the frame's size */
	h2_set_frame_size(outbuf.area, fsize);

	/* FIXME: for now we only set the ES flag on empty DATA frames, once
	 * meeting EOM. We should optimize this later.
	 */
	if (type == HTX_BLK_EOM) {
		total++; // EOM counts as one byte
		count--;
		es_now = 1;
	}

	if (es_now)
		outbuf.area[4] |= H2_F_DATA_END_STREAM;

	/* commit the H2 response */
	b_add(&h2c->mbuf, fsize + 9);

	/* consume incoming HTX block, including EOM */
	total += fsize;
	if (fsize == bsize) {
		htx_remove_blk(htx, blk);
		if (fsize)
			goto new_frame;
	} else {
		/* we've truncated this block */
		htx_cut_data_blk(htx, blk, fsize);
	}

	if (es_now) {
		if (h2s->st == H2_SS_OPEN)
			h2s->st = H2_SS_HLOC;
		else
			h2s_close(h2s);

		h2s->flags |= H2_SF_ES_SENT;
	}

 end:
	return total;
}