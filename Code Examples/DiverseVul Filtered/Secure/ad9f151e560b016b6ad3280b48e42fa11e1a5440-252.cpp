static void nf_tables_commit_audit_collect(struct list_head *adl,
					   struct nft_table *table, u32 op)
{
	struct nft_audit_data *adp;

	list_for_each_entry(adp, adl, list) {
		if (adp->table == table)
			goto found;
	}
	WARN_ONCE(1, "table=%s not expected in commit list", table->name);
	return;
found:
	adp->entries++;
	if (!adp->op || adp->op > op)
		adp->op = op;
}