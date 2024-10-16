static void __unregister_pernet_operations(struct pernet_operations *ops)
{
	if (!init_net_initialized) {
		list_del(&ops->list);
	} else {
		LIST_HEAD(net_exit_list);
		list_add(&init_net.exit_list, &net_exit_list);
		ops_exit_list(ops, &net_exit_list);
		ops_free_list(ops, &net_exit_list);
	}
}