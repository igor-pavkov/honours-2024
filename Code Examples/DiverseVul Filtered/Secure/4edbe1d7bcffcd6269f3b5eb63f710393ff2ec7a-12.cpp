static int dev_remove(struct file *filp, struct dm_ioctl *param, size_t param_size)
{
	struct hash_cell *hc;
	struct mapped_device *md;
	int r;
	struct dm_table *t;

	down_write(&_hash_lock);
	hc = __find_device_hash_cell(param);

	if (!hc) {
		DMDEBUG_LIMIT("device doesn't appear to be in the dev hash table.");
		up_write(&_hash_lock);
		return -ENXIO;
	}

	md = hc->md;

	/*
	 * Ensure the device is not open and nothing further can open it.
	 */
	r = dm_lock_for_deletion(md, !!(param->flags & DM_DEFERRED_REMOVE), false);
	if (r) {
		if (r == -EBUSY && param->flags & DM_DEFERRED_REMOVE) {
			up_write(&_hash_lock);
			dm_put(md);
			return 0;
		}
		DMDEBUG_LIMIT("unable to remove open device %s", hc->name);
		up_write(&_hash_lock);
		dm_put(md);
		return r;
	}

	t = __hash_remove(hc);
	up_write(&_hash_lock);

	if (t) {
		dm_sync_table(md);
		dm_table_destroy(t);
	}

	param->flags &= ~DM_DEFERRED_REMOVE;

	if (!dm_kobject_uevent(md, KOBJ_REMOVE, param->event_nr))
		param->flags |= DM_UEVENT_GENERATED_FLAG;

	dm_put(md);
	dm_destroy(md);
	return 0;
}