static struct mapped_device *dm_hash_rename(struct dm_ioctl *param,
					    const char *new)
{
	char *new_data, *old_name = NULL;
	struct hash_cell *hc;
	struct dm_table *table;
	struct mapped_device *md;
	unsigned change_uuid = (param->flags & DM_UUID_FLAG) ? 1 : 0;
	int srcu_idx;

	/*
	 * duplicate new.
	 */
	new_data = kstrdup(new, GFP_KERNEL);
	if (!new_data)
		return ERR_PTR(-ENOMEM);

	down_write(&_hash_lock);

	/*
	 * Is new free ?
	 */
	if (change_uuid)
		hc = __get_uuid_cell(new);
	else
		hc = __get_name_cell(new);

	if (hc) {
		DMWARN("Unable to change %s on mapped device %s to one that "
		       "already exists: %s",
		       change_uuid ? "uuid" : "name",
		       param->name, new);
		dm_put(hc->md);
		up_write(&_hash_lock);
		kfree(new_data);
		return ERR_PTR(-EBUSY);
	}

	/*
	 * Is there such a device as 'old' ?
	 */
	hc = __get_name_cell(param->name);
	if (!hc) {
		DMWARN("Unable to rename non-existent device, %s to %s%s",
		       param->name, change_uuid ? "uuid " : "", new);
		up_write(&_hash_lock);
		kfree(new_data);
		return ERR_PTR(-ENXIO);
	}

	/*
	 * Does this device already have a uuid?
	 */
	if (change_uuid && hc->uuid) {
		DMWARN("Unable to change uuid of mapped device %s to %s "
		       "because uuid is already set to %s",
		       param->name, new, hc->uuid);
		dm_put(hc->md);
		up_write(&_hash_lock);
		kfree(new_data);
		return ERR_PTR(-EINVAL);
	}

	if (change_uuid)
		__set_cell_uuid(hc, new_data);
	else
		old_name = __change_cell_name(hc, new_data);

	/*
	 * Wake up any dm event waiters.
	 */
	table = dm_get_live_table(hc->md, &srcu_idx);
	if (table)
		dm_table_event(table);
	dm_put_live_table(hc->md, srcu_idx);

	if (!dm_kobject_uevent(hc->md, KOBJ_CHANGE, param->event_nr))
		param->flags |= DM_UEVENT_GENERATED_FLAG;

	md = hc->md;
	up_write(&_hash_lock);
	kfree(old_name);

	return md;
}