static const struct conn_stream *h2_get_first_cs(const struct connection *conn)
{
	struct h2c *h2c = conn->ctx;
	struct h2s *h2s;
	struct eb32_node *node;

	node = eb32_first(&h2c->streams_by_id);
	while (node) {
		h2s = container_of(node, struct h2s, by_id);
		if (h2s->cs)
			return h2s->cs;
		node = eb32_next(node);
	}
	return NULL;
}