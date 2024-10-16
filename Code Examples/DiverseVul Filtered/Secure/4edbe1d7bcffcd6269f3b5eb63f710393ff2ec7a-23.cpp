static void free_params(struct dm_ioctl *param, size_t param_size, int param_flags)
{
	if (param_flags & DM_WIPE_BUFFER)
		memset(param, 0, param_size);

	if (param_flags & DM_PARAMS_MALLOC)
		kvfree(param);
}