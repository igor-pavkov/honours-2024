static int h2s_frt_make_resp_data(struct h2s *h2s, struct buffer *buf)
{
	struct h2c *h2c = h2s->h2c;
	struct h1m *h1m = &h2s->res;
	struct chunk outbuf;
	int ret = 0;
	int total = 0;
	int es_now = 0;
	int size = 0;
	char *blk1, *blk2;
	int len1, len2;

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
	if (!buf->o)
		goto end;

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
		goto end;
	}

	/* len: 0x000000 (fill later), type: 0(DATA), flags: none=0 */
	memcpy(outbuf.str, "\x00\x00\x00\x00\x00", 5);
	write_n32(outbuf.str + 5, h2s->id); // 4 bytes
	outbuf.len = 9;

	switch (h1m->flags & (H1_MF_CLEN|H1_MF_CHNK)) {
	case 0:           /* no content length, read till SHUTW */
		size = buf->o;
		h1m->curr_len = size;
		break;
	case H1_MF_CLEN:  /* content-length: read only h2m->body_len */
		size = buf->o;
		if ((long long)size > h1m->curr_len)
			size = h1m->curr_len;
		break;
	default:          /* te:chunked : parse chunks */
		if (h1m->state == HTTP_MSG_CHUNK_CRLF) {
			ret = h1_skip_chunk_crlf(buf, -buf->o, 0);
			if (!ret)
				goto end;

			if (ret < 0) {
				/* FIXME: bad contents. how to proceed here when we're in H2 ? */
				h1m->err_pos = ret;
				h2s_error(h2s, H2_ERR_INTERNAL_ERROR);
				goto end;
			}
			bo_del(buf, ret);
			total += ret;
			h1m->state = HTTP_MSG_CHUNK_SIZE;
		}

		if (h1m->state == HTTP_MSG_CHUNK_SIZE) {
			unsigned int chunk;

			ret = h1_parse_chunk_size(buf, -buf->o, 0, &chunk);
			if (!ret)
				goto end;

			if (ret < 0) {
				/* FIXME: bad contents. how to proceed here when we're in H2 ? */
				h1m->err_pos = ret;
				h2s_error(h2s, H2_ERR_INTERNAL_ERROR);
				goto end;
			}

			size = chunk;
			h1m->curr_len = chunk;
			h1m->body_len += chunk;
			bo_del(buf, ret);
			total += ret;
			h1m->state = size ? HTTP_MSG_DATA : HTTP_MSG_TRAILERS;
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

	if (size > buf->o)
		size = buf->o;

	if (size > h2s->mws)
		size = h2s->mws;

	if (size <= 0) {
		h2s->flags |= H2_SF_BLK_SFCTL;
		goto end;
	}

	if (h2c->mfs && size > h2c->mfs)
		size = h2c->mfs;

	if (size + 9 > outbuf.size) {
		/* we have an opportunity for enlarging the too small
		 * available space, let's try.
		 */
		if (buffer_space_wraps(h2c->mbuf))
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
	ret = bo_getblk_nc(buf, &blk1, &len1, &blk2, &len2);
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
	memcpy(outbuf.str + 9, blk1, len1);
	if (len2)
		memcpy(outbuf.str + 9 + len1, blk2, len2);

 send_empty:
	/* we may need to add END_STREAM */
	/* FIXME: we should also detect shutdown(w) below, but how ? Maybe we
	 * could rely on the MSG_MORE flag as a hint for this ?
	 */
	if (((h1m->flags & H1_MF_CLEN) && !(h1m->curr_len - size)) ||
	    !h1m->curr_len || h1m->state >= HTTP_MSG_DONE)
		es_now = 1;

	/* update the frame's size */
	h2_set_frame_size(outbuf.str, size);

	if (es_now)
		outbuf.str[4] |= H2_F_DATA_END_STREAM;

	/* commit the H2 response */
	h2c->mbuf->o += size + 9;
	h2c->mbuf->p = b_ptr(h2c->mbuf, size + 9);

	/* consume incoming H1 response */
	if (size > 0) {
		bo_del(buf, size);
		total += size;
		h1m->curr_len -= size;
		h2s->mws -= size;
		h2c->mws -= size;

		if (size && !h1m->curr_len && (h1m->flags & H1_MF_CHNK)) {
			h1m->state = HTTP_MSG_CHUNK_CRLF;
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
			bo_del(buf, buf->o);

			h1m->state = HTTP_MSG_DONE;
		}

		h2s->flags |= H2_SF_ES_SENT;
	}

 end:
	trace("[%d] sent simple H2 DATA response (sid=%d) = %d bytes out (%d in, st=%s, ep=%u, es=%s, h2cws=%d h2sws=%d) buf->o=%d", h2c->st0, h2s->id, size+9, total, h1_msg_state_str(h1m->state), h1m->err_pos, h1_msg_state_str(h1m->err_state), h2c->mws, h2s->mws, buf->o);
	return total;
}