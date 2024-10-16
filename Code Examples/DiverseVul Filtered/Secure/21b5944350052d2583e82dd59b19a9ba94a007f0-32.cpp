static __net_init int net_ns_net_init(struct net *net)
{
#ifdef CONFIG_NET_NS
	net->ns.ops = &netns_operations;
#endif
	return ns_alloc_inum(&net->ns);
}