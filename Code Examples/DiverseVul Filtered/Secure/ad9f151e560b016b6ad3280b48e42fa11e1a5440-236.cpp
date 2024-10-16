static void nf_tables_commit_audit_log(struct list_head *adl, u32 generation)
{
	struct nft_audit_data *adp, *adn;
	char aubuf[AUNFTABLENAMELEN];

	list_for_each_entry_safe(adp, adn, adl, list) {
		snprintf(aubuf, AUNFTABLENAMELEN, "%s:%u", adp->table->name,
			 generation);
		audit_log_nfcfg(aubuf, adp->table->family, adp->entries,
				nft2audit_op[adp->op], GFP_KERNEL);
		list_del(&adp->list);
		kfree(adp);
	}
}