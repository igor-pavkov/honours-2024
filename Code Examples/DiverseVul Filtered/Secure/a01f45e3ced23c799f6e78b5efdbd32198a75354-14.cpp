static struct conn_stream *h2_attach(struct connection *conn, struct session *sess)
{
	struct conn_stream *cs;
	struct h2s *h2s;
	struct h2c *h2c = conn->ctx;

	cs = cs_new(conn);
	if (!cs)
		return NULL;
	h2s = h2c_bck_stream_new(h2c, cs, sess);
	if (!h2s) {
		cs_free(cs);
		return NULL;
	}
	return cs;
}