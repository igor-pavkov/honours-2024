static int __net_init net_defaults_init_net(struct net *net)
{
	net->core.sysctl_somaxconn = SOMAXCONN;
	return 0;
}