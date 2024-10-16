int dm_copy_name_and_uuid(struct mapped_device *md, char *name, char *uuid)
{
	int r = 0;
	struct hash_cell *hc;

	if (!md)
		return -ENXIO;

	mutex_lock(&dm_hash_cells_mutex);
	hc = dm_get_mdptr(md);
	if (!hc || hc->md != md) {
		r = -ENXIO;
		goto out;
	}

	if (name)
		strcpy(name, hc->name);
	if (uuid)
		strcpy(uuid, hc->uuid ? : "");

out:
	mutex_unlock(&dm_hash_cells_mutex);

	return r;
}