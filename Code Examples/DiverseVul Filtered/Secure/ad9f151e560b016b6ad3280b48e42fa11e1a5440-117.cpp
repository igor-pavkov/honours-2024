static int nf_tables_dump_flowtable_done(struct netlink_callback *cb)
{
	struct nft_flowtable_filter *filter = cb->data;

	if (!filter)
		return 0;

	kfree(filter->table);
	kfree(filter);

	return 0;
}