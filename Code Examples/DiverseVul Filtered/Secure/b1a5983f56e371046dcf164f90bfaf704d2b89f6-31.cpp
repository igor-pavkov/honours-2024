static int nft_immediate_validate(const struct nft_ctx *ctx,
				  const struct nft_expr *expr,
				  const struct nft_data **d)
{
	const struct nft_immediate_expr *priv = nft_expr_priv(expr);
	struct nft_ctx *pctx = (struct nft_ctx *)ctx;
	const struct nft_data *data;
	int err;

	if (priv->dreg != NFT_REG_VERDICT)
		return 0;

	data = &priv->data;

	switch (data->verdict.code) {
	case NFT_JUMP:
	case NFT_GOTO:
		pctx->level++;
		err = nft_chain_validate(ctx, data->verdict.chain);
		if (err < 0)
			return err;
		pctx->level--;
		break;
	default:
		break;
	}

	return 0;
}