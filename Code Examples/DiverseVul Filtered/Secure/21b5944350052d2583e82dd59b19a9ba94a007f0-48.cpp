int peernet2id(struct net *net, struct net *peer)
{
	int id;

	spin_lock_bh(&net->nsid_lock);
	id = __peernet2id(net, peer);
	spin_unlock_bh(&net->nsid_lock);
	return id;
}