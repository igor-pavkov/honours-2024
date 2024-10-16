static int __peernet2id(struct net *net, struct net *peer)
{
	bool no = false;

	return __peernet2id_alloc(net, peer, &no);
}