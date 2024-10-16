static int h2_unsubscribe(struct conn_stream *cs, int event_type, void *param)
{
	struct wait_event *sw;
	struct h2s *h2s = cs->ctx;

	if (event_type & SUB_RETRY_RECV) {
		sw = param;
		if (h2s->recv_wait == sw) {
			sw->events &= ~SUB_RETRY_RECV;
			h2s->recv_wait = NULL;
		}
	}
	if (event_type & SUB_RETRY_SEND) {
		sw = param;
		if (h2s->send_wait == sw) {
			LIST_DEL(&h2s->list);
			LIST_INIT(&h2s->list);
			sw->events &= ~SUB_RETRY_SEND;
			h2s->send_wait = NULL;
		}
	}
	if (event_type & SUB_CALL_UNSUBSCRIBE) {
		sw = param;
		if (h2s->send_wait == sw) {
			sw->events &= ~SUB_CALL_UNSUBSCRIBE;
			h2s->send_wait = NULL;
			LIST_DEL(&h2s->list);
			LIST_INIT(&h2s->list);
		}
	}
	return 0;
}