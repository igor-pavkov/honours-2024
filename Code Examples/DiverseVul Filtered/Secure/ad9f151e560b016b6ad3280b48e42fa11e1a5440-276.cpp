static int nf_tables_newexpr(const struct nft_ctx *ctx,
			     const struct nft_expr_info *expr_info,
			     struct nft_expr *expr)
{
	const struct nft_expr_ops *ops = expr_info->ops;
	int err;

	expr->ops = ops;
	if (ops->init) {
		err = ops->init(ctx, expr, (const struct nlattr **)expr_info->tb);
		if (err < 0)
			goto err1;
	}

	return 0;
err1:
	expr->ops = NULL;
	return err;
}