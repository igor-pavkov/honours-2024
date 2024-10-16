static int ops_init(const struct pernet_operations *ops, struct net *net)
{
	int err = -ENOMEM;
	void *data = NULL;

	if (ops->id && ops->size) {
		data = kzalloc(ops->size, GFP_KERNEL);
		if (!data)
			goto out;

		err = net_assign_generic(net, *ops->id, data);
		if (err)
			goto cleanup;
	}
	err = 0;
	if (ops->init)
		err = ops->init(net);
	if (!err)
		return 0;

cleanup:
	kfree(data);

out:
	return err;
}