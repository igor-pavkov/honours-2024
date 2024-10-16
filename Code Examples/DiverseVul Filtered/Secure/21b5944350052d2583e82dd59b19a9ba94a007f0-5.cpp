static __net_exit void net_ns_net_exit(struct net *net)
{
	ns_free_inum(&net->ns);
}