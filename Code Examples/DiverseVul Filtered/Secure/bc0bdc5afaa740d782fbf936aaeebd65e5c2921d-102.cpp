static void __exit cma_cleanup(void)
{
	cma_configfs_exit();
	ib_unregister_client(&cma_client);
	unregister_netdevice_notifier(&cma_nb);
	ib_sa_unregister_client(&sa_client);
	unregister_pernet_subsys(&cma_pernet_operations);
	destroy_workqueue(cma_wq);
}