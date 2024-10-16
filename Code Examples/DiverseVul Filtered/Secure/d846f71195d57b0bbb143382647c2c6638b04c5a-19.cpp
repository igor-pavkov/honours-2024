void ebt_unregister_table(struct net *net, struct ebt_table *table)
{
	int i;

	if (!table) {
		BUGPRINT("Request to unregister NULL table!!!\n");
		return;
	}
	mutex_lock(&ebt_mutex);
	list_del(&table->list);
	mutex_unlock(&ebt_mutex);
	EBT_ENTRY_ITERATE(table->private->entries, table->private->entries_size,
			  ebt_cleanup_entry, net, NULL);
	if (table->private->nentries)
		module_put(table->me);
	vfree(table->private->entries);
	if (table->private->chainstack) {
		for_each_possible_cpu(i)
			vfree(table->private->chainstack[i]);
		vfree(table->private->chainstack);
	}
	vfree(table->private);
	kfree(table);
}