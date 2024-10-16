static int nft_fwd_netdev_dump(struct sk_buff *skb, const struct nft_expr *expr)
{
	struct nft_fwd_netdev *priv = nft_expr_priv(expr);

	if (nft_dump_register(skb, NFTA_FWD_SREG_DEV, priv->sreg_dev))
		goto nla_put_failure;

	return 0;

nla_put_failure:
	return -1;
}