static int __init nf_tables_module_init(void)
{
	int err;

	err = register_pernet_subsys(&nf_tables_net_ops);
	if (err < 0)
		return err;

	err = nft_chain_filter_init();
	if (err < 0)
		goto err_chain_filter;

	err = nf_tables_core_module_init();
	if (err < 0)
		goto err_core_module;

	err = register_netdevice_notifier(&nf_tables_flowtable_notifier);
	if (err < 0)
		goto err_netdev_notifier;

	err = rhltable_init(&nft_objname_ht, &nft_objname_ht_params);
	if (err < 0)
		goto err_rht_objname;

	err = nft_offload_init();
	if (err < 0)
		goto err_offload;

	err = netlink_register_notifier(&nft_nl_notifier);
	if (err < 0)
		goto err_netlink_notifier;

	/* must be last */
	err = nfnetlink_subsys_register(&nf_tables_subsys);
	if (err < 0)
		goto err_nfnl_subsys;

	nft_chain_route_init();

	return err;

err_nfnl_subsys:
	netlink_unregister_notifier(&nft_nl_notifier);
err_netlink_notifier:
	nft_offload_exit();
err_offload:
	rhltable_destroy(&nft_objname_ht);
err_rht_objname:
	unregister_netdevice_notifier(&nf_tables_flowtable_notifier);
err_netdev_notifier:
	nf_tables_core_module_exit();
err_core_module:
	nft_chain_filter_fini();
err_chain_filter:
	unregister_pernet_subsys(&nf_tables_net_ops);
	return err;
}