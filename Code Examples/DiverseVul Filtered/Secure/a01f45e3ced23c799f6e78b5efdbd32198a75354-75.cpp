static int h2_avail_streams(struct connection *conn)
{
	struct h2c *h2c = conn->ctx;

	/* XXX Should use the negociated max concurrent stream nb instead of the conf value */
	return (h2_settings_max_concurrent_streams - h2c->nb_streams);
}