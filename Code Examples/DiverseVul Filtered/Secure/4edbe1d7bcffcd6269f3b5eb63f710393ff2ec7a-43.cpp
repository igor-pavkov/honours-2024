static void *get_result_buffer(struct dm_ioctl *param, size_t param_size,
			       size_t *len)
{
	param->data_start = align_ptr(param + 1) - (void *) param;

	if (param->data_start < param_size)
		*len = param_size - param->data_start;
	else
		*len = 0;

	return ((void *) param) + param->data_start;
}