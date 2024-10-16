static struct h2s *h2c_frt_stream_new(struct h2c *h2c, int id)
{
	struct session *sess = h2c->conn->owner;
	struct conn_stream *cs;
	struct h2s *h2s;

	if (h2c->nb_streams >= h2_settings_max_concurrent_streams)
		goto out;

	h2s = h2s_new(h2c, id);
	if (!h2s)
		goto out;

	cs = cs_new(h2c->conn);
	if (!cs)
		goto out_close;

	cs->flags |= CS_FL_NOT_FIRST;
	h2s->cs = cs;
	cs->ctx = h2s;
	h2c->nb_cs++;

	if (stream_create_from_cs(cs) < 0)
		goto out_free_cs;

	/* We want the accept date presented to the next stream to be the one
	 * we have now, the handshake time to be null (since the next stream
	 * is not delayed by a handshake), and the idle time to count since
	 * right now.
	 */
	sess->accept_date = date;
	sess->tv_accept   = now;
	sess->t_handshake = 0;

	/* OK done, the stream lives its own life now */
	if (h2_has_too_many_cs(h2c))
		h2c->flags |= H2_CF_DEM_TOOMANY;
	return h2s;

 out_free_cs:
	h2c->nb_cs--;
	cs_free(cs);
 out_close:
	h2s_destroy(h2s);
 out:
	sess_log(sess);
	return NULL;
}