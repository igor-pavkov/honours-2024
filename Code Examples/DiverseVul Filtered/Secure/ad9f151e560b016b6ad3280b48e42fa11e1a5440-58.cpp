static int nft_delflowtable(struct nft_ctx *ctx,
			    struct nft_flowtable *flowtable)
{
	int err;

	err = nft_trans_flowtable_add(ctx, NFT_MSG_DELFLOWTABLE, flowtable);
	if (err < 0)
		return err;

	nft_deactivate_next(ctx->net, flowtable);
	ctx->table->use--;

	return err;
}