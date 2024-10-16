static void h2_destroy(struct connection *conn)
{
	struct h2c *h2c = conn->ctx;

	if (eb_is_empty(&h2c->streams_by_id))
		h2_release(h2c->conn);
}