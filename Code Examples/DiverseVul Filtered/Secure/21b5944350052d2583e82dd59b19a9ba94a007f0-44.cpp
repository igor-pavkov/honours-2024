static int __peernet2id_alloc(struct net *net, struct net *peer, bool *alloc)
{
	int id = idr_for_each(&net->netns_ids, net_eq_idr, peer);
	bool alloc_it = *alloc;

	*alloc = false;

	/* Magic value for id 0. */
	if (id == NET_ID_ZERO)
		return 0;
	if (id > 0)
		return id;

	if (alloc_it) {
		id = alloc_netid(net, peer, -1);
		*alloc = true;
		return id >= 0 ? id : NETNSA_NSID_NOT_ASSIGNED;
	}

	return NETNSA_NSID_NOT_ASSIGNED;
}