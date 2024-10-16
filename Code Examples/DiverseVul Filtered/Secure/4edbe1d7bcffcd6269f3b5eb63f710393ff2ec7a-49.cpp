static int dm_hash_insert(const char *name, const char *uuid, struct mapped_device *md)
{
	struct hash_cell *cell, *hc;

	/*
	 * Allocate the new cells.
	 */
	cell = alloc_cell(name, uuid, md);
	if (!cell)
		return -ENOMEM;

	/*
	 * Insert the cell into both hash tables.
	 */
	down_write(&_hash_lock);
	hc = __get_name_cell(name);
	if (hc) {
		dm_put(hc->md);
		goto bad;
	}

	list_add(&cell->name_list, _name_buckets + hash_str(name));

	if (uuid) {
		hc = __get_uuid_cell(uuid);
		if (hc) {
			list_del(&cell->name_list);
			dm_put(hc->md);
			goto bad;
		}
		list_add(&cell->uuid_list, _uuid_buckets + hash_str(uuid));
	}
	dm_get(md);
	mutex_lock(&dm_hash_cells_mutex);
	dm_set_mdptr(md, cell);
	mutex_unlock(&dm_hash_cells_mutex);
	up_write(&_hash_lock);

	return 0;

 bad:
	up_write(&_hash_lock);
	free_cell(cell);
	return -EBUSY;
}