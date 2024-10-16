static int __net_init packet_net_init(struct net *net)
{
	mutex_init(&net->packet.sklist_lock);
	INIT_HLIST_HEAD(&net->packet.sklist);

#ifdef CONFIG_PROC_FS
	if (!proc_create_net("packet", 0, net->proc_net, &packet_seq_ops,
			sizeof(struct seq_net_private)))
		return -ENOMEM;
#endif /* CONFIG_PROC_FS */

	return 0;
}