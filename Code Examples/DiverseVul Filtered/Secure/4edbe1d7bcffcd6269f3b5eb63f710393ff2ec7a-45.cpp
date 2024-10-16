static char *__change_cell_name(struct hash_cell *hc, char *new_name)
{
	char *old_name;

	/*
	 * Rename and move the name cell.
	 */
	list_del(&hc->name_list);
	old_name = hc->name;

	mutex_lock(&dm_hash_cells_mutex);
	hc->name = new_name;
	mutex_unlock(&dm_hash_cells_mutex);

	list_add(&hc->name_list, _name_buckets + hash_str(new_name));

	return old_name;
}