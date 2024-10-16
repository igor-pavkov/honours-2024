static int h2_max_streams(struct connection *conn)
{
	/* XXX Should use the negociated max concurrent stream nb instead of the conf value */
	return h2_settings_max_concurrent_streams;
}