static struct task *h2_deferred_shut(struct task *t, void *ctx, unsigned short state)
{
	struct h2s *h2s = ctx;
	long reason = (long)h2s->wait_event.handle;

	if (h2s->send_wait) {
		h2s->send_wait->events &= ~SUB_CALL_UNSUBSCRIBE;
		h2s->send_wait = NULL;
		LIST_DEL(&h2s->list);
		LIST_INIT(&h2s->list);
	}
	if (reason & 2)
		h2_do_shutw(h2s);
	if (reason & 1)
		h2_do_shutr(h2s);

	if (h2s->st == H2_SS_CLOSED &&
	    !((h2s->flags & (H2_SF_BLK_MBUSY | H2_SF_BLK_MROOM | H2_SF_BLK_MFCTL))) && !h2s->cs)
		h2s_destroy(h2s);
	return NULL;
}