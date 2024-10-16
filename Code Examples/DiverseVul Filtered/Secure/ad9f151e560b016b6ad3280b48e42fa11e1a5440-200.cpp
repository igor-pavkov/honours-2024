void nft_register_flowtable_type(struct nf_flowtable_type *type)
{
	nfnl_lock(NFNL_SUBSYS_NFTABLES);
	list_add_tail_rcu(&type->list, &nf_tables_flowtables);
	nfnl_unlock(NFNL_SUBSYS_NFTABLES);
}