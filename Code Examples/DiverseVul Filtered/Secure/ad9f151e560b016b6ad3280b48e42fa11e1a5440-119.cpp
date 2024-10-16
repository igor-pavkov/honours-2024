static int nf_tables_abort(struct net *net, struct sk_buff *skb,
			   enum nfnl_abort_action action)
{
	struct nftables_pernet *nft_net = nft_pernet(net);
	int ret = __nf_tables_abort(net, action);

	mutex_unlock(&nft_net->commit_mutex);

	return ret;
}