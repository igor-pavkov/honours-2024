int bt_procfs_init(struct module* module, struct net *net, const char *name,
		   struct bt_sock_list* sk_list,
		   int (* seq_show)(struct seq_file *, void *))
{
	struct proc_dir_entry * pde;

	sk_list->custom_seq_show = seq_show;

	sk_list->fops.owner     = module;
	sk_list->fops.open      = bt_seq_open;
	sk_list->fops.read      = seq_read;
	sk_list->fops.llseek    = seq_lseek;
	sk_list->fops.release   = seq_release_private;

	pde = proc_create(name, 0, net->proc_net, &sk_list->fops);
	if (!pde)
		return -ENOMEM;

	pde->data = sk_list;

	return 0;
}