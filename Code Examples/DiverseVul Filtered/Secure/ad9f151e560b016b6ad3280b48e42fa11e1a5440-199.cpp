static void nft_set_elem_expr_destroy(const struct nft_ctx *ctx,
				      struct nft_set_elem_expr *elem_expr)
{
	struct nft_expr *expr;
	u32 size;

	nft_setelem_expr_foreach(expr, elem_expr, size)
		__nft_set_elem_expr_destroy(ctx, expr);
}