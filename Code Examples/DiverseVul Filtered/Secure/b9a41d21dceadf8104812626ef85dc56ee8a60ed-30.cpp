void dm_destroy(struct mapped_device *md)
{
	__dm_destroy(md, true);
}