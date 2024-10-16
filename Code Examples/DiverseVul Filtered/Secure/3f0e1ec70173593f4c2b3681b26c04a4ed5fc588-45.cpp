static int h2_snd_buf(struct conn_stream *cs, struct buffer *buf, int flags)
{
	struct h2s *h2s = cs->ctx;
	int total = 0;

	if (!(h2s->flags & H2_SF_OUTGOING_DATA) && buf->o)
		h2s->flags |= H2_SF_OUTGOING_DATA;

	while (h2s->res.state < HTTP_MSG_DONE && buf->o) {
		if (h2s->res.state < HTTP_MSG_BODY) {
			total += h2s_frt_make_resp_headers(h2s, buf);

			if (h2s->st >= H2_SS_ERROR)
				break;

			if (h2s->flags & H2_SF_BLK_ANY)
				break;
		}
		else if (h2s->res.state < HTTP_MSG_TRAILERS) {
			total += h2s_frt_make_resp_data(h2s, buf);

			if (h2s->st >= H2_SS_ERROR)
				break;

			if (h2s->flags & H2_SF_BLK_ANY)
				break;
		}
		else if (h2s->res.state == HTTP_MSG_TRAILERS) {
			/* consume the trailers if any (we don't forward them for now) */
			int count = h1_measure_trailers(buf);

			if (unlikely(count <= 0)) {
				if (count < 0)
					h2s_error(h2s, H2_ERR_INTERNAL_ERROR);
				break;
			}
			total += count;
			bo_del(buf, count);

			// trim any possibly pending data (eg: extra CR-LF, ...)
			bo_del(buf, buf->o);

			h2s->res.state = HTTP_MSG_DONE;
			break;
		}
		else {
			cs->flags |= CS_FL_ERROR;
			break;
		}
	}

	/* RST are sent similarly to frame acks */
	if (h2s->st == H2_SS_ERROR || h2s->flags & H2_SF_RST_RCVD) {
		cs->flags |= CS_FL_ERROR;
		if (h2s_send_rst_stream(h2s->h2c, h2s) > 0)
			h2s_close(h2s);
	}

	if (h2s->flags & H2_SF_BLK_SFCTL) {
		/* stream flow control, quit the list */
		LIST_DEL(&h2s->list);
		LIST_INIT(&h2s->list);
	}
	else if (LIST_ISEMPTY(&h2s->list)) {
		if (h2s->flags & H2_SF_BLK_MFCTL)
			LIST_ADDQ(&h2s->h2c->fctl_list, &h2s->list);
		else if (h2s->flags & (H2_SF_BLK_MBUSY|H2_SF_BLK_MROOM))
			LIST_ADDQ(&h2s->h2c->send_list, &h2s->list);
	}

	return total;
}