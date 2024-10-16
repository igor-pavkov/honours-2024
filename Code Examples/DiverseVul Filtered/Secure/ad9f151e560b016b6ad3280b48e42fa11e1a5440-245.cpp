static int nf_tables_dump_set_done(struct netlink_callback *cb)
{
	kfree(cb->data);
	return 0;
}