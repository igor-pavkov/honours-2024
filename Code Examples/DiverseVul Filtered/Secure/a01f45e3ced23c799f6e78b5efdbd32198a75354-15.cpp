static void h2s_destroy(struct h2s *h2s)
{
	h2s_close(h2s);
	eb32_delete(&h2s->by_id);
	if (b_size(&h2s->rxbuf)) {
		b_free(&h2s->rxbuf);
		offer_buffers(NULL, tasks_run_queue);
	}
	if (h2s->send_wait != NULL)
		h2s->send_wait->events &= ~SUB_RETRY_SEND;
	if (h2s->recv_wait != NULL)
		h2s->recv_wait->events &= ~SUB_RETRY_RECV;
	/* There's no need to explicitly call unsubscribe here, the only
	 * reference left would be in the h2c send_list/fctl_list, and if
	 * we're in it, we're getting out anyway
	 */
	LIST_DEL(&h2s->list);
	LIST_INIT(&h2s->list);
	tasklet_free(h2s->wait_event.task);
	pool_free(pool_head_h2s, h2s);
}