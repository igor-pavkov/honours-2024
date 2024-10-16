static struct nft_set *nft_set_lookup_byhandle(const struct nft_table *table,
					       const struct nlattr *nla,
					       u8 genmask)
{
	struct nft_set *set;

	list_for_each_entry(set, &table->sets, list) {
		if (be64_to_cpu(nla_get_be64(nla)) == set->handle &&
		    nft_active_genmask(set, genmask))
			return set;
	}
	return ERR_PTR(-ENOENT);
}