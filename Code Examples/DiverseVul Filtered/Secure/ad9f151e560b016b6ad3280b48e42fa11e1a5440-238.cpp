struct nft_flowtable *nft_flowtable_lookup(const struct nft_table *table,
					   const struct nlattr *nla, u8 genmask)
{
	struct nft_flowtable *flowtable;

	list_for_each_entry_rcu(flowtable, &table->flowtables, list) {
		if (!nla_strcmp(nla, flowtable->name) &&
		    nft_active_genmask(flowtable, genmask))
			return flowtable;
	}
	return ERR_PTR(-ENOENT);
}