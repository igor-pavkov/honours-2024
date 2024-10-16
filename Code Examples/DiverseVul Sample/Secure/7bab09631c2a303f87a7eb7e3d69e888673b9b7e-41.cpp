void xfrm_garbage_collect(struct net *net)
{
	flow_cache_flush(net);
}