static int nft_set_catchall_bind_check(const struct nft_ctx *ctx,
				       struct nft_set *set)
{
	u8 genmask = nft_genmask_next(ctx->net);
	struct nft_set_elem_catchall *catchall;
	struct nft_set_elem elem;
	struct nft_set_ext *ext;
	int ret = 0;

	list_for_each_entry_rcu(catchall, &set->catchall_list, list) {
		ext = nft_set_elem_ext(set, catchall->elem);
		if (!nft_set_elem_active(ext, genmask))
			continue;

		elem.priv = catchall->elem;
		ret = nft_setelem_data_validate(ctx, set, &elem);
		if (ret < 0)
			break;
	}

	return ret;
}