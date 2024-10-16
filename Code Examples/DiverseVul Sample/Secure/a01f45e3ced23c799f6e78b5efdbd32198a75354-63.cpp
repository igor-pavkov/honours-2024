static void h2_show_fd(struct buffer *msg, struct connection *conn)
{
	struct h2c *h2c = conn->ctx;
	struct h2s *h2s = NULL;
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

	h2s = NULL;
	node = eb32_first(&h2c->streams_by_id);
	while (node) {
		h2s = container_of(node, struct h2s, by_id);
		tree_cnt++;
		if (!h2s->cs)
			orph_cnt++;
		node = eb32_next(node);
	}

	chunk_appendf(msg, " h2c.st0=%d .err=%d .maxid=%d .lastid=%d .flg=0x%04x"
		      " .nbst=%u .nbcs=%u .fctl_cnt=%d .send_cnt=%d .tree_cnt=%d"
		      " .orph_cnt=%d .sub=%d .dsi=%d .dbuf=%u@%p+%u/%u .msi=%d .mbuf=%u@%p+%u/%u",
		      h2c->st0, h2c->errcode, h2c->max_id, h2c->last_sid, h2c->flags,
		      h2c->nb_streams, h2c->nb_cs, fctl_cnt, send_cnt, tree_cnt, orph_cnt,
		      h2c->wait_event.events, h2c->dsi,
		      (unsigned int)b_data(&h2c->dbuf), b_orig(&h2c->dbuf),
		      (unsigned int)b_head_ofs(&h2c->dbuf), (unsigned int)b_size(&h2c->dbuf),
		      h2c->msi,
		      (unsigned int)b_data(&h2c->mbuf), b_orig(&h2c->mbuf),
		      (unsigned int)b_head_ofs(&h2c->mbuf), (unsigned int)b_size(&h2c->mbuf));

	if (h2s) {
		chunk_appendf(msg, " last_h2s=%p .id=%d .flg=0x%04x .rxbuf=%u@%p+%u/%u .cs=%p",
			      h2s, h2s->id, h2s->flags,
			      (unsigned int)b_data(&h2s->rxbuf), b_orig(&h2s->rxbuf),
			      (unsigned int)b_head_ofs(&h2s->rxbuf), (unsigned int)b_size(&h2s->rxbuf),
			      h2s->cs);
		if (h2s->cs)
			chunk_appendf(msg, " .cs.flg=0x%08x .cs.data=%p",
				      h2s->cs->flags, h2s->cs->data);
	}
}