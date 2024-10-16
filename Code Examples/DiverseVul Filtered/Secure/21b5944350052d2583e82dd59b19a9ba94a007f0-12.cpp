static void ops_free_list(const struct pernet_operations *ops,
			  struct list_head *net_exit_list)
{
	struct net *net;
	if (ops->size && ops->id) {
		list_for_each_entry(net, net_exit_list, exit_list)
			ops_free(ops, net);
	}
}