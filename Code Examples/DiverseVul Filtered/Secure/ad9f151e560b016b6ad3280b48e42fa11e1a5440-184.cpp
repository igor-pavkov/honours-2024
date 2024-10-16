static void nft_set_destroy(const struct nft_ctx *ctx, struct nft_set *set)
{
	int i;

	if (WARN_ON(set->use > 0))
		return;

	for (i = 0; i < set->num_exprs; i++)
		nft_expr_destroy(ctx, set->exprs[i]);

	set->ops->destroy(set);
	nft_set_catchall_destroy(ctx, set);
	kfree(set->name);
	kvfree(set);
}