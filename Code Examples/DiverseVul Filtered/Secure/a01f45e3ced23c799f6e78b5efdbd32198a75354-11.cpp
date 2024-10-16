static size_t h2_rcv_buf(struct conn_stream *cs, struct buffer *buf, size_t count, int flags)
{
	struct h2s *h2s = cs->ctx;
	struct h2c *h2c = h2s->h2c;
	struct htx *h2s_htx = NULL;
	struct htx *buf_htx = NULL;
	struct htx_ret htx_ret;
	size_t ret = 0;

	/* transfer possibly pending data to the upper layer */
	if (h2c->proxy->options2 & PR_O2_USE_HTX) {
		/* in HTX mode we ignore the count argument */
		h2s_htx = htx_from_buf(&h2s->rxbuf);
		if (htx_is_empty(h2s_htx)) {
			if (cs->flags & CS_FL_REOS)
				cs->flags |= CS_FL_EOS;
			if (cs->flags & CS_FL_ERR_PENDING)
				cs->flags |= CS_FL_ERROR;
			goto end;
		}

		buf_htx = htx_from_buf(buf);
		count = htx_free_space(buf_htx);

		htx_ret = htx_xfer_blks(buf_htx, h2s_htx, count, HTX_BLK_EOM);

		buf_htx->extra = h2s_htx->extra;
		htx_to_buf(buf_htx, buf);
		htx_to_buf(h2s_htx, &h2s->rxbuf);
		ret = htx_ret.ret;
	}
	else {
		ret = b_xfer(buf, &h2s->rxbuf, count);
	}

	if (b_data(&h2s->rxbuf))
		cs->flags |= (CS_FL_RCV_MORE | CS_FL_WANT_ROOM);
	else {
		cs->flags &= ~(CS_FL_RCV_MORE | CS_FL_WANT_ROOM);
		if (cs->flags & CS_FL_REOS)
			cs->flags |= CS_FL_EOS;
		if (cs->flags & CS_FL_ERR_PENDING)
			cs->flags |= CS_FL_ERROR;
		if (b_size(&h2s->rxbuf)) {
			b_free(&h2s->rxbuf);
			offer_buffers(NULL, tasks_run_queue);
		}
	}

	if (ret && h2c->dsi == h2s->id) {
		/* demux is blocking on this stream's buffer */
		h2c->flags &= ~H2_CF_DEM_SFULL;
		h2c_restart_reading(h2c);
	}
end:
	return ret;
}