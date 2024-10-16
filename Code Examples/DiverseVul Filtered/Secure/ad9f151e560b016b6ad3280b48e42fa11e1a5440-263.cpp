static int nft_set_elem_expr_dump(struct sk_buff *skb,
				  const struct nft_set *set,
				  const struct nft_set_ext *ext)
{
	struct nft_set_elem_expr *elem_expr;
	u32 size, num_exprs = 0;
	struct nft_expr *expr;
	struct nlattr *nest;

	elem_expr = nft_set_ext_expr(ext);
	nft_setelem_expr_foreach(expr, elem_expr, size)
		num_exprs++;

	if (num_exprs == 1) {
		expr = nft_setelem_expr_at(elem_expr, 0);
		if (nft_expr_dump(skb, NFTA_SET_ELEM_EXPR, expr) < 0)
			return -1;

		return 0;
	} else if (num_exprs > 1) {
		nest = nla_nest_start_noflag(skb, NFTA_SET_ELEM_EXPRESSIONS);
		if (nest == NULL)
			goto nla_put_failure;

		nft_setelem_expr_foreach(expr, elem_expr, size) {
			expr = nft_setelem_expr_at(elem_expr, size);
			if (nft_expr_dump(skb, NFTA_LIST_ELEM, expr) < 0)
				goto nla_put_failure;
		}
		nla_nest_end(skb, nest);
	}
	return 0;

nla_put_failure:
	return -1;
}