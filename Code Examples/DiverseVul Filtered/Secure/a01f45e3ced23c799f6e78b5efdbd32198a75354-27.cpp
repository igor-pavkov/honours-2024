static void __maybe_unused h2s_notify_recv(struct h2s *h2s)
{
	struct wait_event *sw;

	if (h2s->recv_wait) {
		sw = h2s->recv_wait;
		sw->events &= ~SUB_RETRY_RECV;
		tasklet_wakeup(sw->task);
		h2s->recv_wait = NULL;
	}
}