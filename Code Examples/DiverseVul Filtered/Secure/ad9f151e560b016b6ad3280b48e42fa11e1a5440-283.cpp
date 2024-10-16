static int nft_set_catchall_flush(const struct nft_ctx *ctx,
				  struct nft_set *set)
{
	u8 genmask = nft_genmask_next(ctx->net);
	struct nft_set_elem_catchall *catchall;
	struct nft_set_elem elem;
	struct nft_set_ext *ext;
	int ret = 0;

	list_for_each_entry_rcu(catchall, &set->catchall_list, list) {
		ext = nft_set_elem_ext(set, catchall->elem);
		if (!nft_set_elem_active(ext, genmask) ||
		    nft_set_elem_mark_busy(ext))
			continue;

		elem.priv = catchall->elem;
		ret = __nft_set_catchall_flush(ctx, set, &elem);
		if (ret < 0)
			break;
	}

	return ret;
}