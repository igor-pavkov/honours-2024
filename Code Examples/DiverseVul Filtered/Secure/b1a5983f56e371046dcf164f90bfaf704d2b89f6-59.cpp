static int nft_immediate_dump(struct sk_buff *skb, const struct nft_expr *expr)
{
	const struct nft_immediate_expr *priv = nft_expr_priv(expr);

	if (nft_dump_register(skb, NFTA_IMMEDIATE_DREG, priv->dreg))
		goto nla_put_failure;

	return nft_data_dump(skb, NFTA_IMMEDIATE_DATA, &priv->data,
			     nft_dreg_to_type(priv->dreg), priv->dlen);

nla_put_failure:
	return -1;
}