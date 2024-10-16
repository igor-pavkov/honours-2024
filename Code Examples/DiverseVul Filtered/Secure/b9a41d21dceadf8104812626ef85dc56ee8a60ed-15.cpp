sector_t dm_get_size(struct mapped_device *md)
{
	return get_capacity(md->disk);
}