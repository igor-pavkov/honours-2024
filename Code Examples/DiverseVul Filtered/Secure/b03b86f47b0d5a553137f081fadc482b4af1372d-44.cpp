void connections_free(server *srv) {
	connections * const conns = &srv->conns;
	for (uint32_t i = 0; i < conns->size; ++i) {
		connection *con = conns->ptr[i];
		request_st * const r = &con->request;

		connection_reset(con);
		if (con->write_queue != &r->write_queue)
			chunkqueue_free(con->write_queue);
		if (con->read_queue != &r->read_queue)
			chunkqueue_free(con->read_queue);
		request_free_data(r);

		free(con->plugin_ctx);
		buffer_free(con->dst_addr_buf);

		free(con);
	}

	free(conns->ptr);
	conns->ptr = NULL;
}