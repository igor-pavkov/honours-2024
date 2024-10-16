static void nft_dup_netdev_eval(const struct nft_expr *expr,
				struct nft_regs *regs,
				const struct nft_pktinfo *pkt)
{
	struct nft_dup_netdev *priv = nft_expr_priv(expr);
	int oif = regs->data[priv->sreg_dev];

	nf_dup_netdev_egress(pkt, oif);
}