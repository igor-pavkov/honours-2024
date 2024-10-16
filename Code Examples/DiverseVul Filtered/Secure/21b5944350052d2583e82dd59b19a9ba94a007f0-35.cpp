static int alloc_netid(struct net *net, struct net *peer, int reqid)
{
	int min = 0, max = 0;

	if (reqid >= 0) {
		min = reqid;
		max = reqid + 1;
	}

	return idr_alloc(&net->netns_ids, peer, min, max, GFP_ATOMIC);
}