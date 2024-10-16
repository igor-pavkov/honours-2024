static void h2_show_fd(struct chunk *msg, struct connection *conn)
{
	struct h2c *h2c = conn->mux_ctx;
	struct h2s *h2s;
	struct eb32_node *node;
	int fctl_cnt = 0;
	int send_cnt = 0;
	int tree_cnt = 0;
	int orph_cnt = 0;

	if (!h2c)
		return;

	list_for_each_entry(h2s, &h2c->fctl_list, list)
		fctl_cnt++;

	list_for_each_entry(h2s, &h2c->send_list, list)
		send_cnt++;

	node = eb32_first(&h2c->streams_by_id);
	while (node) {
		h2s = container_of(node, struct h2s, by_id);
		tree_cnt++;
		if (!h2s->cs)
			orph_cnt++;
		node = eb32_next(node);
	}

	chunk_appendf(msg, " st0=%d flg=0x%08x fctl_cnt=%d send_cnt=%d tree_cnt=%d orph_cnt=%d",
		      h2c->st0, h2c->flags, fctl_cnt, send_cnt, tree_cnt, orph_cnt);
}