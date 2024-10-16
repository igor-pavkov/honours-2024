static int nft_flush(struct nft_ctx *ctx, int family)
{
	struct nftables_pernet *nft_net = nft_pernet(ctx->net);
	const struct nlattr * const *nla = ctx->nla;
	struct nft_table *table, *nt;
	int err = 0;

	list_for_each_entry_safe(table, nt, &nft_net->tables, list) {
		if (family != AF_UNSPEC && table->family != family)
			continue;

		ctx->family = table->family;

		if (!nft_is_active_next(ctx->net, table))
			continue;

		if (nft_table_has_owner(table) && table->nlpid != ctx->portid)
			continue;

		if (nla[NFTA_TABLE_NAME] &&
		    nla_strcmp(nla[NFTA_TABLE_NAME], table->name) != 0)
			continue;

		ctx->table = table;

		err = nft_flush_table(ctx);
		if (err < 0)
			goto out;
	}
out:
	return err;
}