static int nft_fwd_netdev_init(const struct nft_ctx *ctx,
			       const struct nft_expr *expr,
			       const struct nlattr * const tb[])
{
	struct nft_fwd_netdev *priv = nft_expr_priv(expr);

	if (tb[NFTA_FWD_SREG_DEV] == NULL)
		return -EINVAL;

	return nft_parse_register_load(tb[NFTA_FWD_SREG_DEV], &priv->sreg_dev,
				       sizeof(int));
}