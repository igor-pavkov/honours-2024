static void __unregister_pernet_operations(struct pernet_operations *ops)
{
	struct net *net;
	LIST_HEAD(net_exit_list);

	list_del(&ops->list);
	for_each_net(net)
		list_add_tail(&net->exit_list, &net_exit_list);
	ops_exit_list(ops, &net_exit_list);
	ops_free_list(ops, &net_exit_list);
}