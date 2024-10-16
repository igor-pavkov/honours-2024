static int h2_wake(struct connection *conn)
{
	struct h2c *h2c = conn->ctx;

	return (h2_process(h2c));
}