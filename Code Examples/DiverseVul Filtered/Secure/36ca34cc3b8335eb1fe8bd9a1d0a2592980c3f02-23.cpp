static void sit_exit_net(struct net *net)
{
	struct sit_net *sitn;

	sitn = net_generic(net, sit_net_id);
	rtnl_lock();
	sit_destroy_tunnels(sitn);
	unregister_netdevice(sitn->fb_tunnel_dev);
	rtnl_unlock();
	kfree(sitn);
}