int dm_open_count(struct mapped_device *md)
{
	return atomic_read(&md->open_count);
}