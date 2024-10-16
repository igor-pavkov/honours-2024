static int dev_suspend(struct file *filp, struct dm_ioctl *param, size_t param_size)
{
	if (param->flags & DM_SUSPEND_FLAG)
		return do_suspend(param);

	return do_resume(param);
}