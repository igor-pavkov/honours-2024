static __net_exit void proto_exit_net(struct net *net)
{
	proc_net_remove(net, "protocols");
}