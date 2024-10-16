static __net_init int proto_init_net(struct net *net)
{
	if (!proc_net_fops_create(net, "protocols", S_IRUGO, &proto_seq_fops))
		return -ENOMEM;

	return 0;
}