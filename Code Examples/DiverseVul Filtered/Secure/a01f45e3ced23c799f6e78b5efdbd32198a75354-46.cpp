static void h2_do_shutr(struct h2s *h2s)
{
	struct h2c *h2c = h2s->h2c;
	struct wait_event *sw = &h2s->wait_event;

	if (h2s->st == H2_SS_HLOC || h2s->st == H2_SS_ERROR || h2s->st == H2_SS_CLOSED)
		return;

	/* if no outgoing data was seen on this stream, it means it was
	 * closed with a "tcp-request content" rule that is normally
	 * used to kill the connection ASAP (eg: limit abuse). In this
	 * case we send a goaway to close the connection.
	 */
	if (!(h2s->flags & H2_SF_RST_SENT) &&
	    h2s_send_rst_stream(h2c, h2s) <= 0)
		goto add_to_list;

	if (!(h2s->flags & H2_SF_OUTGOING_DATA) &&
	    !(h2s->h2c->flags & (H2_CF_GOAWAY_SENT|H2_CF_GOAWAY_FAILED)) &&
	    h2c_send_goaway_error(h2c, h2s) <= 0)
		return;

	if (!(h2c->wait_event.events & SUB_RETRY_SEND))
		tasklet_wakeup(h2c->wait_event.task);
	h2s_close(h2s);

	return;
add_to_list:
	if (LIST_ISEMPTY(&h2s->list)) {
		sw->events |= SUB_RETRY_SEND;
		if (h2s->flags & H2_SF_BLK_MFCTL) {
			LIST_ADDQ(&h2c->fctl_list, &h2s->list);
			h2s->send_wait = sw;
		} else if (h2s->flags & (H2_SF_BLK_MBUSY|H2_SF_BLK_MROOM)) {
			h2s->send_wait = sw;
			LIST_ADDQ(&h2c->send_list, &h2s->list);
		}
	}
	/* Let the handler know we want shutr */
	sw->handle = (void *)((long)sw->handle | 1);

}