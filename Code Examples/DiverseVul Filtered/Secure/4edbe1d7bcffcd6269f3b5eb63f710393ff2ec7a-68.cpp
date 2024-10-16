static int do_resume(struct dm_ioctl *param)
{
	int r = 0;
	unsigned suspend_flags = DM_SUSPEND_LOCKFS_FLAG;
	struct hash_cell *hc;
	struct mapped_device *md;
	struct dm_table *new_map, *old_map = NULL;

	down_write(&_hash_lock);

	hc = __find_device_hash_cell(param);
	if (!hc) {
		DMDEBUG_LIMIT("device doesn't appear to be in the dev hash table.");
		up_write(&_hash_lock);
		return -ENXIO;
	}

	md = hc->md;

	new_map = hc->new_map;
	hc->new_map = NULL;
	param->flags &= ~DM_INACTIVE_PRESENT_FLAG;

	up_write(&_hash_lock);

	/* Do we need to load a new map ? */
	if (new_map) {
		/* Suspend if it isn't already suspended */
		if (param->flags & DM_SKIP_LOCKFS_FLAG)
			suspend_flags &= ~DM_SUSPEND_LOCKFS_FLAG;
		if (param->flags & DM_NOFLUSH_FLAG)
			suspend_flags |= DM_SUSPEND_NOFLUSH_FLAG;
		if (!dm_suspended_md(md))
			dm_suspend(md, suspend_flags);

		old_map = dm_swap_table(md, new_map);
		if (IS_ERR(old_map)) {
			dm_sync_table(md);
			dm_table_destroy(new_map);
			dm_put(md);
			return PTR_ERR(old_map);
		}

		if (dm_table_get_mode(new_map) & FMODE_WRITE)
			set_disk_ro(dm_disk(md), 0);
		else
			set_disk_ro(dm_disk(md), 1);
	}

	if (dm_suspended_md(md)) {
		r = dm_resume(md);
		if (!r && !dm_kobject_uevent(md, KOBJ_CHANGE, param->event_nr))
			param->flags |= DM_UEVENT_GENERATED_FLAG;
	}

	/*
	 * Since dm_swap_table synchronizes RCU, nobody should be in
	 * read-side critical section already.
	 */
	if (old_map)
		dm_table_destroy(old_map);

	if (!r)
		__dev_status(md, param);

	dm_put(md);
	return r;
}