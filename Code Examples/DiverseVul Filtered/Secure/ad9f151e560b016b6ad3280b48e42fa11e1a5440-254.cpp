static void __exit nf_tables_module_exit(void)
{
	nfnetlink_subsys_unregister(&nf_tables_subsys);
	netlink_unregister_notifier(&nft_nl_notifier);
	nft_offload_exit();
	unregister_netdevice_notifier(&nf_tables_flowtable_notifier);
	nft_chain_filter_fini();
	nft_chain_route_fini();
	unregister_pernet_subsys(&nf_tables_net_ops);
	cancel_work_sync(&trans_destroy_work);
	rcu_barrier();
	rhltable_destroy(&nft_objname_ht);
	nf_tables_core_module_exit();
}