static int target_message(struct file *filp, struct dm_ioctl *param, size_t param_size)
{
	int r, argc;
	char **argv;
	struct mapped_device *md;
	struct dm_table *table;
	struct dm_target *ti;
	struct dm_target_msg *tmsg = (void *) param + param->data_start;
	size_t maxlen;
	char *result = get_result_buffer(param, param_size, &maxlen);
	int srcu_idx;

	md = find_device(param);
	if (!md)
		return -ENXIO;

	if (tmsg < (struct dm_target_msg *) param->data ||
	    invalid_str(tmsg->message, (void *) param + param_size)) {
		DMWARN("Invalid target message parameters.");
		r = -EINVAL;
		goto out;
	}

	r = dm_split_args(&argc, &argv, tmsg->message);
	if (r) {
		DMWARN("Failed to split target message parameters");
		goto out;
	}

	if (!argc) {
		DMWARN("Empty message received.");
		r = -EINVAL;
		goto out_argv;
	}

	r = message_for_md(md, argc, argv, result, maxlen);
	if (r <= 1)
		goto out_argv;

	table = dm_get_live_table(md, &srcu_idx);
	if (!table)
		goto out_table;

	if (dm_deleting_md(md)) {
		r = -ENXIO;
		goto out_table;
	}

	ti = dm_table_find_target(table, tmsg->sector);
	if (!ti) {
		DMWARN("Target message sector outside device.");
		r = -EINVAL;
	} else if (ti->type->message)
		r = ti->type->message(ti, argc, argv, result, maxlen);
	else {
		DMWARN("Target type does not support messages");
		r = -EINVAL;
	}

 out_table:
	dm_put_live_table(md, srcu_idx);
 out_argv:
	kfree(argv);
 out:
	if (r >= 0)
		__dev_status(md, param);

	if (r == 1) {
		param->flags |= DM_DATA_OUT_FLAG;
		if (dm_message_test_buffer_overflow(result, maxlen))
			param->flags |= DM_BUFFER_FULL_FLAG;
		else
			param->data_size = param->data_start + strlen(result) + 1;
		r = 0;
	}

	dm_put(md);
	return r;
}