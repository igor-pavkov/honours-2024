static size_t h2s_frt_make_resp_data(struct h2s *h2s, const struct buffer *buf, size_t ofs, size_t max)
{
	struct h2c *h2c = h2s->h2c;
	struct h1m *h1m = &h2s->h1m;
	struct buffer outbuf;
	int ret = 0;
	size_t total = 0;
	int es_now = 0;
	int size = 0;
	const char *blk1, *blk2;
	size_t len1, len2;

	if (h2c_mux_busy(h2c, h2s)) {
		h2s->flags |= H2_SF_BLK_MBUSY;
		goto end;
	}

	if (!h2_get_buf(h2c, &h2c->mbuf)) {
		h2c->flags |= H2_CF_MUX_MALLOC;
		h2s->flags |= H2_SF_BLK_MROOM;
		goto end;
	}

 new_frame:
	if (!max)
		goto end;

	chunk_reset(&outbuf);

	while (1) {
		outbuf.area  = b_tail(&h2c->mbuf);
		outbuf.size = b_contig_space(&h2c->mbuf);
		outbuf.data = 0;

		if (outbuf.size >= 9 || !b_space_wraps(&h2c->mbuf))
			break;
	realign_again:
		/* If there are pending data in the output buffer, and we have
		 * less than 1/4 of the mbuf's size and everything fits, we'll
		 * still perform a copy anyway. Otherwise we'll pretend the mbuf
		 * is full and wait, to save some slow realign calls.
		 */
		if ((max + 9 > b_room(&h2c->mbuf) || max >= b_size(&h2c->mbuf) / 4)) {
			h2c->flags |= H2_CF_MUX_MFULL;
			h2s->flags |= H2_SF_BLK_MROOM;
			goto end;
		}

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

	switch (h1m->flags & (H1_MF_CLEN|H1_MF_CHNK)) {
	case 0:           /* no content length, read till SHUTW */
		size = max;
		h1m->curr_len = size;
		break;
	case H1_MF_CLEN:  /* content-length: read only h2m->body_len */
		size = max;
		if ((long long)size > h1m->curr_len)
			size = h1m->curr_len;
		break;
	default:          /* te:chunked : parse chunks */
		if (h1m->state == H1_MSG_CHUNK_CRLF) {
			ret = h1_skip_chunk_crlf(buf, ofs, ofs + max);
			if (!ret)
				goto end;

			if (ret < 0) {
				/* FIXME: bad contents. how to proceed here when we're in H2 ? */
				h1m->err_pos = ofs + max + ret;
				h2s_error(h2s, H2_ERR_INTERNAL_ERROR);
				goto end;
			}
			max -= ret;
			ofs += ret;
			total += ret;
			h1m->state = H1_MSG_CHUNK_SIZE;
		}

		if (h1m->state == H1_MSG_CHUNK_SIZE) {
			unsigned int chunk;
			ret = h1_parse_chunk_size(buf, ofs, ofs + max, &chunk);
			if (!ret)
				goto end;

			if (ret < 0) {
				/* FIXME: bad contents. how to proceed here when we're in H2 ? */
				h1m->err_pos = ofs + max + ret;
				h2s_error(h2s, H2_ERR_INTERNAL_ERROR);
				goto end;
			}

			size = chunk;
			h1m->curr_len = chunk;
			h1m->body_len += chunk;
			max -= ret;
			ofs += ret;
			total += ret;
			h1m->state = size ? H1_MSG_DATA : H1_MSG_TRAILERS;
			if (!size)
				goto send_empty;
		}

		/* in MSG_DATA state, continue below */
		size = h1m->curr_len;
		break;
	}

	/* we have in <size> the exact number of bytes we need to copy from
	 * the H1 buffer. We need to check this against the connection's and
	 * the stream's send windows, and to ensure that this fits in the max
	 * frame size and in the buffer's available space minus 9 bytes (for
	 * the frame header). The connection's flow control is applied last so
	 * that we can use a separate list of streams which are immediately
	 * unblocked on window opening. Note: we don't implement padding.
	 */

	if (size > max)
		size = max;

	if (size > h2s->mws)
		size = h2s->mws;

	if (size <= 0) {
		h2s->flags |= H2_SF_BLK_SFCTL;
		if (h2s->send_wait) {
			LIST_DEL(&h2s->list);
			LIST_INIT(&h2s->list);
		}
		goto end;
	}

	if (h2c->mfs && size > h2c->mfs)
		size = h2c->mfs;

	if (size + 9 > outbuf.size) {
		/* we have an opportunity for enlarging the too small
		 * available space, let's try.
		 */
		if (b_space_wraps(&h2c->mbuf))
			goto realign_again;
		size = outbuf.size - 9;
	}

	if (size <= 0) {
		h2c->flags |= H2_CF_MUX_MFULL;
		h2s->flags |= H2_SF_BLK_MROOM;
		goto end;
	}

	if (size > h2c->mws)
		size = h2c->mws;

	if (size <= 0) {
		h2s->flags |= H2_SF_BLK_MFCTL;
		goto end;
	}

	/* copy whatever we can */
	blk1 = blk2 = NULL; // silence a maybe-uninitialized warning
	ret = b_getblk_nc(buf, &blk1, &len1, &blk2, &len2, ofs, max);
	if (ret == 1)
		len2 = 0;

	if (!ret || len1 + len2 < size) {
		/* FIXME: must normally never happen */
		h2s_error(h2s, H2_ERR_INTERNAL_ERROR);
		goto end;
	}

	/* limit len1/len2 to size */
	if (len1 + len2 > size) {
		int sub = len1 + len2 - size;

		if (len2 > sub)
			len2 -= sub;
		else {
			sub -= len2;
			len2 = 0;
			len1 -= sub;
		}
	}

	/* now let's copy this this into the output buffer */
	memcpy(outbuf.area + 9, blk1, len1);
	if (len2)
		memcpy(outbuf.area + 9 + len1, blk2, len2);

 send_empty:
	/* we may need to add END_STREAM */
	/* FIXME: we should also detect shutdown(w) below, but how ? Maybe we
	 * could rely on the MSG_MORE flag as a hint for this ?
	 *
	 * FIXME: what we do here is not correct because we send end_stream
	 * before knowing if we'll have to send a HEADERS frame for the
	 * trailers. More importantly we're not consuming the trailing CRLF
	 * after the end of trailers, so it will be left to the caller to
	 * eat it. The right way to do it would be to measure trailers here
	 * and to send ES only if there are no trailers.
	 *
	 */
	if (((h1m->flags & H1_MF_CLEN) && !(h1m->curr_len - size)) ||
	    !h1m->curr_len || h1m->state >= H1_MSG_DONE)
		es_now = 1;

	/* update the frame's size */
	h2_set_frame_size(outbuf.area, size);

	if (es_now)
		outbuf.area[4] |= H2_F_DATA_END_STREAM;

	/* commit the H2 response */
	b_add(&h2c->mbuf, size + 9);

	/* consume incoming H1 response */
	if (size > 0) {
		max -= size;
		ofs += size;
		total += size;
		h1m->curr_len -= size;
		h2s->mws -= size;
		h2c->mws -= size;

		if (size && !h1m->curr_len && (h1m->flags & H1_MF_CHNK)) {
			h1m->state = H1_MSG_CHUNK_CRLF;
			goto new_frame;
		}
	}

	if (es_now) {
		if (h2s->st == H2_SS_OPEN)
			h2s->st = H2_SS_HLOC;
		else
			h2s_close(h2s);

		if (!(h1m->flags & H1_MF_CHNK)) {
			// trim any possibly pending data (eg: inconsistent content-length)
			total += max;
			ofs += max;
			max = 0;

			h1m->state = H1_MSG_DONE;
		}

		h2s->flags |= H2_SF_ES_SENT;
	}

 end:
	trace("[%d] sent simple H2 DATA response (sid=%d) = %d bytes out (%u in, st=%s, ep=%u, es=%s, h2cws=%d h2sws=%d) data=%u", h2c->st0, h2s->id, size+9, (unsigned int)total, h1m_state_str(h1m->state), h1m->err_pos, h1m_state_str(h1m->err_state), h2c->mws, h2s->mws, (unsigned int)b_data(buf));
	return total;
}