void xfrm_garbage_collect_deferred(struct net *net)
{
	flow_cache_flush_deferred(net);
}