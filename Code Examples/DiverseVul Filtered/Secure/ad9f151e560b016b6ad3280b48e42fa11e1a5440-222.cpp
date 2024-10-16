static int nf_tables_expr_parse(const struct nft_ctx *ctx,
				const struct nlattr *nla,
				struct nft_expr_info *info)
{
	const struct nft_expr_type *type;
	const struct nft_expr_ops *ops;
	struct nlattr *tb[NFTA_EXPR_MAX + 1];
	int err;

	err = nla_parse_nested_deprecated(tb, NFTA_EXPR_MAX, nla,
					  nft_expr_policy, NULL);
	if (err < 0)
		return err;

	type = nft_expr_type_get(ctx->net, ctx->family, tb[NFTA_EXPR_NAME]);
	if (IS_ERR(type))
		return PTR_ERR(type);

	if (tb[NFTA_EXPR_DATA]) {
		err = nla_parse_nested_deprecated(info->tb, type->maxattr,
						  tb[NFTA_EXPR_DATA],
						  type->policy, NULL);
		if (err < 0)
			goto err1;
	} else
		memset(info->tb, 0, sizeof(info->tb[0]) * (type->maxattr + 1));

	if (type->select_ops != NULL) {
		ops = type->select_ops(ctx,
				       (const struct nlattr * const *)info->tb);
		if (IS_ERR(ops)) {
			err = PTR_ERR(ops);
#ifdef CONFIG_MODULES
			if (err == -EAGAIN)
				if (nft_expr_type_request_module(ctx->net,
								 ctx->family,
								 tb[NFTA_EXPR_NAME]) != -EAGAIN)
					err = -ENOENT;
#endif
			goto err1;
		}
	} else
		ops = type->ops;

	info->attr = nla;
	info->ops = ops;

	return 0;

err1:
	module_put(type->owner);
	return err;
}