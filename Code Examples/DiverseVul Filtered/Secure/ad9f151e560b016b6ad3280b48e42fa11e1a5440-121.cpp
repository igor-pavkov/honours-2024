static int __net_init nf_tables_init_net(struct net *net)
{
	struct nftables_pernet *nft_net = nft_pernet(net);

	INIT_LIST_HEAD(&nft_net->tables);
	INIT_LIST_HEAD(&nft_net->commit_list);
	INIT_LIST_HEAD(&nft_net->module_list);
	INIT_LIST_HEAD(&nft_net->notify_list);
	mutex_init(&nft_net->commit_mutex);
	nft_net->base_seq = 1;
	nft_net->validate_state = NFT_VALIDATE_SKIP;

	return 0;
}