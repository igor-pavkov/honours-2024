bool peernet_has_id(struct net *net, struct net *peer)
{
	return peernet2id(net, peer) >= 0;
}