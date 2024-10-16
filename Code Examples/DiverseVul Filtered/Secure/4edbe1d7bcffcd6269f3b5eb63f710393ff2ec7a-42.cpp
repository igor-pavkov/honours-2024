static void __set_cell_uuid(struct hash_cell *hc, char *new_uuid)
{
	mutex_lock(&dm_hash_cells_mutex);
	hc->uuid = new_uuid;
	mutex_unlock(&dm_hash_cells_mutex);

	list_add(&hc->uuid_list, _uuid_buckets + hash_str(new_uuid));
}