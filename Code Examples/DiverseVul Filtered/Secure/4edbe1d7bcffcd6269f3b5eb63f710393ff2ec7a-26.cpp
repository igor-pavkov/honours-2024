static inline fmode_t get_mode(struct dm_ioctl *param)
{
	fmode_t mode = FMODE_READ | FMODE_WRITE;

	if (param->flags & DM_READONLY_FLAG)
		mode = FMODE_READ;

	return mode;
}