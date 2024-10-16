static int h2_init(struct connection *conn)
{
	if (conn->mux_ctx) {
		/* we don't support outgoing connections for now */
		return -1;
	}

	return h2c_frt_init(conn);
}