static int nft_setelem_parse_key(struct nft_ctx *ctx, struct nft_set *set,
				 struct nft_data *key, struct nlattr *attr)
{
	struct nft_data_desc desc;
	int err;

	err = nft_data_init(ctx, key, NFT_DATA_VALUE_MAXLEN, &desc, attr);
	if (err < 0)
		return err;

	if (desc.type != NFT_DATA_VALUE || desc.len != set->klen) {
		nft_data_release(key, desc.type);
		return -EINVAL;
	}

	return 0;
}