static void ipv6_route_seq_setup_walk(struct ipv6_route_iter *iter,
				      struct net *net)
{
	memset(&iter->w, 0, sizeof(iter->w));
	iter->w.func = ipv6_route_yield;
	iter->w.root = &iter->tbl->tb6_root;
	iter->w.state = FWS_INIT;
	iter->w.node = iter->w.root;
	iter->w.args = iter;
	iter->sernum = iter->w.root->fn_sernum;
	INIT_LIST_HEAD(&iter->w.lh);
	fib6_walker_link(net, &iter->w);
}