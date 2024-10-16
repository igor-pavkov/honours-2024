static void __net_exit nf_tables_pre_exit_net(struct net *net)
{
	__nft_release_hooks(net);
}