static int __nft_set_catchall_flush(const struct nft_ctx *ctx,
				    struct nft_set *set,
				    struct nft_set_elem *elem)
{
	struct nft_trans *trans;

	trans = nft_trans_alloc_gfp(ctx, NFT_MSG_DELSETELEM,
				    sizeof(struct nft_trans_elem), GFP_KERNEL);
	if (!trans)
		return -ENOMEM;

	nft_setelem_data_deactivate(ctx->net, set, elem);
	nft_trans_elem_set(trans) = set;
	nft_trans_elem(trans) = *elem;
	nft_trans_commit_list_add_tail(ctx->net, trans);

	return 0;
}