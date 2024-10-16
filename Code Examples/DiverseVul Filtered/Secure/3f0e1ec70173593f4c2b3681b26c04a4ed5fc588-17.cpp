static void h2_shutw(struct conn_stream *cs, enum cs_shw_mode mode)
{
	struct h2s *h2s = cs->ctx;

	if (h2s->st == H2_SS_HLOC || h2s->st == H2_SS_ERROR || h2s->st == H2_SS_CLOSED)
		return;

	if (h2s->flags & H2_SF_HEADERS_SENT) {
		/* we can cleanly close using an empty data frame only after headers */

		if (!(h2s->flags & (H2_SF_ES_SENT|H2_SF_RST_SENT)) &&
		    h2_send_empty_data_es(h2s) <= 0)
			goto add_to_list;

		if (h2s->st == H2_SS_HREM)
			h2s_close(h2s);
		else
			h2s->st = H2_SS_HLOC;
	} else {
		/* if no outgoing data was seen on this stream, it means it was
		 * closed with a "tcp-request content" rule that is normally
		 * used to kill the connection ASAP (eg: limit abuse). In this
		 * case we send a goaway to close the connection.
		 */
		if (!(h2s->flags & H2_SF_RST_SENT) &&
		    h2s_send_rst_stream(h2s->h2c, h2s) <= 0)
			goto add_to_list;

		if (!(h2s->flags & H2_SF_OUTGOING_DATA) &&
		    !(h2s->h2c->flags & (H2_CF_GOAWAY_SENT|H2_CF_GOAWAY_FAILED)) &&
		    h2c_send_goaway_error(h2s->h2c, h2s) <= 0)
			goto add_to_list;

		h2s_close(h2s);
	}

	if (h2s->h2c->mbuf->o && !(cs->conn->flags & CO_FL_XPRT_WR_ENA))
		conn_xprt_want_send(cs->conn);

 add_to_list:
	if (LIST_ISEMPTY(&h2s->list)) {
		if (h2s->flags & H2_SF_BLK_MFCTL)
			LIST_ADDQ(&h2s->h2c->fctl_list, &h2s->list);
		else if (h2s->flags & (H2_SF_BLK_MBUSY|H2_SF_BLK_MROOM))
			LIST_ADDQ(&h2s->h2c->send_list, &h2s->list);
	}
}