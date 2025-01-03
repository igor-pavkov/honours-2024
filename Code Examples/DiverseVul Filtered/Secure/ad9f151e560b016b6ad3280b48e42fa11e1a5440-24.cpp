static struct nft_expr *nft_expr_init(const struct nft_ctx *ctx,
				      const struct nlattr *nla)
{
	struct nft_expr_info expr_info;
	struct nft_expr *expr;
	struct module *owner;
	int err;

	err = nf_tables_expr_parse(ctx, nla, &expr_info);
	if (err < 0)
		goto err1;

	err = -ENOMEM;
	expr = kzalloc(expr_info.ops->size, GFP_KERNEL);
	if (expr == NULL)
		goto err2;

	err = nf_tables_newexpr(ctx, &expr_info, expr);
	if (err < 0)
		goto err3;

	return expr;
err3:
	kfree(expr);
err2:
	owner = expr_info.ops->type->owner;
	if (expr_info.ops->type->release_ops)
		expr_info.ops->type->release_ops(expr_info.ops);

	module_put(owner);
err1:
	return ERR_PTR(err);
}