int __init dm_early_create(struct dm_ioctl *dmi,
			   struct dm_target_spec **spec_array,
			   char **target_params_array)
{
	int r, m = DM_ANY_MINOR;
	struct dm_table *t, *old_map;
	struct mapped_device *md;
	unsigned int i;

	if (!dmi->target_count)
		return -EINVAL;

	r = check_name(dmi->name);
	if (r)
		return r;

	if (dmi->flags & DM_PERSISTENT_DEV_FLAG)
		m = MINOR(huge_decode_dev(dmi->dev));

	/* alloc dm device */
	r = dm_create(m, &md);
	if (r)
		return r;

	/* hash insert */
	r = dm_hash_insert(dmi->name, *dmi->uuid ? dmi->uuid : NULL, md);
	if (r)
		goto err_destroy_dm;

	/* alloc table */
	r = dm_table_create(&t, get_mode(dmi), dmi->target_count, md);
	if (r)
		goto err_hash_remove;

	/* add targets */
	for (i = 0; i < dmi->target_count; i++) {
		r = dm_table_add_target(t, spec_array[i]->target_type,
					(sector_t) spec_array[i]->sector_start,
					(sector_t) spec_array[i]->length,
					target_params_array[i]);
		if (r) {
			DMWARN("error adding target to table");
			goto err_destroy_table;
		}
	}

	/* finish table */
	r = dm_table_complete(t);
	if (r)
		goto err_destroy_table;

	md->type = dm_table_get_type(t);
	/* setup md->queue to reflect md's type (may block) */
	r = dm_setup_md_queue(md, t);
	if (r) {
		DMWARN("unable to set up device queue for new table.");
		goto err_destroy_table;
	}

	/* Set new map */
	dm_suspend(md, 0);
	old_map = dm_swap_table(md, t);
	if (IS_ERR(old_map)) {
		r = PTR_ERR(old_map);
		goto err_destroy_table;
	}
	set_disk_ro(dm_disk(md), !!(dmi->flags & DM_READONLY_FLAG));

	/* resume device */
	r = dm_resume(md);
	if (r)
		goto err_destroy_table;

	DMINFO("%s (%s) is ready", md->disk->disk_name, dmi->name);
	dm_put(md);
	return 0;

err_destroy_table:
	dm_table_destroy(t);
err_hash_remove:
	(void) __hash_remove(__get_name_cell(dmi->name));
	/* release reference from __get_name_cell */
	dm_put(md);
err_destroy_dm:
	dm_put(md);
	dm_destroy(md);
	return r;
}