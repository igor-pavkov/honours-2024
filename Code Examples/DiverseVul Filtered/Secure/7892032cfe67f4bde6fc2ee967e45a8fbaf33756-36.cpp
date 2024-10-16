static int __net_init ip6gre_init_net(struct net *net)
{
	struct ip6gre_net *ign = net_generic(net, ip6gre_net_id);
	int err;

	ign->fb_tunnel_dev = alloc_netdev(sizeof(struct ip6_tnl), "ip6gre0",
					  NET_NAME_UNKNOWN,
					  ip6gre_tunnel_setup);
	if (!ign->fb_tunnel_dev) {
		err = -ENOMEM;
		goto err_alloc_dev;
	}
	dev_net_set(ign->fb_tunnel_dev, net);
	/* FB netdevice is special: we have one, and only one per netns.
	 * Allowing to move it to another netns is clearly unsafe.
	 */
	ign->fb_tunnel_dev->features |= NETIF_F_NETNS_LOCAL;


	ip6gre_fb_tunnel_init(ign->fb_tunnel_dev);
	ign->fb_tunnel_dev->rtnl_link_ops = &ip6gre_link_ops;

	err = register_netdev(ign->fb_tunnel_dev);
	if (err)
		goto err_reg_dev;

	rcu_assign_pointer(ign->tunnels_wc[0],
			   netdev_priv(ign->fb_tunnel_dev));
	return 0;

err_reg_dev:
	ip6gre_dev_free(ign->fb_tunnel_dev);
err_alloc_dev:
	return err;
}