static int nft_fwd_validate(const struct nft_ctx *ctx,
			    const struct nft_expr *expr,
			    const struct nft_data **data)
{
	return nft_chain_validate_hooks(ctx->chain, (1 << NF_NETDEV_INGRESS) |
						    (1 << NF_NETDEV_EGRESS));
}