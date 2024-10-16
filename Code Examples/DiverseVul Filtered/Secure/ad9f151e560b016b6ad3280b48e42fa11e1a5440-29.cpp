static int nft_setelem_get(struct nft_ctx *ctx, struct nft_set *set,
			   struct nft_set_elem *elem, u32 flags)
{
	void *priv;

	if (!(flags & NFT_SET_ELEM_CATCHALL)) {
		priv = set->ops->get(ctx->net, set, elem, flags);
		if (IS_ERR(priv))
			return PTR_ERR(priv);
	} else {
		priv = nft_setelem_catchall_get(ctx->net, set);
		if (!priv)
			return -ENOENT;
	}
	elem->priv = priv;

	return 0;
}