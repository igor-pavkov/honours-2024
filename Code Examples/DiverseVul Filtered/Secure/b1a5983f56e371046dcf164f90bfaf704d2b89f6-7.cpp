static void nft_fwd_netdev_eval(const struct nft_expr *expr,
				struct nft_regs *regs,
				const struct nft_pktinfo *pkt)
{
	struct nft_fwd_netdev *priv = nft_expr_priv(expr);
	int oif = regs->data[priv->sreg_dev];
	struct sk_buff *skb = pkt->skb;

	/* This is used by ifb only. */
	skb->skb_iif = skb->dev->ifindex;
	skb_set_redirected(skb, nft_hook(pkt) == NF_NETDEV_INGRESS);

	nf_fwd_netdev_egress(pkt, oif);
	regs->verdict.code = NF_STOLEN;
}