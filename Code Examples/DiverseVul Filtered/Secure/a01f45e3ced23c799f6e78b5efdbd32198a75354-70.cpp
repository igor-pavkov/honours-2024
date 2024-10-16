static void h2_stop_senders(struct h2c *h2c)
{
	struct h2s *h2s, *h2s_back;

	list_for_each_entry_safe(h2s, h2s_back, &h2c->sending_list, list) {
		/* Don't unschedule the stream if the mux is just busy waiting for more data fro mthat stream */
		if (h2c->msi == h2s_id(h2s))
			continue;
		LIST_DEL(&h2s->list);
		LIST_INIT(&h2s->list);
		task_remove_from_task_list((struct task *)h2s->send_wait->task);
		h2s->send_wait->events |= SUB_RETRY_SEND;
		h2s->send_wait->events &= ~SUB_CALL_UNSUBSCRIBE;
		LIST_ADD(&h2c->send_list, &h2s->list);
	}
}