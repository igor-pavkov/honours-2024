static int nft_netdev_register_hooks(struct net *net,
				     struct list_head *hook_list)
{
	struct nft_hook *hook;
	int err, j;

	j = 0;
	list_for_each_entry(hook, hook_list, list) {
		err = nf_register_net_hook(net, &hook->ops);
		if (err < 0)
			goto err_register;

		j++;
	}
	return 0;

err_register:
	list_for_each_entry(hook, hook_list, list) {
		if (j-- <= 0)
			break;

		nf_unregister_net_hook(net, &hook->ops);
	}
	return err;
}