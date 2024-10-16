void nf_tables_destroy_set(const struct nft_ctx *ctx, struct nft_set *set)
{
	if (list_empty(&set->bindings) && nft_set_is_anonymous(set))
		nft_set_destroy(ctx, set);
}