static int table_load(struct file *filp, struct dm_ioctl *param, size_t param_size)
{
	int r;
	struct hash_cell *hc;
	struct dm_table *t, *old_map = NULL;
	struct mapped_device *md;
	struct target_type *immutable_target_type;

	md = find_device(param);
	if (!md)
		return -ENXIO;

	r = dm_table_create(&t, get_mode(param), param->target_count, md);
	if (r)
		goto err;

	/* Protect md->type and md->queue against concurrent table loads. */
	dm_lock_md_type(md);
	r = populate_table(t, param, param_size);
	if (r)
		goto err_unlock_md_type;

	immutable_target_type = dm_get_immutable_target_type(md);
	if (immutable_target_type &&
	    (immutable_target_type != dm_table_get_immutable_target_type(t)) &&
	    !dm_table_get_wildcard_target(t)) {
		DMWARN("can't replace immutable target type %s",
		       immutable_target_type->name);
		r = -EINVAL;
		goto err_unlock_md_type;
	}

	if (dm_get_md_type(md) == DM_TYPE_NONE) {
		/* Initial table load: acquire type of table. */
		dm_set_md_type(md, dm_table_get_type(t));

		/* setup md->queue to reflect md's type (may block) */
		r = dm_setup_md_queue(md, t);
		if (r) {
			DMWARN("unable to set up device queue for new table.");
			goto err_unlock_md_type;
		}
	} else if (!is_valid_type(dm_get_md_type(md), dm_table_get_type(t))) {
		DMWARN("can't change device type (old=%u vs new=%u) after initial table load.",
		       dm_get_md_type(md), dm_table_get_type(t));
		r = -EINVAL;
		goto err_unlock_md_type;
	}

	dm_unlock_md_type(md);

	/* stage inactive table */
	down_write(&_hash_lock);
	hc = dm_get_mdptr(md);
	if (!hc || hc->md != md) {
		DMWARN("device has been removed from the dev hash table.");
		up_write(&_hash_lock);
		r = -ENXIO;
		goto err_destroy_table;
	}

	if (hc->new_map)
		old_map = hc->new_map;
	hc->new_map = t;
	up_write(&_hash_lock);

	param->flags |= DM_INACTIVE_PRESENT_FLAG;
	__dev_status(md, param);

	if (old_map) {
		dm_sync_table(md);
		dm_table_destroy(old_map);
	}

	dm_put(md);

	return 0;

err_unlock_md_type:
	dm_unlock_md_type(md);
err_destroy_table:
	dm_table_destroy(t);
err:
	dm_put(md);

	return r;
}