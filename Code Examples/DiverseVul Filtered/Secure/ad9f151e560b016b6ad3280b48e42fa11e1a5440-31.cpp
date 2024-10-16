static void __nft_set_elem_expr_destroy(const struct nft_ctx *ctx,
					struct nft_expr *expr)
{
	if (expr->ops->destroy_clone) {
		expr->ops->destroy_clone(ctx, expr);
		module_put(expr->ops->type->owner);
	} else {
		nf_tables_expr_destroy(ctx, expr);
	}
}