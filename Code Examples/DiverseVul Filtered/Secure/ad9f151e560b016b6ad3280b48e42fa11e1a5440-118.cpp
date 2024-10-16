static void nft_flowtable_event(unsigned long event, struct net_device *dev,
				struct nft_flowtable *flowtable)
{
	struct nft_hook *hook;

	list_for_each_entry(hook, &flowtable->hook_list, list) {
		if (hook->ops.dev != dev)
			continue;

		/* flow_offload_netdev_event() cleans up entries for us. */
		nft_unregister_flowtable_hook(dev_net(dev), flowtable, hook);
		list_del_rcu(&hook->list);
		kfree_rcu(hook, rcu);
		break;
	}
}