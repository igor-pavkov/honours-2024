static int nft_dup_netdev_init(const struct nft_ctx *ctx,
			       const struct nft_expr *expr,
			       const struct nlattr * const tb[])
{
	struct nft_dup_netdev *priv = nft_expr_priv(expr);

	if (tb[NFTA_DUP_SREG_DEV] == NULL)
		return -EINVAL;

	return nft_parse_register_load(tb[NFTA_DUP_SREG_DEV], &priv->sreg_dev,
				       sizeof(int));
}