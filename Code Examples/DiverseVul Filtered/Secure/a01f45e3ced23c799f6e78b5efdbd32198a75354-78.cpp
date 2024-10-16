static struct h2s *h2s_new(struct h2c *h2c, int id)
{
	struct h2s *h2s;

	h2s = pool_alloc(pool_head_h2s);
	if (!h2s)
		goto out;

	h2s->wait_event.task = tasklet_new();
	if (!h2s->wait_event.task) {
		pool_free(pool_head_h2s, h2s);
		goto out;
	}
	h2s->send_wait = NULL;
	h2s->recv_wait = NULL;
	h2s->wait_event.task->process = h2_deferred_shut;
	h2s->wait_event.task->context = h2s;
	h2s->wait_event.handle = NULL;
	h2s->wait_event.events = 0;
	LIST_INIT(&h2s->list);
	h2s->h2c       = h2c;
	h2s->cs        = NULL;
	h2s->mws       = h2c->miw;
	h2s->flags     = H2_SF_NONE;
	h2s->errcode   = H2_ERR_NO_ERROR;
	h2s->st        = H2_SS_IDLE;
	h2s->status    = 0;
	h2s->rxbuf     = BUF_NULL;

	if (h2c->flags & H2_CF_IS_BACK) {
		h1m_init_req(&h2s->h1m);
		h2s->h1m.err_pos = -1; // don't care about errors on the request path
		h2s->h1m.flags |= H1_MF_TOLOWER;
	} else {
		h1m_init_res(&h2s->h1m);
		h2s->h1m.err_pos = -1; // don't care about errors on the response path
		h2s->h1m.flags |= H1_MF_TOLOWER;
	}

	h2s->by_id.key = h2s->id = id;
	if (id > 0)
		h2c->max_id      = id;

	eb32_insert(&h2c->streams_by_id, &h2s->by_id);
	h2c->nb_streams++;

	return h2s;

 out_free_h2s:
	pool_free(pool_head_h2s, h2s);
 out:
	return NULL;
}