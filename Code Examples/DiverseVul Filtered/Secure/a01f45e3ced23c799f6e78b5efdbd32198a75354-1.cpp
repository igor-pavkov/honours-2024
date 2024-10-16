static void __maybe_unused h2s_alert(struct h2s *h2s)
{
	if (h2s->recv_wait || h2s->send_wait) {
		h2s_notify_recv(h2s);
		h2s_notify_send(h2s);
	}
	else if (h2s->cs && h2s->cs->data_cb->wake != NULL)
		h2s->cs->data_cb->wake(h2s->cs);
}