static struct nft_hook *nft_netdev_hook_alloc(struct net *net,
					      const struct nlattr *attr)
{
	struct net_device *dev;
	char ifname[IFNAMSIZ];
	struct nft_hook *hook;
	int err;

	hook = kmalloc(sizeof(struct nft_hook), GFP_KERNEL);
	if (!hook) {
		err = -ENOMEM;
		goto err_hook_alloc;
	}

	nla_strscpy(ifname, attr, IFNAMSIZ);
	/* nf_tables_netdev_event() is called under rtnl_mutex, this is
	 * indirectly serializing all the other holders of the commit_mutex with
	 * the rtnl_mutex.
	 */
	dev = __dev_get_by_name(net, ifname);
	if (!dev) {
		err = -ENOENT;
		goto err_hook_dev;
	}
	hook->ops.dev = dev;
	hook->inactive = false;

	return hook;

err_hook_dev:
	kfree(hook);
err_hook_alloc:
	return ERR_PTR(err);
}