static int __register_pernet_operations(struct list_head *list,
					struct pernet_operations *ops)
{
	if (!init_net_initialized) {
		list_add_tail(&ops->list, list);
		return 0;
	}

	return ops_init(ops, &init_net);
}