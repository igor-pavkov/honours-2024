static void __maybe_unused h2s_notify_send(struct h2s *h2s)
{
	struct wait_event *sw;

	if (h2s->send_wait) {
		sw = h2s->send_wait;
		sw->events &= ~SUB_RETRY_SEND;
		tasklet_wakeup(sw->task);
		h2s->send_wait = NULL;
		LIST_DEL(&h2s->list);
		LIST_INIT(&h2s->list);
	}
}