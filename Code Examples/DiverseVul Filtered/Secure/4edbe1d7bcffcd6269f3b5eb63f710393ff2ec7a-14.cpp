static int do_suspend(struct dm_ioctl *param)
{
	int r = 0;
	unsigned suspend_flags = DM_SUSPEND_LOCKFS_FLAG;
	struct mapped_device *md;

	md = find_device(param);
	if (!md)
		return -ENXIO;

	if (param->flags & DM_SKIP_LOCKFS_FLAG)
		suspend_flags &= ~DM_SUSPEND_LOCKFS_FLAG;
	if (param->flags & DM_NOFLUSH_FLAG)
		suspend_flags |= DM_SUSPEND_NOFLUSH_FLAG;

	if (!dm_suspended_md(md)) {
		r = dm_suspend(md, suspend_flags);
		if (r)
			goto out;
	}

	__dev_status(md, param);

out:
	dm_put(md);

	return r;
}