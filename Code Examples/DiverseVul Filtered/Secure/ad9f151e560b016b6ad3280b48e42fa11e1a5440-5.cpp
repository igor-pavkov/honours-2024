static void __net_exit nf_tables_exit_net(struct net *net)
{
	struct nftables_pernet *nft_net = nft_pernet(net);

	mutex_lock(&nft_net->commit_mutex);
	if (!list_empty(&nft_net->commit_list))
		__nf_tables_abort(net, NFNL_ABORT_NONE);
	__nft_release_tables(net);
	mutex_unlock(&nft_net->commit_mutex);
	WARN_ON_ONCE(!list_empty(&nft_net->tables));
	WARN_ON_ONCE(!list_empty(&nft_net->module_list));
	WARN_ON_ONCE(!list_empty(&nft_net->notify_list));
}