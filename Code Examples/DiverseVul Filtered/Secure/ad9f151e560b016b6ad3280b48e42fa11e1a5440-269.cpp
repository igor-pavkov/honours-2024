static int nf_tables_dump_obj_start(struct netlink_callback *cb)
{
	const struct nlattr * const *nla = cb->data;
	struct nft_obj_filter *filter = NULL;

	if (nla[NFTA_OBJ_TABLE] || nla[NFTA_OBJ_TYPE]) {
		filter = kzalloc(sizeof(*filter), GFP_ATOMIC);
		if (!filter)
			return -ENOMEM;

		if (nla[NFTA_OBJ_TABLE]) {
			filter->table = nla_strdup(nla[NFTA_OBJ_TABLE], GFP_ATOMIC);
			if (!filter->table) {
				kfree(filter);
				return -ENOMEM;
			}
		}

		if (nla[NFTA_OBJ_TYPE])
			filter->type = ntohl(nla_get_be32(nla[NFTA_OBJ_TYPE]));
	}

	cb->data = filter;
	return 0;
}