static void nf_tables_expr_destroy(const struct nft_ctx *ctx,
				   struct nft_expr *expr)
{
	const struct nft_expr_type *type = expr->ops->type;

	if (expr->ops->destroy)
		expr->ops->destroy(ctx, expr);
	module_put(type->owner);
}