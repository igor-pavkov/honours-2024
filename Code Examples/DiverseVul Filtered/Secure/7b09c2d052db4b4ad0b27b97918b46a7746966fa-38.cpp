static void ipv6_route_check_sernum(struct ipv6_route_iter *iter)
{
	if (iter->sernum != iter->w.root->fn_sernum) {
		iter->sernum = iter->w.root->fn_sernum;
		iter->w.state = FWS_INIT;
		iter->w.node = iter->w.root;
		WARN_ON(iter->w.skip);
		iter->w.skip = iter->w.count;
	}
}