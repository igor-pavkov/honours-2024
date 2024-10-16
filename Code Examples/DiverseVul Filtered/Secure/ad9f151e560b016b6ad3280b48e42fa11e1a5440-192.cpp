static int nf_tables_commit_audit_alloc(struct list_head *adl,
					struct nft_table *table)
{
	struct nft_audit_data *adp;

	list_for_each_entry(adp, adl, list) {
		if (adp->table == table)
			return 0;
	}
	adp = kzalloc(sizeof(*adp), GFP_KERNEL);
	if (!adp)
		return -ENOMEM;
	adp->table = table;
	list_add(&adp->list, adl);
	return 0;
}