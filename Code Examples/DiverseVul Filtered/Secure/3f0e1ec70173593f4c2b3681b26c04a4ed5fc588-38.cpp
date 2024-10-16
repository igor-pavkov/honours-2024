static void h2s_destroy(struct h2s *h2s)
{
	h2s_close(h2s);
	LIST_DEL(&h2s->list);
	LIST_INIT(&h2s->list);
	eb32_delete(&h2s->by_id);
	pool_free(pool_head_h2s, h2s);
}