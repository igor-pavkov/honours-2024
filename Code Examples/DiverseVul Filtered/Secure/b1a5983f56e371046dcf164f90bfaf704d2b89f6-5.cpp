static int nft_immediate_offload(struct nft_offload_ctx *ctx,
				 struct nft_flow_rule *flow,
				 const struct nft_expr *expr)
{
	const struct nft_immediate_expr *priv = nft_expr_priv(expr);

	if (priv->dreg == NFT_REG_VERDICT)
		return nft_immediate_offload_verdict(ctx, flow, priv);

	memcpy(&ctx->regs[priv->dreg].data, &priv->data, sizeof(priv->data));

	return 0;
}