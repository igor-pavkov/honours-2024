static void nft_netdev_unregister_hooks(struct net *net,
					struct list_head *hook_list)
{
	struct nft_hook *hook;

	list_for_each_entry(hook, hook_list, list)
		nf_unregister_net_hook(net, &hook->ops);
}