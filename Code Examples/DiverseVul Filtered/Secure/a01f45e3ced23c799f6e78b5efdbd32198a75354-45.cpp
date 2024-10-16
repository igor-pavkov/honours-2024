static struct h2s *h2c_bck_stream_new(struct h2c *h2c, struct conn_stream *cs, struct session *sess)
{
	struct h2s *h2s = NULL;

	if (h2c->nb_streams >= h2_settings_max_concurrent_streams)
		goto out;

	/* Defer choosing the ID until we send the first message to create the stream */
	h2s = h2s_new(h2c, 0);
	if (!h2s)
		goto out;

	h2s->cs = cs;
	h2s->sess = sess;
	cs->ctx = h2s;
	h2c->nb_cs++;

 out:
	return h2s;
}