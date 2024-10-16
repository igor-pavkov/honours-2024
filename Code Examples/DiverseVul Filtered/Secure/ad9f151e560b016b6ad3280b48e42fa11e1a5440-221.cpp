static int nf_tables_parse_netdev_hooks(struct net *net,
					const struct nlattr *attr,
					struct list_head *hook_list)
{
	struct nft_hook *hook, *next;
	const struct nlattr *tmp;
	int rem, n = 0, err;

	nla_for_each_nested(tmp, attr, rem) {
		if (nla_type(tmp) != NFTA_DEVICE_NAME) {
			err = -EINVAL;
			goto err_hook;
		}

		hook = nft_netdev_hook_alloc(net, tmp);
		if (IS_ERR(hook)) {
			err = PTR_ERR(hook);
			goto err_hook;
		}
		if (nft_hook_list_find(hook_list, hook)) {
			kfree(hook);
			err = -EEXIST;
			goto err_hook;
		}
		list_add_tail(&hook->list, hook_list);
		n++;

		if (n == NFT_NETDEVICE_MAX) {
			err = -EFBIG;
			goto err_hook;
		}
	}

	return 0;

err_hook:
	list_for_each_entry_safe(hook, next, hook_list, list) {
		list_del(&hook->list);
		kfree(hook);
	}
	return err;
}