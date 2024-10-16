static int ip_vs_rs_unhash(struct ip_vs_dest *dest)
{
	/*
	 * Remove it from the rs_table table.
	 */
	if (!list_empty(&dest->d_list)) {
		list_del(&dest->d_list);
		INIT_LIST_HEAD(&dest->d_list);
	}

	return 1;
}