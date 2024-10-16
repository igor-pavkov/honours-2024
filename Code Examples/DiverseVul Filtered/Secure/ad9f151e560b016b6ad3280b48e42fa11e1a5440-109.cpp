static void nft_trans_commit_list_add_tail(struct net *net, struct nft_trans *trans)
{
	struct nftables_pernet *nft_net = nft_pernet(net);

	list_add_tail(&trans->list, &nft_net->commit_list);
}