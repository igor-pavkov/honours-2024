static int nft_chain_parse_netdev(struct net *net,
				  struct nlattr *tb[],
				  struct list_head *hook_list)
{
	struct nft_hook *hook;
	int err;

	if (tb[NFTA_HOOK_DEV]) {
		hook = nft_netdev_hook_alloc(net, tb[NFTA_HOOK_DEV]);
		if (IS_ERR(hook))
			return PTR_ERR(hook);

		list_add_tail(&hook->list, hook_list);
	} else if (tb[NFTA_HOOK_DEVS]) {
		err = nf_tables_parse_netdev_hooks(net, tb[NFTA_HOOK_DEVS],
						   hook_list);
		if (err < 0)
			return err;

		if (list_empty(hook_list))
			return -EINVAL;
	} else {
		return -EINVAL;
	}

	return 0;
}