int peernet2id_alloc(struct net *net, struct net *peer)
{
	bool alloc;
	int id;

	if (atomic_read(&net->count) == 0)
		return NETNSA_NSID_NOT_ASSIGNED;
	spin_lock_bh(&net->nsid_lock);
	alloc = atomic_read(&peer->count) == 0 ? false : true;
	id = __peernet2id_alloc(net, peer, &alloc);
	spin_unlock_bh(&net->nsid_lock);
	if (alloc && id >= 0)
		rtnl_net_notifyid(net, RTM_NEWNSID, id);
	return id;
}