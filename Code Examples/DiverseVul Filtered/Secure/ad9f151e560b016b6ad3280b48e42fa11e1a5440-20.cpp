static int nft_table_validate(struct net *net, const struct nft_table *table)
{
	struct nft_chain *chain;
	struct nft_ctx ctx = {
		.net	= net,
		.family	= table->family,
	};
	int err;

	list_for_each_entry(chain, &table->chains, list) {
		if (!nft_is_base_chain(chain))
			continue;

		ctx.chain = chain;
		err = nft_chain_validate(&ctx, chain);
		if (err < 0)
			return err;
	}

	return 0;
}