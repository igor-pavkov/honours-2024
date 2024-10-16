static int nft_delchain(struct nft_ctx *ctx)
{
	struct nft_trans *trans;

	trans = nft_trans_chain_add(ctx, NFT_MSG_DELCHAIN);
	if (IS_ERR(trans))
		return PTR_ERR(trans);

	ctx->table->use--;
	nft_deactivate_next(ctx->net, ctx->chain);

	return 0;
}