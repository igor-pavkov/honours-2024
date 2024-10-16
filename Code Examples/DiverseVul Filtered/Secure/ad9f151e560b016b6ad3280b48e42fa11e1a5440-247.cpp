static int nft_set_catchall_loops(const struct nft_ctx *ctx,
				  struct nft_set *set)
{
	u8 genmask = nft_genmask_next(ctx->net);
	struct nft_set_elem_catchall *catchall;
	struct nft_set_ext *ext;
	int ret = 0;

	list_for_each_entry_rcu(catchall, &set->catchall_list, list) {
		ext = nft_set_elem_ext(set, catchall->elem);
		if (!nft_set_elem_active(ext, genmask))
			continue;

		ret = nft_check_loops(ctx, ext);
		if (ret < 0)
			return ret;
	}

	return ret;
}