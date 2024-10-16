static int dev_create(struct file *filp, struct dm_ioctl *param, size_t param_size)
{
	int r, m = DM_ANY_MINOR;
	struct mapped_device *md;

	r = check_name(param->name);
	if (r)
		return r;

	if (param->flags & DM_PERSISTENT_DEV_FLAG)
		m = MINOR(huge_decode_dev(param->dev));

	r = dm_create(m, &md);
	if (r)
		return r;

	r = dm_hash_insert(param->name, *param->uuid ? param->uuid : NULL, md);
	if (r) {
		dm_put(md);
		dm_destroy(md);
		return r;
	}

	param->flags &= ~DM_INACTIVE_PRESENT_FLAG;

	__dev_status(md, param);

	dm_put(md);

	return 0;
}