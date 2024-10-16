static struct dm_table *__hash_remove(struct hash_cell *hc)
{
	struct dm_table *table;
	int srcu_idx;

	/* remove from the dev hash */
	list_del(&hc->uuid_list);
	list_del(&hc->name_list);
	mutex_lock(&dm_hash_cells_mutex);
	dm_set_mdptr(hc->md, NULL);
	mutex_unlock(&dm_hash_cells_mutex);

	table = dm_get_live_table(hc->md, &srcu_idx);
	if (table)
		dm_table_event(table);
	dm_put_live_table(hc->md, srcu_idx);

	table = NULL;
	if (hc->new_map)
		table = hc->new_map;
	dm_put(hc->md);
	free_cell(hc);

	return table;
}