static void nft_set_trans_bind(const struct nft_ctx *ctx, struct nft_set *set)
{
	struct nftables_pernet *nft_net;
	struct net *net = ctx->net;
	struct nft_trans *trans;

	if (!nft_set_is_anonymous(set))
		return;

	nft_net = nft_pernet(net);
	list_for_each_entry_reverse(trans, &nft_net->commit_list, list) {
		switch (trans->msg_type) {
		case NFT_MSG_NEWSET:
			if (nft_trans_set(trans) == set)
				nft_trans_set_bound(trans) = true;
			break;
		case NFT_MSG_NEWSETELEM:
			if (nft_trans_elem_set(trans) == set)
				nft_trans_elem_set_bound(trans) = true;
			break;
		}
	}
}