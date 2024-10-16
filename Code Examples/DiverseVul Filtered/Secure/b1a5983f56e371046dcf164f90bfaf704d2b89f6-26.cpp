static int nft_dup_netdev_offload(struct nft_offload_ctx *ctx,
				  struct nft_flow_rule *flow,
				  const struct nft_expr *expr)
{
	const struct nft_dup_netdev *priv = nft_expr_priv(expr);
	int oif = ctx->regs[priv->sreg_dev].data.data[0];

	return nft_fwd_dup_netdev_offload(ctx, flow, FLOW_ACTION_MIRRED, oif);
}