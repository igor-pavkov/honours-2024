static int nft_immediate_init(const struct nft_ctx *ctx,
			      const struct nft_expr *expr,
			      const struct nlattr * const tb[])
{
	struct nft_immediate_expr *priv = nft_expr_priv(expr);
	struct nft_data_desc desc;
	int err;

	if (tb[NFTA_IMMEDIATE_DREG] == NULL ||
	    tb[NFTA_IMMEDIATE_DATA] == NULL)
		return -EINVAL;

	err = nft_data_init(ctx, &priv->data, sizeof(priv->data), &desc,
			    tb[NFTA_IMMEDIATE_DATA]);
	if (err < 0)
		return err;

	priv->dlen = desc.len;

	err = nft_parse_register_store(ctx, tb[NFTA_IMMEDIATE_DREG],
				       &priv->dreg, &priv->data, desc.type,
				       desc.len);
	if (err < 0)
		goto err1;

	if (priv->dreg == NFT_REG_VERDICT) {
		struct nft_chain *chain = priv->data.verdict.chain;

		switch (priv->data.verdict.code) {
		case NFT_JUMP:
		case NFT_GOTO:
			if (nft_chain_is_bound(chain)) {
				err = -EBUSY;
				goto err1;
			}
			chain->bound = true;
			break;
		default:
			break;
		}
	}

	return 0;

err1:
	nft_data_release(&priv->data, desc.type);
	return err;
}