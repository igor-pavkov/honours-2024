static int h2_subscribe(struct conn_stream *cs, int event_type, void *param)
{
	struct wait_event *sw;
	struct h2s *h2s = cs->ctx;
	struct h2c *h2c = h2s->h2c;

	if (event_type & SUB_RETRY_RECV) {
		sw = param;
		if (!(sw->events & SUB_RETRY_RECV)) {
			sw->events |= SUB_RETRY_RECV;
			sw->handle = h2s;
			h2s->recv_wait = sw;
		}
		event_type &= ~SUB_RETRY_RECV;
	}
	if (event_type & SUB_RETRY_SEND) {
		sw = param;
		if (!(sw->events & SUB_RETRY_SEND)) {
			sw->events |= SUB_RETRY_SEND;
			sw->handle = h2s;
			h2s->send_wait = sw;
			if (!(h2s->flags & H2_SF_BLK_SFCTL)) {
				if (h2s->flags & H2_SF_BLK_MFCTL)
					LIST_ADDQ(&h2c->fctl_list, &h2s->list);
				else
					LIST_ADDQ(&h2c->send_list, &h2s->list);
			}
		}
		event_type &= ~SUB_RETRY_SEND;
	}
	if (event_type != 0)
		return -1;
	return 0;


}