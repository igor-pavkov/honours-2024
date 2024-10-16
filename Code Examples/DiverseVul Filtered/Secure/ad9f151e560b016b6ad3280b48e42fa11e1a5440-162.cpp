static void nf_tables_set_elem_destroy(const struct nft_ctx *ctx,
				       const struct nft_set *set, void *elem)
{
	struct nft_set_ext *ext = nft_set_elem_ext(set, elem);

	if (nft_set_ext_exists(ext, NFT_SET_EXT_EXPRESSIONS))
		nft_set_elem_expr_destroy(ctx, nft_set_ext_expr(ext));

	kfree(elem);
}