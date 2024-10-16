static struct h2s *h2c_stream_new(struct h2c *h2c, int id)
{
	struct conn_stream *cs;
	struct h2s *h2s;

	h2s = pool_alloc(pool_head_h2s);
	if (!h2s)
		goto out;

	h2s->h2c       = h2c;
	h2s->mws       = h2c->miw;
	h2s->flags     = H2_SF_NONE;
	h2s->errcode   = H2_ERR_NO_ERROR;
	h2s->st        = H2_SS_IDLE;
	h1m_init(&h2s->req);
	h1m_init(&h2s->res);
	h2s->by_id.key = h2s->id = id;
	h2c->max_id    = id;
	LIST_INIT(&h2s->list);

	eb32_insert(&h2c->streams_by_id, &h2s->by_id);
	h2c->nb_streams++;
	if (h2c->nb_streams > h2_settings_max_concurrent_streams)
		goto out_close;

	cs = cs_new(h2c->conn);
	if (!cs)
		goto out_close;

	h2s->cs = cs;
	cs->ctx = h2s;

	if (stream_create_from_cs(cs) < 0)
		goto out_free_cs;

	/* OK done, the stream lives its own life now */
	return h2s;

 out_free_cs:
	cs_free(cs);
 out_close:
	h2s_destroy(h2s);
	h2s = NULL;
 out:
	return h2s;
}