static int nft_check_loops(const struct nft_ctx *ctx,
			   const struct nft_set_ext *ext)
{
	const struct nft_data *data;
	int ret;

	data = nft_set_ext_data(ext);
	switch (data->verdict.code) {
	case NFT_JUMP:
	case NFT_GOTO:
		ret = nf_tables_check_loops(ctx, data->verdict.chain);
		break;
	default:
		ret = 0;
		break;
	}

	return ret;
}