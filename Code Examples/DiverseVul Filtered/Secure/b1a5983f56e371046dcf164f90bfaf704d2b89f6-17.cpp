static int nft_fwd_neigh_dump(struct sk_buff *skb, const struct nft_expr *expr)
{
	struct nft_fwd_neigh *priv = nft_expr_priv(expr);

	if (nft_dump_register(skb, NFTA_FWD_SREG_DEV, priv->sreg_dev) ||
	    nft_dump_register(skb, NFTA_FWD_SREG_ADDR, priv->sreg_addr) ||
	    nla_put_be32(skb, NFTA_FWD_NFPROTO, htonl(priv->nfproto)))
		goto nla_put_failure;

	return 0;

nla_put_failure:
	return -1;
}