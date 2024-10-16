static int nft_set_flush(struct nft_ctx *ctx, struct nft_set *set, u8 genmask)
{
	struct nft_set_iter iter = {
		.genmask	= genmask,
		.fn		= nft_setelem_flush,
	};

	set->ops->walk(ctx, set, &iter);
	if (!iter.err)
		iter.err = nft_set_catchall_flush(ctx, set);

	return iter.err;
}