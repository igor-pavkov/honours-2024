static int __list_versions(struct dm_ioctl *param, size_t param_size, const char *name)
{
	size_t len, needed = 0;
	struct dm_target_versions *vers;
	struct vers_iter iter_info;
	struct target_type *tt = NULL;

	if (name) {
		tt = dm_get_target_type(name);
		if (!tt)
			return -EINVAL;
	}

	/*
	 * Loop through all the devices working out how much
	 * space we need.
	 */
	if (!tt)
		dm_target_iterate(list_version_get_needed, &needed);
	else
		list_version_get_needed(tt, &needed);

	/*
	 * Grab our output buffer.
	 */
	vers = get_result_buffer(param, param_size, &len);
	if (len < needed) {
		param->flags |= DM_BUFFER_FULL_FLAG;
		goto out;
	}
	param->data_size = param->data_start + needed;

	iter_info.param_size = param_size;
	iter_info.old_vers = NULL;
	iter_info.vers = vers;
	iter_info.flags = 0;
	iter_info.end = (char *)vers+len;

	/*
	 * Now loop through filling out the names & versions.
	 */
	if (!tt)
		dm_target_iterate(list_version_get_info, &iter_info);
	else
		list_version_get_info(tt, &iter_info);
	param->flags |= iter_info.flags;

 out:
	if (tt)
		dm_put_target_type(tt);
	return 0;
}