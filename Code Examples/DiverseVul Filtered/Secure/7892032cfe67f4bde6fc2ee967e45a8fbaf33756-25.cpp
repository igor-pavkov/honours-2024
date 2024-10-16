static void __net_exit ip6gre_exit_net(struct net *net)
{
	LIST_HEAD(list);

	rtnl_lock();
	ip6gre_destroy_tunnels(net, &list);
	unregister_netdevice_many(&list);
	rtnl_unlock();
}