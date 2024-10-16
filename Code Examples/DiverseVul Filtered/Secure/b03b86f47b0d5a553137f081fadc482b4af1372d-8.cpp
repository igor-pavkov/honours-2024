static connection *connections_get_new_connection(server *srv) {
	connections * const conns = &srv->conns;
	size_t i;

	if (conns->size == conns->used) {
		conns->size += srv->max_conns >= 128 ? 128 : srv->max_conns > 16 ? 16 : srv->max_conns;
		conns->ptr = realloc(conns->ptr, sizeof(*conns->ptr) * conns->size);
		force_assert(NULL != conns->ptr);

		for (i = conns->used; i < conns->size; i++) {
			conns->ptr[i] = connection_init(srv);
			connection_reset(conns->ptr[i]);
		}
	}

	conns->ptr[conns->used]->ndx = conns->used;
	return conns->ptr[conns->used++];
}