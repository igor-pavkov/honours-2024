static int dev_rename(struct file *filp, struct dm_ioctl *param, size_t param_size)
{
	int r;
	char *new_data = (char *) param + param->data_start;
	struct mapped_device *md;
	unsigned change_uuid = (param->flags & DM_UUID_FLAG) ? 1 : 0;

	if (new_data < param->data ||
	    invalid_str(new_data, (void *) param + param_size) || !*new_data ||
	    strlen(new_data) > (change_uuid ? DM_UUID_LEN - 1 : DM_NAME_LEN - 1)) {
		DMWARN("Invalid new mapped device name or uuid string supplied.");
		return -EINVAL;
	}

	if (!change_uuid) {
		r = check_name(new_data);
		if (r)
			return r;
	}

	md = dm_hash_rename(param, new_data);
	if (IS_ERR(md))
		return PTR_ERR(md);

	__dev_status(md, param);
	dm_put(md);

	return 0;
}