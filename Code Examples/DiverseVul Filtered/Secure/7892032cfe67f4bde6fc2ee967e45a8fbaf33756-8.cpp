static void ip6gre_destroy_tunnels(struct net *net, struct list_head *head)
{
	struct ip6gre_net *ign = net_generic(net, ip6gre_net_id);
	struct net_device *dev, *aux;
	int prio;

	for_each_netdev_safe(net, dev, aux)
		if (dev->rtnl_link_ops == &ip6gre_link_ops ||
		    dev->rtnl_link_ops == &ip6gre_tap_ops)
			unregister_netdevice_queue(dev, head);

	for (prio = 0; prio < 4; prio++) {
		int h;
		for (h = 0; h < IP6_GRE_HASH_SIZE; h++) {
			struct ip6_tnl *t;

			t = rtnl_dereference(ign->tunnels[prio][h]);

			while (t) {
				/* If dev is in the same netns, it has already
				 * been added to the list by the previous loop.
				 */
				if (!net_eq(dev_net(t->dev), net))
					unregister_netdevice_queue(t->dev,
								   head);
				t = rtnl_dereference(t->next);
			}
		}
	}
}