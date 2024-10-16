static int remove_all(struct file *filp, struct dm_ioctl *param, size_t param_size)
{
	dm_hash_remove_all(true, !!(param->flags & DM_DEFERRED_REMOVE), false);
	param->data_size = 0;
	return 0;
}