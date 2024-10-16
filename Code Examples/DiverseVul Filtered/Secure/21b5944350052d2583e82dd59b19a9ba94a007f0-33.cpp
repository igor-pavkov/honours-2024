static void ops_exit_list(const struct pernet_operations *ops,
			  struct list_head *net_exit_list)
{
	struct net *net;
	if (ops->exit) {
		list_for_each_entry(net, net_exit_list, exit_list)
			ops->exit(net);
	}
	if (ops->exit_batch)
		ops->exit_batch(net_exit_list);
}