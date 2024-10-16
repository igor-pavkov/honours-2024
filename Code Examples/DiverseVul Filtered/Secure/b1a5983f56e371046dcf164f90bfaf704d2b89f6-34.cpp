void nft_immediate_eval(const struct nft_expr *expr,
			struct nft_regs *regs,
			const struct nft_pktinfo *pkt)
{
	const struct nft_immediate_expr *priv = nft_expr_priv(expr);

	nft_data_copy(&regs->data[priv->dreg], &priv->data, priv->dlen);
}