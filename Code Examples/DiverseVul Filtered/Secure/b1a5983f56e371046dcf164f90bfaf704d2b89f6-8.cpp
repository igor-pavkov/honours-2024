static int nft_fwd_neigh_init(const struct nft_ctx *ctx,
			      const struct nft_expr *expr,
			      const struct nlattr * const tb[])
{
	struct nft_fwd_neigh *priv = nft_expr_priv(expr);
	unsigned int addr_len;
	int err;

	if (!tb[NFTA_FWD_SREG_DEV] ||
	    !tb[NFTA_FWD_SREG_ADDR] ||
	    !tb[NFTA_FWD_NFPROTO])
		return -EINVAL;

	priv->nfproto = ntohl(nla_get_be32(tb[NFTA_FWD_NFPROTO]));

	switch (priv->nfproto) {
	case NFPROTO_IPV4:
		addr_len = sizeof(struct in_addr);
		break;
	case NFPROTO_IPV6:
		addr_len = sizeof(struct in6_addr);
		break;
	default:
		return -EOPNOTSUPP;
	}

	err = nft_parse_register_load(tb[NFTA_FWD_SREG_DEV], &priv->sreg_dev,
				      sizeof(int));
	if (err < 0)
		return err;

	return nft_parse_register_load(tb[NFTA_FWD_SREG_ADDR], &priv->sreg_addr,
				       addr_len);
}