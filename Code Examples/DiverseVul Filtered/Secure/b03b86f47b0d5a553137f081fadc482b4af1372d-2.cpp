static void connection_del(server *srv, connection *con) {
	connections * const conns = &srv->conns;

	if (-1 == con->ndx) return;
	uint32_t i = (uint32_t)con->ndx;

	/* not last element */

	if (i != --conns->used) {
		connection * const temp = conns->ptr[i];
		conns->ptr[i] = conns->ptr[conns->used];
		conns->ptr[conns->used] = temp;

		conns->ptr[i]->ndx = i;
		conns->ptr[conns->used]->ndx = -1;
	}

	con->ndx = -1;
#if 0
	fprintf(stderr, "%s.%d: del: (%d)", __FILE__, __LINE__, conns->used);
	for (i = 0; i < conns->used; i++) {
		fprintf(stderr, "%d ", conns->ptr[i]->fd);
	}
	fprintf(stderr, "\n");
#endif
}