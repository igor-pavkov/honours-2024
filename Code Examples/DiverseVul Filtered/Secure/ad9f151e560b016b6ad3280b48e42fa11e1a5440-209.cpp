static int nf_tables_updtable(struct nft_ctx *ctx)
{
	struct nft_trans *trans;
	u32 flags;
	int ret;

	if (!ctx->nla[NFTA_TABLE_FLAGS])
		return 0;

	flags = ntohl(nla_get_be32(ctx->nla[NFTA_TABLE_FLAGS]));
	if (flags & ~NFT_TABLE_F_MASK)
		return -EOPNOTSUPP;

	if (flags == ctx->table->flags)
		return 0;

	if ((nft_table_has_owner(ctx->table) &&
	     !(flags & NFT_TABLE_F_OWNER)) ||
	    (!nft_table_has_owner(ctx->table) &&
	     flags & NFT_TABLE_F_OWNER))
		return -EOPNOTSUPP;

	trans = nft_trans_alloc(ctx, NFT_MSG_NEWTABLE,
				sizeof(struct nft_trans_table));
	if (trans == NULL)
		return -ENOMEM;

	if ((flags & NFT_TABLE_F_DORMANT) &&
	    !(ctx->table->flags & NFT_TABLE_F_DORMANT)) {
		ctx->table->flags |= NFT_TABLE_F_DORMANT;
		if (!(ctx->table->flags & __NFT_TABLE_F_UPDATE))
			ctx->table->flags |= __NFT_TABLE_F_WAS_AWAKEN;
	} else if (!(flags & NFT_TABLE_F_DORMANT) &&
		   ctx->table->flags & NFT_TABLE_F_DORMANT) {
		ctx->table->flags &= ~NFT_TABLE_F_DORMANT;
		if (!(ctx->table->flags & __NFT_TABLE_F_UPDATE)) {
			ret = nf_tables_table_enable(ctx->net, ctx->table);
			if (ret < 0)
				goto err_register_hooks;

			ctx->table->flags |= __NFT_TABLE_F_WAS_DORMANT;
		}
	}

	nft_trans_table_update(trans) = true;
	nft_trans_commit_list_add_tail(ctx->net, trans);

	return 0;

err_register_hooks:
	nft_trans_destroy(trans);
	return ret;
}