static connection *connection_init(server *srv) {
	connection * const con = calloc(1, sizeof(*con));
	force_assert(NULL != con);

	con->fd = 0;
	con->ndx = -1;
	con->bytes_written = 0;
	con->bytes_read = 0;

	con->dst_addr_buf = buffer_init();
	con->srv  = srv;
	con->plugin_slots = srv->plugin_slots;
	con->config_data_base = srv->config_data_base;

	request_st * const r = &con->request;
	request_init_data(r, con, srv);
	config_reset_config(r);
	con->write_queue = &r->write_queue;
	con->read_queue = &r->read_queue;

	/* init plugin-specific per-connection structures */
	con->plugin_ctx = calloc(1, (srv->plugins.used + 1) * sizeof(void *));
	force_assert(NULL != con->plugin_ctx);

	return con;
}