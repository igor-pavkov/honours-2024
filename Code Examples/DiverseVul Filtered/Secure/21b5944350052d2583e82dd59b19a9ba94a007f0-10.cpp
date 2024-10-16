static void net_free(struct net *net)
{
	kfree(rcu_access_pointer(net->gen));
	kmem_cache_free(net_cachep, net);
}