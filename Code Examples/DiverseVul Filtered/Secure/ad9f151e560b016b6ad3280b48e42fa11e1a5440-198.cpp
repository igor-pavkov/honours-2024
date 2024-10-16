static int nf_tables_dump_flowtable_start(struct netlink_callback *cb)
{
	const struct nlattr * const *nla = cb->data;
	struct nft_flowtable_filter *filter = NULL;

	if (nla[NFTA_FLOWTABLE_TABLE]) {
		filter = kzalloc(sizeof(*filter), GFP_ATOMIC);
		if (!filter)
			return -ENOMEM;

		filter->table = nla_strdup(nla[NFTA_FLOWTABLE_TABLE],
					   GFP_ATOMIC);
		if (!filter->table) {
			kfree(filter);
			return -ENOMEM;
		}
	}

	cb->data = filter;
	return 0;
}