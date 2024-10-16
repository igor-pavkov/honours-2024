static int __init cma_init(void)
{
	int ret;

	/*
	 * There is a rare lock ordering dependency in cma_netdev_callback()
	 * that only happens when bonding is enabled. Teach lockdep that rtnl
	 * must never be nested under lock so it can find these without having
	 * to test with bonding.
	 */
	if (IS_ENABLED(CONFIG_LOCKDEP)) {
		rtnl_lock();
		mutex_lock(&lock);
		mutex_unlock(&lock);
		rtnl_unlock();
	}

	cma_wq = alloc_ordered_workqueue("rdma_cm", WQ_MEM_RECLAIM);
	if (!cma_wq)
		return -ENOMEM;

	ret = register_pernet_subsys(&cma_pernet_operations);
	if (ret)
		goto err_wq;

	ib_sa_register_client(&sa_client);
	register_netdevice_notifier(&cma_nb);

	ret = ib_register_client(&cma_client);
	if (ret)
		goto err;

	ret = cma_configfs_init();
	if (ret)
		goto err_ib;

	return 0;

err_ib:
	ib_unregister_client(&cma_client);
err:
	unregister_netdevice_notifier(&cma_nb);
	ib_sa_unregister_client(&sa_client);
	unregister_pernet_subsys(&cma_pernet_operations);
err_wq:
	destroy_workqueue(cma_wq);
	return ret;
}