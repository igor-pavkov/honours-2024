static size_t h2_snd_buf(struct conn_stream *cs, struct buffer *buf, size_t count, int flags)
{
	struct h2s *h2s = cs->ctx;
	size_t orig_count = count;
	size_t total = 0;
	size_t ret;
	struct htx *htx;
	struct htx_blk *blk;
	enum htx_blk_type btype;
	uint32_t bsize;
	int32_t idx;

	if (h2s->send_wait) {
		h2s->send_wait->events &= ~SUB_CALL_UNSUBSCRIBE;
		h2s->send_wait = NULL;
		LIST_DEL(&h2s->list);
		LIST_INIT(&h2s->list);
	}
	if (h2s->h2c->st0 < H2_CS_FRAME_H)
		return 0;

	/* htx will be enough to decide if we're using HTX or legacy */
	htx = (h2s->h2c->proxy->options2 & PR_O2_USE_HTX) ? htx_from_buf(buf) : NULL;

	if (!(h2s->flags & H2_SF_OUTGOING_DATA) && count)
		h2s->flags |= H2_SF_OUTGOING_DATA;

	if (h2s->id == 0) {
		int32_t id = h2c_get_next_sid(h2s->h2c);

		if (id < 0) {
			cs->ctx = NULL;
			cs->flags |= CS_FL_ERROR;
			h2s_destroy(h2s);
			return 0;
		}

		eb32_delete(&h2s->by_id);
		h2s->by_id.key = h2s->id = id;
		h2s->h2c->max_id = id;
		eb32_insert(&h2s->h2c->streams_by_id, &h2s->by_id);
	}

	if (htx) {
		while (h2s->st < H2_SS_ERROR && !(h2s->flags & H2_SF_BLK_ANY) &&
		       count && !htx_is_empty(htx)) {
			idx   = htx_get_head(htx);
			blk   = htx_get_blk(htx, idx);
			btype = htx_get_blk_type(blk);
			bsize = htx_get_blksz(blk);

			switch (btype) {
			case HTX_BLK_REQ_SL:
				/* start-line before headers */
				ret = h2s_htx_bck_make_req_headers(h2s, htx);
				if (ret > 0) {
					total += ret;
					count -= ret;
					if (ret < bsize)
						goto done;
				}
				break;

			case HTX_BLK_RES_SL:
				/* start-line before headers */
				ret = h2s_htx_frt_make_resp_headers(h2s, htx);
				if (ret > 0) {
					total += ret;
					count -= ret;
					if (ret < bsize)
						goto done;
				}
				break;

			case HTX_BLK_DATA:
			case HTX_BLK_EOD:
			case HTX_BLK_EOM:
				/* all these cause the emission of a DATA frame (possibly empty).
				 * This EOM necessarily is one before trailers, as the EOM following
				 * trailers would have been consumed by the trailers parser.
				 */
				ret = h2s_htx_frt_make_resp_data(h2s, buf, count);
				if (ret > 0) {
					htx = htx_from_buf(buf);
					total += ret;
					count -= ret;
					if (ret < bsize)
						goto done;
				}
				break;

			case HTX_BLK_TLR:
				/* This is the first trailers block, all the subsequent ones AND
				 * the EOM will be swallowed by the parser.
				 */
				ret = h2s_htx_make_trailers(h2s, htx);
				if (ret > 0) {
					total += ret;
					count -= ret;
					if (ret < bsize)
						goto done;
				}
				break;

			default:
				htx_remove_blk(htx, blk);
				total += bsize;
				count -= bsize;
				break;
			}
		}
		goto done;
	}

	/* legacy transfer mode */
	while (h2s->h1m.state < H1_MSG_DONE && count) {
		if (h2s->h1m.state <= H1_MSG_LAST_LF) {
			if (h2s->h2c->flags & H2_CF_IS_BACK)
				ret = -1;
			else
				ret = h2s_frt_make_resp_headers(h2s, buf, total, count);
		}
		else if (h2s->h1m.state < H1_MSG_TRAILERS) {
			ret = h2s_frt_make_resp_data(h2s, buf, total, count);
		}
		else if (h2s->h1m.state == H1_MSG_TRAILERS) {
			/* consume the trailers if any (we don't forward them for now) */
			ret = h1_measure_trailers(buf, total, count);

			if (unlikely((int)ret <= 0)) {
				if ((int)ret < 0)
					h2s_error(h2s, H2_ERR_INTERNAL_ERROR);
				break;
			}
			// trim any possibly pending data (eg: extra CR-LF, ...)
			total += count;
			count  = 0;
			h2s->h1m.state = H1_MSG_DONE;
			break;
		}
		else {
			cs_set_error(cs);
			break;
		}

		total += ret;
		count -= ret;

		if (h2s->st >= H2_SS_ERROR)
			break;

		if (h2s->flags & H2_SF_BLK_ANY)
			break;
	}

 done:
	if (h2s->st >= H2_SS_ERROR) {
		/* trim any possibly pending data after we close (extra CR-LF,
		 * unprocessed trailers, abnormal extra data, ...)
		 */
		total += count;
		count = 0;
	}

	/* RST are sent similarly to frame acks */
	if (h2s->st == H2_SS_ERROR || h2s->flags & H2_SF_RST_RCVD) {
		cs_set_error(cs);
		if (h2s_send_rst_stream(h2s->h2c, h2s) > 0)
			h2s_close(h2s);
	}

	if (htx) {
		htx_to_buf(htx, buf);
	} else {
		b_del(buf, total);
	}

	/* The mux is full, cancel the pending tasks */
	if ((h2s->h2c->flags & H2_CF_MUX_BLOCK_ANY) ||
	    (h2s->flags & H2_SF_BLK_MBUSY))
		h2_stop_senders(h2s->h2c);

	/* If we're running HTX, and we read the whole buffer, then pretend
	 * we read exactly what the caller specified, as with HTX the caller
	 * will always give the buffer size, instead of the amount of data
	 * available.
	 */
	if (htx && !b_data(buf))
		total = orig_count;

	if (total > 0) {
		if (!(h2s->h2c->wait_event.events & SUB_RETRY_SEND))
			tasklet_wakeup(h2s->h2c->wait_event.task);

	}
	/* If we're waiting for flow control, and we got a shutr on the
	 * connection, we will never be unlocked, so add an error on
	 * the conn_stream.
	 */
	if (conn_xprt_read0_pending(h2s->h2c->conn) &&
	    !b_data(&h2s->h2c->dbuf) &&
	    (h2s->flags & (H2_SF_BLK_SFCTL | H2_SF_BLK_MFCTL))) {
		if (cs->flags & CS_FL_EOS)
			cs->flags |= CS_FL_ERROR;
		else
			cs->flags |= CS_FL_ERR_PENDING;
	}
	return total;
}