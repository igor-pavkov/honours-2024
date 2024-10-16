static int nft_setelem_parse_data(struct nft_ctx *ctx, struct nft_set *set,
				  struct nft_data_desc *desc,
				  struct nft_data *data,
				  struct nlattr *attr)
{
	u32 dtype;
	int err;

	err = nft_data_init(ctx, data, NFT_DATA_VALUE_MAXLEN, desc, attr);
	if (err < 0)
		return err;

	if (set->dtype == NFT_DATA_VERDICT)
		dtype = NFT_DATA_VERDICT;
	else
		dtype = NFT_DATA_VALUE;

	if (dtype != desc->type ||
	    set->dlen != desc->len) {
		nft_data_release(data, desc->type);
		return -EINVAL;
	}

	return 0;
}